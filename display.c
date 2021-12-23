#include "ble_hrs_c.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_gq.h"
#include "nrf_ble_scan.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "app_timer.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "ws2812b_drive.h"

#define SCAN_DURATION_WITELIST      3000                                /**< Duration of the scanning in units of 10 milliseconds. */
#define TARGET_UUID                 BLE_UUID_HEART_RATE_SERVICE         /**< Target device uuid that application is looking for. */

uint16_t g_current_heart_rate = 0;
uint8_t g_clock = 0;

APP_TIMER_DEF(led_timer);

NRF_BLE_GQ_DEF(m_ble_gatt_queue,                                    /**< BLE GATT Queue instance. */
               NRF_SDH_BLE_CENTRAL_LINK_COUNT,
               NRF_BLE_GQ_QUEUE_SIZE);
BLE_HRS_C_DEF(m_hrs_c);                                             /**< Structure used to identify the heart rate client module. */
NRF_BLE_GATT_DEF(m_gatt);                                           /**< GATT module instanc. */
BLE_DB_DISCOVERY_DEF(m_db_disc);                                    /**< DB discovery module instance. */
NRF_BLE_SCAN_DEF(m_scan)                                           /**< Scanning module instance. */

#define LED_STRIP_PIN 13
#define NUM_LEDS 35
rgb_led_t led_array[NUM_LEDS];

// static uint16_t m_conn_handle;                                      /**< Current connection handle. */
// static bool     m_memory_access_in_progress;                        /**< Flag to keep track of ongoing operations on persistent memory. */

/**< Scan parameters requested for scanning and connection. */
static ble_gap_scan_params_t const m_scan_param =
{
    .active        = 0x01,
#if (NRF_SD_BLE_API_VERSION > 7)
    .interval_us   = NRF_BLE_SCAN_SCAN_INTERVAL * UNIT_0_625_MS,
    .window_us     = NRF_BLE_SCAN_SCAN_WINDOW * UNIT_0_625_MS,
#else
    .interval      = NRF_BLE_SCAN_SCAN_INTERVAL,
    .window        = NRF_BLE_SCAN_SCAN_WINDOW,
#endif // (NRF_SD_BLE_API_VERSION > 7)
    .filter_policy = BLE_GAP_SCAN_FP_ACCEPT_ALL,
    .timeout       = SCAN_DURATION_WITELIST,
    .scan_phys     = BLE_GAP_PHY_AUTO,
};


static void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
    switch (p_evt->evt_id)
    {
        case NRF_BLE_GATT_EVT_ATT_MTU_UPDATED:
        {
            NRF_LOG_INFO("GATT ATT MTU on connection 0x%x changed to %d.",
                         p_evt->conn_handle,
                         p_evt->params.att_mtu_effective);
        } break;

        case NRF_BLE_GATT_EVT_DATA_LENGTH_UPDATED:
        {
            NRF_LOG_INFO("Data length for connection 0x%x updated to %d.",
                         p_evt->conn_handle,
                         p_evt->params.data_length);
        } break;

        default:
            break;
    }
}

static void scan_start(void)
{
    ret_code_t err_code;

    // if (nrf_fstorage_is_busy(NULL))
    // {
    //     m_memory_access_in_progress = true;
    //     return;
    // }

    NRF_LOG_INFO("Starting scan.");

    err_code = nrf_ble_scan_start(&m_scan);
    APP_ERROR_CHECK(err_code);
}

static void scan_evt_handler(scan_evt_t const * p_scan_evt)
{
    ret_code_t err_code;
    switch(p_scan_evt->scan_evt_id)
    {
        case NRF_BLE_SCAN_EVT_CONNECTED:
        {
            NRF_LOG_INFO("Connected!");
        } break;

        case NRF_BLE_SCAN_EVT_CONNECTING_ERROR:
        {
            err_code = p_scan_evt->params.connecting_err.err_code;
            APP_ERROR_CHECK(err_code);
        } break;

        case NRF_BLE_SCAN_EVT_SCAN_TIMEOUT:
        {
            NRF_LOG_INFO("Scan timed out.");
            scan_start();
        } break;

        case NRF_BLE_SCAN_EVT_NOT_FOUND:
        {
        } break;

        // case NRF_BLE_SCAN_EVT_FILTER_MATCH:
        //     break;
        // case NRF_BLE_SCAN_EVT_WHITELIST_ADV_REPORT:
        //     break;

        default:
            NRF_LOG_INFO("scan evt: %d", p_scan_evt->scan_evt_id);
          break;
    }
}

static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_gap_evt_t const * p_gap_evt = &p_ble_evt->evt.gap_evt;
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
        {
            APP_ERROR_CHECK(ble_db_discovery_start(&m_db_disc, p_ble_evt->evt.gap_evt.conn_handle));
        } break;
        case BLE_GAP_EVT_ADV_REPORT:
            break;
        case BLE_GAP_EVT_DISCONNECTED:
        {
            NRF_LOG_INFO("Disconnected, reason 0x%x.",
                         p_gap_evt->params.disconnected.reason);

            scan_start();
        } break;
        default:
          NRF_LOG_INFO("ble evt %d", p_ble_evt->header.evt_id);
            break;
    }
}

static void scan_init(void)
{
    ret_code_t          err_code;
    nrf_ble_scan_init_t init_scan;

    memset(&init_scan, 0, sizeof(init_scan));

    init_scan.p_scan_param     = &m_scan_param;
    init_scan.connect_if_match = true;
    init_scan.conn_cfg_tag     = BLE_CONN_CFG_TAG_DEFAULT;

    err_code = nrf_ble_scan_init(&m_scan, &init_scan, scan_evt_handler);
    APP_ERROR_CHECK(err_code);

    ble_uuid_t uuid =
    {
        .uuid = TARGET_UUID,
        .type = BLE_UUID_TYPE_BLE,
    };

    err_code = nrf_ble_scan_filter_set(&m_scan,
                                       SCAN_UUID_FILTER,
                                       &uuid);
    APP_ERROR_CHECK(err_code);

    // if (strlen(m_target_periph_name) != 0)
    // {
    //     err_code = nrf_ble_scan_filter_set(&m_scan,
    //                                        SCAN_NAME_FILTER,
    //                                        m_target_periph_name);
    //     APP_ERROR_CHECK(err_code);
    // }

    // if (is_connect_per_addr)
    // {
    //    err_code = nrf_ble_scan_filter_set(&m_scan,
    //                                       SCAN_ADDR_FILTER,
    //                                       m_target_periph_addr.addr);
    //    APP_ERROR_CHECK(err_code);
    // }

    err_code = nrf_ble_scan_filters_enable(&m_scan,
                                           NRF_BLE_SCAN_ALL_FILTER,
                                           false);
    APP_ERROR_CHECK(err_code);

}

static void hrs_c_evt_handler(ble_hrs_c_t * p_hrs_c, ble_hrs_c_evt_t * p_hrs_c_evt)
{
    ret_code_t err_code;

    switch (p_hrs_c_evt->evt_type)
    {
        case BLE_HRS_C_EVT_DISCOVERY_COMPLETE:
        {
            NRF_LOG_DEBUG("Heart rate service discovered.");

            err_code = ble_hrs_c_handles_assign(p_hrs_c,
                                                p_hrs_c_evt->conn_handle,
                                                &p_hrs_c_evt->params.peer_db);
            APP_ERROR_CHECK(err_code);

            // Heart rate service discovered. Enable notification of Heart Rate Measurement.
            err_code = ble_hrs_c_hrm_notif_enable(p_hrs_c);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_HRS_C_EVT_HRM_NOTIFICATION:
        {
            NRF_LOG_INFO("Heart Rate = %d.", p_hrs_c_evt->params.hrm.hr_value);
            g_current_heart_rate = p_hrs_c_evt->params.hrm.hr_value;

            if (p_hrs_c_evt->params.hrm.rr_intervals_cnt != 0)
            {
                uint32_t rr_avg = 0;
                for (uint32_t i = 0; i < p_hrs_c_evt->params.hrm.rr_intervals_cnt; i++)
                {
                    rr_avg += p_hrs_c_evt->params.hrm.rr_intervals[i];
                }
                rr_avg = rr_avg / p_hrs_c_evt->params.hrm.rr_intervals_cnt;
                NRF_LOG_DEBUG("rr_interval (avg) = %d.", rr_avg);
            }
        } break;

        default:
            break;
    }
}

static void db_disc_handler(ble_db_discovery_evt_t * p_evt)
{
    ble_hrs_on_db_disc_evt(&m_hrs_c, p_evt);
}

static void service_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

static void do_leds(void * p_context)
{
    g_clock += g_current_heart_rate/60*255*10 / 2;
    for (int i = 0; i < NUM_LEDS; i++) {
      led_array[i].red = g_current_heart_rate ? g_clock / 32 : 10;
      // led_array[i].green += 1;
      // led_array[i].blue += 1;
    }
    APP_ERROR_CHECK(i2s_ws2812b_drive_xfer(led_array, NUM_LEDS, LED_STRIP_PIN));
}

int main(void) {
  NRF_LOG_INIT(NULL);
  NRF_LOG_DEFAULT_BACKENDS_INIT();

  ws2812b_drive_set_blank(led_array, NUM_LEDS);

  // for (;;) {
  //   for (int i = 0; i < NUM_LEDS; i++) {
  //     led_array[i].red = (led_array[i].red + 2) % 127;
  //     // led_array[i].green += 1;
  //     // led_array[i].blue += 1;
  //   }
  //   APP_ERROR_CHECK(i2s_ws2812b_drive_xfer(led_array, NUM_LEDS, LED_STRIP_PIN));
  //   NRF_LOG_FLUSH();
  //   nrf_delay_ms(10);
  // }


  NRF_LOG_INFO("alive 1");


  APP_ERROR_CHECK(nrf_sdh_enable_request());
  {
    uint32_t ram_start = 0;
    APP_ERROR_CHECK(nrf_sdh_ble_default_cfg_set(BLE_CONN_CFG_TAG_DEFAULT, &ram_start));
    APP_ERROR_CHECK(nrf_sdh_ble_enable(&ram_start));
  }
  NRF_SDH_BLE_OBSERVER(m_ble_observer, 3, ble_evt_handler, NULL);

  APP_ERROR_CHECK(nrf_ble_gatt_init(&m_gatt, gatt_evt_handler));

  {
    ble_db_discovery_init_t db_init;

    memset(&db_init, 0, sizeof(db_init));

    db_init.evt_handler  = db_disc_handler;
    db_init.p_gatt_queue = &m_ble_gatt_queue;

    ret_code_t err_code = ble_db_discovery_init(&db_init);

    APP_ERROR_CHECK(err_code);
  }
  {
    ble_hrs_c_init_t hrs_c_init_obj;

    hrs_c_init_obj.evt_handler   = hrs_c_evt_handler;
    hrs_c_init_obj.error_handler = service_error_handler;
    hrs_c_init_obj.p_gatt_queue  = &m_ble_gatt_queue;

    ret_code_t err_code = ble_hrs_c_init(&m_hrs_c, &hrs_c_init_obj);
    APP_ERROR_CHECK(err_code);
  }

  APP_ERROR_CHECK(nrf_pwr_mgmt_init());
  NRF_LOG_INFO("alive 2");
  scan_init();
  NRF_LOG_INFO("alive 3");
  scan_start();
  NRF_LOG_INFO("alive 4");

  APP_ERROR_CHECK(app_timer_init());
  APP_ERROR_CHECK(app_timer_create(&led_timer, APP_TIMER_MODE_REPEATED, do_leds));
  APP_ERROR_CHECK(app_timer_start(led_timer, APP_TIMER_TICKS(10), NULL));

  for (;;) {
    // APP_ERROR_CHECK(nrf_ble_lesc_request_handler());
#if NRF_MODULE_ENABLED(NRF_LOG)
    NRF_LOG_FLUSH();
#endif
    nrf_pwr_mgmt_run();
  }
}
