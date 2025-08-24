/*
 * user_cli.h
 *
 * Created: 18.12.2021 14:10:21
 *  Author: Tobias
 */ 


#ifndef USER_CLI_H_
#define USER_CLI_H_

#include <atmel_start.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "external_trigger.h"
#include "current_measurement.h"
#include "userTypedefs.h"
#include "StatusLeds.h"
#include "current_logging.h"
#include "smarteeprom_user_parameters.h"

typedef enum usb_data_received_status_e {
	USB_NO_DATA_RECEIVED,
	USB_DATA_RECEIVED
} usb_data_received_status_t;

typedef enum usb_task_status_e {
	USB_TASK_NOT_FINISHED,
	USB_TASK_FINISHED
} usb_task_status_t;

void usb_cli_init(void);

void usb_cli_task_set_finished(void);

usb_task_status_t get_usb_cli_task_status(void);

void usb_cli_get_help(void);

void usb_cli_error(nanoLogger_t *nLogger);

void usb_cli_cancel_operation(nanoLogger_t *nLogger);

void usb_cli_device_calibration(nanoLogger_t *nLogger);

void usb_cli_get_adc_raw_value(currentMeasurement_t *measurement_s);

void usb_cli_get_adc_voltage(currentMeasurement_t *measurement_s);

void usb_cli_get_adc_raw_average(currentMeasurement_t *measurement_s, uint32_t samples);

void usb_cli_get_adc_voltage_average(currentMeasurement_t *measurement_s, uint32_t samples);

void usb_cli_get_current_range(currentMeasurement_t *measurement_s);

void usb_cli_get_current(currentMeasurement_t *measurement_s, uint32_t samples);

void usb_cli_get_current_converted(currentMeasurement_t *measurement_s, uint32_t samples);

void usb_cli_get_adc_resolution(currentMeasurement_t *measurement_s);

void usb_cli_set_external_trigger(nanoLogger_t *nLogger);

void usb_cli_logging(nanoLogger_t *nLogger);

void usb_cli_handle_response(void);

void usb_cli_process_received_data(nanoLogger_t *nLogger);

void usb_cli_execute_operation(nanoLogger_t *nLogger);

#endif /* USER_CLI_H_ */