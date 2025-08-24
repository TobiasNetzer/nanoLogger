/*
 * StatusLeds.c
 *
 * Created: 22.12.2021 11:20:51
 *  Author: Tobias
 */ 

#include "StatusLeds.h"

static struct timer_task TIMER_0_led_task;

static bool led_status_clear = false;
static bool led_mode_clear = false;
static led_task_status_t led_task_status = LED_TASK_HALTED;


void led_set_usb_idle(void) {
	gpio_set_pin_level(USB_STATUS_LED_RED, true);
	gpio_set_pin_level(USB_STATUS_LED_GREEN, false);
	gpio_set_pin_level(USB_STATUS_LED_BLUE, true);
}

void led_set_usb_error(void) {
	gpio_set_pin_level(USB_STATUS_LED_RED, false);
	gpio_set_pin_level(USB_STATUS_LED_GREEN, true);
	gpio_set_pin_level(USB_STATUS_LED_BLUE, true);
}

void led_set_usb_communication_ongoing(void) {
	gpio_set_pin_level(USB_STATUS_LED_RED, true);
	gpio_set_pin_level(USB_STATUS_LED_GREEN, true);
	gpio_set_pin_level(USB_STATUS_LED_BLUE, false);
}

void led_set_mode_idle(void) {
	gpio_set_pin_level(MODE_LED_RED, true);
	gpio_set_pin_level(MODE_LED_GREEN, false);
	gpio_set_pin_level(MODE_LED_BLUE, true);
}

void led_set_mode_single_measurement(void) {
	gpio_set_pin_level(MODE_LED_RED, false);
	gpio_set_pin_level(MODE_LED_GREEN, false);
	gpio_set_pin_level(MODE_LED_BLUE, true);
}

void led_set_mode_logging(void) {
	gpio_set_pin_level(MODE_LED_RED, true);
	gpio_set_pin_level(MODE_LED_GREEN, true);
	gpio_set_pin_level(MODE_LED_BLUE, false);
}

void led_set_mode_calibration(void) {
	gpio_set_pin_level(MODE_LED_RED, false);
	gpio_set_pin_level(MODE_LED_GREEN, true);
	gpio_set_pin_level(MODE_LED_BLUE, false);
}

void led_set_mode_error(void) {
	gpio_set_pin_level(MODE_LED_RED, false);
	gpio_set_pin_level(MODE_LED_GREEN, true);
	gpio_set_pin_level(MODE_LED_BLUE, true);
}

void led_set_status_measurement_ongoing(void) {
	gpio_set_pin_level(STATUS_LED_RED, true);
	gpio_set_pin_level(STATUS_LED_GREEN, false);
	gpio_set_pin_level(STATUS_LED_BLUE, true);
}

void led_set_status_wait_for_trigger(void) {
	gpio_set_pin_level(STATUS_LED_RED, false);
	gpio_set_pin_level(STATUS_LED_GREEN, true);
	gpio_set_pin_level(STATUS_LED_BLUE, true);
}

void led_set_status_booting(void) {
	gpio_set_pin_level(STATUS_LED_RED, true);
	gpio_set_pin_level(STATUS_LED_GREEN, true);
	gpio_set_pin_level(STATUS_LED_BLUE, false);
}

void led_set_status_off(void) {
	gpio_set_pin_level(STATUS_LED_RED, true);
	gpio_set_pin_level(STATUS_LED_GREEN, true);
	gpio_set_pin_level(STATUS_LED_BLUE, true);
}

void led_mode_task_done(void) {
	led_mode_clear = true;
}

void led_status_task_done(void) {
	led_status_clear = true;
}

led_task_status_t get_led_task_status(void) {
	return led_task_status;
}

void TIMER_0_led_task_cb(const struct timer_task *const timer_task) {
	timer_stop(&TIMER_TC);
	led_task_status = LED_TASK_HALTED;
	
	if(led_status_clear == true) {
		led_set_status_off();
		led_status_clear = false;
	}
	
	if(led_mode_clear == true) {
		led_set_mode_idle();
		led_mode_clear = false;
	}
	
	gpio_set_pin_level(USB_STATUS_LED_BLUE, true);
	if(usbdc_get_state() == 4) {
		led_set_usb_idle();
		} else {
		led_set_usb_error();
	}
}

void timer0_led_task_init(void) {
	TIMER_0_led_task.interval = 100;
	TIMER_0_led_task.cb       = TIMER_0_led_task_cb;
	TIMER_0_led_task.mode     = TIMER_TASK_REPEAT;
	
	timer_add_task(&TIMER_TC, &TIMER_0_led_task);
}

void timer0_led_task_start(void) {
	timer_start(&TIMER_TC);
	led_status_clear = false;
	led_mode_clear = false;
	led_task_status = LED_TASK_RUNNING;
}

void timer0_led_usb_task_start(void) {
	timer_start(&TIMER_TC);
}

void leds_load_calibration(void) {
	led_set_mode_calibration();
	
	for(uint8_t i = 0; i < 10; i++) {
		led_set_status_booting();
		delay_ms(100);
		led_set_status_off();
		delay_ms(100);
	}
	
	if(usbdc_get_state() == 4) {
		led_set_usb_idle();
		} else {
		led_set_usb_error();
	}
	
	led_set_mode_idle();

}

void leds_restore_calibration(void) {
	led_set_mode_error();
	
	for(uint8_t i = 0; i < 20; i++) {
		led_set_status_booting();
		delay_ms(100);
		led_set_status_off();
		delay_ms(100);
	}
	
	if(usbdc_get_state() == 4) {
		led_set_usb_idle();
		} else {
		led_set_usb_error();
	}
	
	led_set_mode_idle();
	
}