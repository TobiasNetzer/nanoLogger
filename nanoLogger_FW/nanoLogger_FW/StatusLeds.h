/*
 * StatusLeds.h
 *
 * Created: 22.12.2021 11:21:06
 *  Author: Tobias
 */ 


#ifndef STATUSLEDS_H_
#define STATUSLEDS_H_

#include "driver_init.h"
#include "utils.h"
#include "usbdc.h"

typedef enum led_task_status_e {
	LED_TASK_HALTED,
	LED_TASK_RUNNING
} led_task_status_t;

void led_set_usb_idle(void);

void led_set_usb_error(void);

void led_set_usb_communication_ongoing(void);

void led_set_mode_idle(void);

void led_set_mode_single_measurement(void);

void led_set_mode_logging(void);

void led_set_mode_calibration(void);

void led_set_mode_error(void);

void led_set_status_measurement_ongoing(void);

void led_set_status_wait_for_trigger(void);

void led_set_status_booting(void);

void led_set_status_off(void);

void led_mode_task_done(void);

void led_status_task_done(void);

led_task_status_t get_led_task_status(void);

void TIMER_0_led_task_cb(const struct timer_task *const timer_task);

void timer0_led_task_init(void);

void timer0_led_task_start(void);

void timer0_led_usb_task_start(void);

void leds_load_calibration(void);

void leds_restore_calibration(void);

#endif /* STATUSLEDS_H_ */