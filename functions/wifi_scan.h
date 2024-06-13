#ifndef _INCLUDE_WIFI_SCAN_H_
#define _INCLUDE_WIFI_SCAN_H_

#include <stdint.h>
#include <stddef.h>

void wifi_scan_init(void);

void wifi_scan_task_exec(void);

void wifi_scan_show_info(uint16_t ap_idx);

#endif // _INCLUDE_WIFI_SCAN_H_