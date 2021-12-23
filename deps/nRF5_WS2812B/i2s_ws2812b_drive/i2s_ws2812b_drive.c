/* Copyright (c) 2016 Takafumi Naka. All Rights Reserved.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#include "i2s_ws2812b_drive.h"
#include "nrf_delay.h"
#include "SEGGER_RTT.h"

uint32_t i2s_ws2812b_drive_tx_buf[WS2812B_N_LEDS * 3 + 6];
static volatile int nled = 1;
nrfx_i2s_buffers_t next_bufs = {.p_tx_buffer = i2s_ws2812b_drive_tx_buf, .p_rx_buffer = NULL};

uint32_t calcChannelValue(uint8_t level)
{
    uint32_t val = 0;

    // 0 
    if(level == 0) {
        val = 0x88888888;
    }
    // 255
    else if (level == 255) {
        val = 0xeeeeeeee;
    }
    else {
        // apply 4-bit 0xe HIGH pattern wherever level bits are 1.
        val = 0x88888888;
        for (uint8_t i = 0; i < 8; i++) {
            if((1 << i) & level) {
                uint32_t mask = ~(0x0f << 4*i);
                uint32_t patt = (0x0e << 4*i);
                val = (val & mask) | patt;
            }
        }

        // swap 16 bits
        val = (val >> 16) | (val << 16);
    }

    return val;
}

void i2s_ws2812b_drive_set_buff(rgb_led_t * rgb_led) {
    for(int i = 0; i < WS2812B_N_LEDS; i++) {
      i2s_ws2812b_drive_tx_buf[i*3+1] = calcChannelValue(rgb_led[i].red);
      i2s_ws2812b_drive_tx_buf[i*3+0] = calcChannelValue(rgb_led[i].green);
      i2s_ws2812b_drive_tx_buf[i*3+2] = calcChannelValue(rgb_led[i].blue);
    }

    // reset 
    for(int i = WS2812B_N_LEDS*3; i < WS2812B_N_LEDS*3+6; i++) {
        i2s_ws2812b_drive_tx_buf[i] = 0;
    }
  // uint8_t *p_xfer = (uint8_t*)i2s_ws2812b_drive_tx_buf;
	// rgb_led_t* p_led = rgb_led; 
	// int8_t offset = 0;
	// 
	// for(uint16_t i_led=0;i_led<(WS2812B_N_LEDS);i_led++)
	// {
	// 	uint32_t rgb_data = (p_led->green << 16) | (p_led->red << 8 ) | p_led->blue;
	// 	for(uint8_t i_rgb=0;i_rgb<I2S_WS2812B_DRIVE_BUF_SIZE_PER_LED;i_rgb++)
	// 	{
	// 		switch(rgb_data & 0x00c00000 )
	// 		{
	// 			case ( 0x00400000 ):
	// 				*(p_xfer + offset)  = (uint8_t)(( I2S_WS2812B_DRIVE_PATTERN_0 << 4 ) | I2S_WS2812B_DRIVE_PATTERN_1);
	// 				break;
	// 			case ( 0x00800000 ):
	// 				*(p_xfer + offset)  = (uint8_t)(( I2S_WS2812B_DRIVE_PATTERN_1 << 4 ) | I2S_WS2812B_DRIVE_PATTERN_0);
	// 				break;
	// 			case ( 0x00c00000 ):
	// 				*(p_xfer + offset)  = (uint8_t)(( I2S_WS2812B_DRIVE_PATTERN_1 << 4 ) | I2S_WS2812B_DRIVE_PATTERN_1);
	// 				break;
	// 			default:
	// 				*(p_xfer + offset)  = (uint8_t)(( I2S_WS2812B_DRIVE_PATTERN_0 << 4 ) | I2S_WS2812B_DRIVE_PATTERN_0);
	// 		}
	// 		p_xfer++;
	// 		offset = -offset;
	// 		rgb_data <<= (24 / I2S_WS2812B_DRIVE_BUF_SIZE_PER_LED);
	// 	}
	// 	p_led++;
	// }
	// for (uint8_t i = 0; i < 6; i++) {
  //   *p_xfer++ = 0;
  // }
}

extern void i2s_ws2812b_drive_handler(const nrfx_i2s_buffers_t *bufs, uint32_t        status)
{
    // nrfx_i2s_next_buffers_set(&next_bufs);
}

bool g_did_init = false;

ret_code_t i2s_ws2812b_drive_xfer(rgb_led_t *led_array, uint16_t num_leds, uint8_t drive_pin)
{
	ret_code_t err_code;

  i2s_ws2812b_drive_set_buff(led_array);

  if (!g_did_init) {
    // define configs
    nrf_drv_i2s_config_t config = NRF_DRV_I2S_DEFAULT_CONFIG;
    config.sck_pin      = 22; // Don't set NRF_DRV_I2S_PIN_NOT_USED for I2S_CONFIG_SCK_PIN. (The program will stack.) 
    config.lrck_pin     = NRF_DRV_I2S_PIN_NOT_USED;
    config.mck_pin      = NRF_DRV_I2S_PIN_NOT_USED;
    config.sdout_pin    = drive_pin;
    config.sdin_pin     = NRF_DRV_I2S_PIN_NOT_USED;

    config.irq_priority = APP_IRQ_PRIORITY_HIGH;
    config.mode         = NRF_I2S_MODE_MASTER;
    config.format       = NRF_I2S_FORMAT_I2S;
    config.alignment    = NRF_I2S_ALIGN_LEFT;
    config.sample_width = NRF_I2S_SWIDTH_16BIT;
    config.channels     = NRF_I2S_CHANNELS_STEREO;
    config.mck_setup 		= NRF_I2S_MCK_32MDIV10;
    config.ratio     		= NRF_I2S_RATIO_32X;
    
    // initialize i2s
    err_code = nrf_drv_i2s_init(&config, i2s_ws2812b_drive_handler);
    APP_ERROR_CHECK(err_code);
    if ( err_code != NRF_SUCCESS ) 
    {
      return err_code;
    }
    g_did_init = true;

    err_code = nrfx_i2s_start(&next_bufs, sizeof(i2s_ws2812b_drive_tx_buf) / 4, 0);
    APP_ERROR_CHECK(err_code);
    if ( err_code != NRF_SUCCESS ) 
    {
      return err_code;
    }
  }

	// start transfer

	// nrf_delay_us((num_leds+20) * (24*5/4));

	// stop transfer
	// nrf_drv_i2s_stop();

	// un-initialize i2s
	// nrf_drv_i2s_uninit();
	
	return NRF_SUCCESS;
}

