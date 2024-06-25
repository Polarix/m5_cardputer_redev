#ifndef _INCLUDE_ESP_KEYPAD_H_
#define _INCLUDE_ESP_KEYPAD_H_

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void esp_keypad_init(void);
void esp_keypad_scan(lv_indev_drv_t* indev_driver, lv_indev_data_t* data);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_ESP_KEYPAD_H_ */