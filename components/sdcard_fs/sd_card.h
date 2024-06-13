#ifndef _INCLUDE_SD_CARD_H_
#define _INCLUDE_SD_CARD_H_

#include <stdlib.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

esp_err_t sdcard_mount(void);

esp_err_t sdcard_format(void);

void sdcard_show_root(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _INCLUDE_SD_CARD_H_
