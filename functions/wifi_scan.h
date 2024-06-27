#ifndef _INCLUDE_WIFI_SCAN_H_
#define _INCLUDE_WIFI_SCAN_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void wifi_scan_init(void);

void wifi_func_deinit(void);

void wifi_scan_task_exec(void);

void wifi_scan_show_info(uint16_t ap_idx);

#ifdef __cplusplus
}
#endif

#endif // _INCLUDE_WIFI_SCAN_H_