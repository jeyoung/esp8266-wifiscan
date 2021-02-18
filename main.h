#include "osapi.h"

#ifndef _MAIN_H_
#define _MAIN_H_

enum main_scan_state { MAIN_SCAN_NONE = 0, MAIN_SCAN_INPROGRESS, MAIN_SCAN_FOUND, MAIN_SCAN_DONE };

static os_timer_t os_timer;

static void main_on_timer(void *arg);

#endif
