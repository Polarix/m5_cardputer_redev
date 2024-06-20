#ifndef _INCLUDE_ESP_LVGL_H_
#define _INCLUDE_ESP_LVGL_H_

#include "hal/spi_ll.h"
#include "esp_lcd_panel_vendor.h"

#ifdef __cplusplus
extern "C" {
#endif

void esp_lvgl_adapter_init(void);
bool esp_lvgl_lock(int timeout_ms);
void esp_lvgl_unlock(void);

#ifdef __cplusplus
}
#endif

#endif // _INCLUDE_ESP_LVGL_H_