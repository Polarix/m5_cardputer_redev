#ifndef _INCLUDE_BAT_VOLT_H_
#define _INCLUDE_BAT_VOLT_H_

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void bat_volt_monitor_init(void);
void bat_volt_monitor_deinit(void);
int bat_volt_read_mv(void);

#ifdef __cplusplus
}
#endif

#endif // _INCLUDE_BAT_VOLT_H_