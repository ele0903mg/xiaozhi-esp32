#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

#define AUDIO_INPUT_SAMPLE_RATE  16000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

#define AUDIO_I2S_MIC_GPIO_WS   GPIO_NUM_41
#define AUDIO_I2S_MIC_GPIO_SCK  GPIO_NUM_40
#define AUDIO_I2S_MIC_GPIO_DIN  GPIO_NUM_42
#define AUDIO_I2S_SPK_GPIO_DOUT GPIO_NUM_18
#define AUDIO_I2S_SPK_GPIO_BCLK GPIO_NUM_17
#define AUDIO_I2S_SPK_GPIO_LRCK GPIO_NUM_16

/*
IO9: BUTTON2
IO10: BUTTON3 引出：KEY3
IO15: BUTTON1
*/
#define BUILTIN_LED_GPIO        GPIO_NUM_45
#define BOOT_BUTTON_GPIO        GPIO_NUM_10
#define TOUCH_BUTTON_GPIO       GPIO_NUM_NC
#define VOLUME_UP_BUTTON_GPIO   GPIO_NUM_15
#define VOLUME_DOWN_BUTTON_GPIO GPIO_NUM_9
#define RESET_NVS_BUTTON_GPIO     GPIO_NUM_10
#define RESET_FACTORY_BUTTON_GPIO GPIO_NUM_NC

/* ---------------------------------------------------------------- */
/* 8-channel relay configuration (added for voice control)          */
/* 8 relays mapped to GPIO4/5/6/7/8/11/12/13                         */
/* ---------------------------------------------------------------- */
#define RELAY1_GPIO   GPIO_NUM_4
#define RELAY2_GPIO   GPIO_NUM_5
#define RELAY3_GPIO   GPIO_NUM_6
#define RELAY4_GPIO   GPIO_NUM_7
#define RELAY5_GPIO   GPIO_NUM_8
#define RELAY6_GPIO   GPIO_NUM_11
#define RELAY7_GPIO   GPIO_NUM_12
#define RELAY8_GPIO   GPIO_NUM_13

/* Common 5V relay boards are ACTIVE LOW: LOW=on, HIGH=off.
   If your board is high-level-trigger, set this to 0 (false). */
#define RELAY_ACTIVE_LOW   1

#endif // _BOARD_CONFIG_H_
