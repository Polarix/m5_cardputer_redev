#ifndef _INCLUDE_WIFI_LIST_SCREEN_H_
#define _INCLUDE_WIFI_LIST_SCREEN_H_

#include <stdint.h>
#include <stddef.h>

void wifi_list_screen_create(void);

void wifi_list_screen_load(void);

void wifi_list_screen_destroy(void);

void wifi_list_screen_clear(void);

void wifi_list_screen_set_item_count(uint16_t item_cnt);

void wifi_list_screen_set_item(uint16_t item_idx, const char* ssid, int rssi, int channel);

void wifi_list_screen_add_item(const char* ssid, int rssi);

void wifi_list_screen_show(void);

#endif // _INCLUDE_STARTUP_SCREEN_H_