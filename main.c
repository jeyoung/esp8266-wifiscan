#include <stdio.h>

#include "mem.h"
#include "osapi.h"
#include "uart.h"
#include "user_interface.h"

#include "main.h"

#define SSID_LIMIT 128

/*
 * This program scans for available WiFi access points.
 */

static enum main_scan_state state = MAIN_SCAN_NONE;

static struct scan_config config = {0};

/* Supports SSID_LIMIT SSIDs of max length 32 */
static char ssids[SSID_LIMIT][32]= {};

/* Used to count the SSIDs as it iterates over them */
static size_t ssid_counter;

static void main_on_scan_done(void *arg, STATUS status)
{
    struct bss_info *found = (struct bss_info *)arg;

    if (!found) {
	ssid_counter = 0;
	state = MAIN_SCAN_DONE;
	return;
    }

    struct bss_info *p;
    for (ssid_counter = 0, p = found; ssid_counter < SSID_LIMIT && p; ++ssid_counter, p = STAILQ_NEXT(p, next)) {
	uint8_t *c = p->ssid;
	char *ssid = ssids[ssid_counter];
	while (*c) {
	    *ssid++ = *c++;
	}
	*ssid = '\0';
    }

    state = MAIN_SCAN_FOUND;
}

static void main_on_starting()
{
    /* Set in station mode */
    wifi_set_opmode(0x01);

    /* Include hidden SSIDs in scan */
    config.show_hidden = 1;

    state = MAIN_SCAN_INPROGRESS;
}

static void main_on_scanning()
{
    wifi_station_scan (&config, &main_on_scan_done);
}

static void main_on_found()
{
    uint8_t i;
    for (i = 0; i < ssid_counter; ++i) {
	os_printf("%s\n", ssids[i]);
    }
    state = MAIN_SCAN_DONE;
}

static void main_on_done()
{
    os_printf("Done!\n");
    state = MAIN_SCAN_NONE;
}

static void main_on_timer(void *arg)
{
    switch (state) {
	case MAIN_SCAN_NONE:
	    main_on_starting();
	    break;

	case MAIN_SCAN_INPROGRESS:
	    main_on_scanning();
	    break;

	case MAIN_SCAN_FOUND:
	    main_on_found();
	    break;

	case MAIN_SCAN_DONE:
	    main_on_done();
	    break;
    }
}

void ICACHE_FLASH_ATTR user_init(void)
{
    /*
     * UART0 is the default debugging interface, so we must initialise UART
     * with the desired baud rate
     */
    uart_init(BIT_RATE_9600, BIT_RATE_9600);
    
    os_timer_disarm(&os_timer);
    os_timer_setfn(&os_timer, &main_on_timer, (void *)NULL);
    os_timer_arm(&os_timer, 1000, 1);
}
