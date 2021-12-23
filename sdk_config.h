#include "nrfx_config.h"

#define APP_SCHEDULER_ENABLED 0

#define APP_TIMER_ENABLED 1
#define APP_TIMER_V2 1
#define APP_TIMER_V2_RTC1_ENABLED 1
#define APP_TIMER_CONFIG_OP_QUEUE_SIZE 10
#define APP_TIMER_SAFE_WINDOW_MS 300000
#define APP_TIMER_CONFIG_RTC_FREQUENCY 1
#define APP_TIMER_KEEPS_RTC_ACTIVE 0
#define APP_TIMER_CONFIG_IRQ_PRIORITY 6

#define APP_FIFO_ENABLED 1

#define BUTTON_ENABLED 1

#define CLOCK_ENABLED 1
#define NRF_CLOCK_ENABLED 1
#define CLOCK_CONFIG_LF_SRC 0
#define NRFX_CLOCK_CONFIG_IRQ_PRIORITY 6
#define CLOCK_CONFIG_SOC_OBSERVER_PRIO 0
#define NRF_SDH_SOC_OBSERVER_PRIO_LEVELS 2
#define CLOCK_CONFIG_STATE_OBSERVER_PRIO 0

#define GPIOTE_ENABLED 1
#define GPIOTE_CONFIG_NUM_OF_LOW_POWER_EVENTS 4
#define NRFX_GPIOTE_CONFIG_IRQ_PRIORITY 6

#define BLE_ADVERTISING_ENABLED 1
#define BSP_BTN_BLE_ENABLED 0
#define BSP_BTN_BLE_OBSERVER_PRIO 1
#define NRF_SDH_BLE_OBSERVER_PRIO_LEVELS 4
#define BLE_CONN_PARAMS_BLE_OBSERVER_PRIO 1
#define BLE_CONN_STATE_BLE_OBSERVER_PRIO 0
#define NRF_SDH_BLE_CENTRAL_LINK_COUNT 1
#define NRF_SDH_BLE_GATTS_ATTR_TAB_SIZE 1408
#define NRF_BLE_GATT_BLE_OBSERVER_PRIO 1
#define NRF_BLE_SCAN_SCAN_WINDOW 80
#define NRF_SDH_BLE_SERVICE_CHANGED 0
#define NRF_SDH_BLE_STACK_OBSERVER_PRIO 0
#define NRF_SDH_SOC_STACK_OBSERVER_PRIO 0

#define NRF_SORTLIST_ENABLED 1
#define NRF_SORTLIST_CONFIG_LOG_ENABLED 0

#define NRF_BLE_CONN_PARAMS_ENABLED 1
#define NRF_BLE_CONN_PARAMS_MAX_SLAVE_LATENCY_DEVIATION 499
#define NRF_BLE_CONN_PARAMS_MAX_SUPERVISION_TIMEOUT_DEVIATION 65535

#define NRF_BLE_SCAN_ENABLED 1
#define NRF_BLE_SCAN_SCAN_DURATION 0
#define NRF_BLE_SCAN_SUPERVISION_TIMEOUT 4000
#define NRF_BLE_SCAN_MIN_CONNECTION_INTERVAL 7.5
#define NRF_BLE_SCAN_MAX_CONNECTION_INTERVAL 30
#define NRF_BLE_SCAN_SLAVE_LATENCY 0

#define NRF_QUEUE_ENABLED 1

#define BLE_DB_DISCOVERY_ENABLED 1

#define NRF_BLE_SCAN_BUFFER 31
#define NRF_BLE_SCAN_SCAN_INTERVAL 160
#define NRF_BLE_SCAN_OBSERVER_PRIO 1
#define NRF_BLE_GQ_ENABLED 1
#define NRF_BLE_GQ_GATTC_WRITE_MAX_DATA_LEN 16
#define NRF_BLE_GQ_GATTS_HVX_MAX_DATA_LEN 16
#define NRF_BLE_GQ_DATAPOOL_ELEMENT_COUNT 8
#define NRF_BLE_GQ_QUEUE_SIZE 8
#define NRF_BLE_GQ_DATAPOOL_ELEMENT_SIZE 20
#define NRF_BLE_GQ_BLE_OBSERVER_PRIO 1

#define BLE_DB_DISC_BLE_OBSERVER_PRIO 1

#define BLE_HRS_C_ENABLED 1
#define BLE_HRS_C_BLE_OBSERVER_PRIO 2

#define NRFX_I2S_ENABLED 1
#define NRFX_I2S_CONFIG_SCK_PIN 31
#define NRFX_I2S_CONFIG_LRCK_PIN 30
#define NRFX_I2S_CONFIG_MCK_PIN 255
#define NRFX_I2S_CONFIG_SDOUT_PIN 29
#define NRFX_I2S_CONFIG_SDIN_PIN 28
#define NRFX_I2S_CONFIG_MASTER 0
#define NRFX_I2S_CONFIG_FORMAT 0
#define NRFX_I2S_CONFIG_ALIGN 0
#define NRFX_I2S_CONFIG_SWIDTH 1
#define NRFX_I2S_CONFIG_MCK_SETUP 402653184
#define NRFX_I2S_CONFIG_CHANNELS 0
#define NRFX_I2S_CONFIG_RATIO 2000
#define NRFX_I2S_CONFIG_IRQ_PRIORITY 7

#ifdef DEBUG
#define NRF_LOG_ENABLED 1
#define NRF_LOG_DEFAULT_LEVEL 5

#define NRF_LOG_BUFSIZE 1024
#define NRF_LOG_MSGPOOL_ELEMENT_SIZE 20
#define NRF_LOG_MSGPOOL_ELEMENT_COUNT 8
#define NRF_LOG_STR_PUSH_BUFFER_SIZE 128
#define NRF_LOG_DEFERRED 1
#define NRF_LOG_ALLOW_OVERFLOW 1
#define NRF_LOG_STR_FORMATTER_TIMESTAMP_FORMAT_ENABLED 1

#define NRF_LOG_BACKEND_UART_ENABLED 0
#define NRF_LOG_TIMESTAMP_DEFAULT_FREQUENCY 0

#define NRF_LOG_BACKEND_RTT_ENABLED 1

#else

#define NRF_LOG_ENABLED 0
#define NRF_LOG_BACKEND_RTT_ENABLED 0

#endif

#define NRF_BLE_SCAN_FILTER_ENABLE 1
#define NRF_BLE_SCAN_UUID_CNT 1

#define SEGGER_RTT_CONFIG_MAX_NUM_UP_BUFFERS 2
#define SEGGER_RTT_CONFIG_MAX_NUM_DOWN_BUFFERS 2
#define SEGGER_RTT_CONFIG_BUFFER_SIZE_UP 4096
#define SEGGER_RTT_CONFIG_BUFFER_SIZE_DOWN 16
#define SEGGER_RTT_CONFIG_DEFAULT_MODE 0
#define NRF_LOG_BACKEND_RTT_TEMP_BUFFER_SIZE 64
#define NRF_LOG_BACKEND_RTT_TX_RETRY_CNT 3
#define NRF_LOG_BACKEND_RTT_TX_RETRY_DELAY_MS 1

#define NRF_PWR_MGMT_ENABLED 1
#define NRF_PWR_MGMT_CONFIG_FPU_SUPPORT_ENABLED 1

#define NRF_FSTORAGE_ENABLED 1

#define NRF_SDH_ENABLED 1
#define NRF_SDH_BLE_ENABLED 1
#define NRF_SDH_BLE_GAP_DATA_LENGTH 27
#define NRF_SDH_BLE_GAP_EVENT_LENGTH 6
#define NRF_SDH_BLE_GATT_MAX_MTU_SIZE 247
#define NRF_SDH_BLE_PERIPHERAL_LINK_COUNT 5
#define NRF_SDH_BLE_VS_UUID_COUNT 1
#define NRF_SDH_SOC_ENABLED 1
#define NRF_SDH_STATE_OBSERVER_PRIO_LEVELS 2
#define NRF_SDH_STACK_OBSERVER_PRIO_LEVELS 2

#define APP_TIMER_PRESCALER             0                                           
#define APP_TIMER_MAX_TIMERS           36
#define APP_TIMER_OP_QUEUE_SIZE        36


#define NRF_SDH_CLOCK_LF_SRC 0
#define NRF_SDH_CLOCK_LF_RC_CTIV 32
#define NRF_SDH_CLOCK_LF_RC_TEMP_CTIV 2
#define NRF_SDH_CLOCK_LF_ACCURACY 1

#define FDS_ENABLED 1

#define PEER_MANAGER_ENABLED 1
#define PM_MAX_REGISTRANTS 6

#define BLE_DFU_ENABLED 1
#define NRF_DFU_BLE_BUTTONLESS_SUPPORTS_BONDS 1
#define NRF_DFU_BLE_REQUIRES_BONDS 1
#define NRF_DFU_TRANSPORT_BLE 1

#define NRF_BLE_GATT_ENABLED 1

#define NRF_FPRINTF_ENABLED 1
#define NRF_FPRINTF_FLAG_AUTOMATIC_CR_ON_LF_ENABLED 1
#define NRF_FPRINTF_DOUBLE_ENABLED 0

#define NRF_STRERROR_ENABLED 1

#define NRF_BALLOC_ENABLED 1
#define NRF_BALLOC_CONFIG_DEBUG_ENABLED 0
#define NRF_BALLOC_CONFIG_HEAD_GUARD_WORDS 1
#define NRF_BALLOC_CONFIG_TAIL_GUARD_WORDS 1
#define NRF_BALLOC_CONFIG_BASIC_CHECKS_ENABLED 0
#define NRF_BALLOC_CONFIG_DOUBLE_FREE_CHECK_ENABLED 0
#define NRF_BALLOC_CONFIG_DATA_TRASHING_CHECK_ENABLED 0

#define NRF_SECTION_ITER_ENABLED 1