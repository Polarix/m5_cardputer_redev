#ifndef _INCLUDE_ESP_KEYPAD_H_
#define _INCLUDE_ESP_KEYPAD_H_

#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _st_lv_key_evt_
{
    uint8_t             code;
    lv_indev_state_t    state;
}esp_key_evt_t;

void esp_keypad_init(void);
void esp_keypad_scan(lv_indev_drv_t* indev_driver, lv_indev_data_t* data);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_ESP_KEYPAD_H_ */