#ifndef _INCLUDE_LOADING_SCREEN_H_
#define _INCLUDE_LOADING_SCREEN_H_

#include <stdint.h>
#include <stddef.h>

void wifi_info_screen_create(void);

void wifi_info_screen_load(uint16_t ap_idx);

void wifi_info_screen_destroy(void);

void wifi_info_screen_fill(const char* ssid, int8_t rssi, uint8_t channel);

#endif // _INCLUDE_LOADING_SCREEN_H_