/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_examples.h"
#include "driver_init.h"
#include "utils.h"

/**
 * Example of using ADC_0 to generate waveform.
 */
void ADC_0_example(void)
{
	uint8_t buffer[2];

	adc_sync_enable_channel(&ADC_0, 0);

	while (1) {
		adc_sync_read_channel(&ADC_0, 0, buffer, 2);
	}
}

static void button_on_PB03_pressed(void)
{
}

/**
 * Example of using EXTERNAL_IRQ_0
 */
void EXTERNAL_IRQ_0_example(void)
{

	ext_irq_register(PIN_PB03, button_on_PB03_pressed);
}

static struct timer_task TIMER_RTC_task1, TIMER_RTC_task2;
/**
 * Example of using TIMER_RTC.
 */
static void TIMER_RTC_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_RTC_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_RTC_example(void)
{
	TIMER_RTC_task1.interval = 100;
	TIMER_RTC_task1.cb       = TIMER_RTC_task1_cb;
	TIMER_RTC_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_RTC_task2.interval = 200;
	TIMER_RTC_task2.cb       = TIMER_RTC_task2_cb;
	TIMER_RTC_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_RTC, &TIMER_RTC_task1);
	timer_add_task(&TIMER_RTC, &TIMER_RTC_task2);
	timer_start(&TIMER_RTC);
}

void delay_example(void)
{
	delay_ms(5000);
}

static struct timer_task TIMER_TC_task1, TIMER_TC_task2;

/**
 * Example of using TIMER_TC.
 */
static void TIMER_TC_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_TC_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_TC_example(void)
{
	TIMER_TC_task1.interval = 100;
	TIMER_TC_task1.cb       = TIMER_TC_task1_cb;
	TIMER_TC_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_TC_task2.interval = 200;
	TIMER_TC_task2.cb       = TIMER_TC_task2_cb;
	TIMER_TC_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_TC, &TIMER_TC_task1);
	timer_add_task(&TIMER_TC, &TIMER_TC_task2);
	timer_start(&TIMER_TC);
}

static struct timer_task TIMER_LOGGING_task1, TIMER_LOGGING_task2;

/**
 * Example of using TIMER_LOGGING.
 */
static void TIMER_LOGGING_task1_cb(const struct timer_task *const timer_task)
{
}

static void TIMER_LOGGING_task2_cb(const struct timer_task *const timer_task)
{
}

void TIMER_LOGGING_example(void)
{
	TIMER_LOGGING_task1.interval = 100;
	TIMER_LOGGING_task1.cb       = TIMER_LOGGING_task1_cb;
	TIMER_LOGGING_task1.mode     = TIMER_TASK_REPEAT;
	TIMER_LOGGING_task2.interval = 200;
	TIMER_LOGGING_task2.cb       = TIMER_LOGGING_task2_cb;
	TIMER_LOGGING_task2.mode     = TIMER_TASK_REPEAT;

	timer_add_task(&TIMER_LOGGING, &TIMER_LOGGING_task1);
	timer_add_task(&TIMER_LOGGING, &TIMER_LOGGING_task2);
	timer_start(&TIMER_LOGGING);
}
