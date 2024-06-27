#ifndef _INCLUDE_RECARDPUTER_WIFI_SCAN_SCREEN_H_
#define _INCLUDE_RECARDPUTER_WIFI_SCAN_SCREEN_H_

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void wifi_scan_screen_create(void);

void wifi_scan_screen_load(void);

void wifi_scan_screen_set_item_count(int count);

void wifi_scan_screen_set_item(int index, const char* ssid, int rssi, int channel);

void wifi_scan_screen_scan_done(void);

#ifdef __cplusplus
}
#endif

#endif // _INCLUDE_RECARDPUTER_WIFI_SCAN_SCREEN_H_
