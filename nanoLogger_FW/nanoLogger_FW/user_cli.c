/*
 * user_cli.c
 *
 * Created: 18.12.2021 14:10:36
 *  Author: Tobias
 */ 

#include "user_cli.h"

static uint8_t usb_buffer[256];
static uint8_t usb_user_buffer[256];
static usb_data_received_status_t usb_data_received_status = USB_NO_DATA_RECEIVED;
static usb_task_status_t usb_task_status = USB_TASK_NOT_FINISHED;

static bool usb_cli_cb_bulk_out(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	usb_cli_handle_response();

	/* No error. */
	return false;
}

static bool usb_cli_cb_bulk_in(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	
	cdcdf_acm_read((uint8_t *)usb_buffer, sizeof(usb_buffer));

	/* No error. */
	return false;
}

static bool usb_device_cb_state_change(usb_cdc_control_signal_t state)
{
	//if (state.rs232.DTR) {
	/* Callbacks must be registered after endpoint allocation */
	cdcdf_acm_register_callback(CDCDF_ACM_CB_READ, (FUNC_PTR)usb_cli_cb_bulk_out);
	cdcdf_acm_register_callback(CDCDF_ACM_CB_WRITE, (FUNC_PTR)usb_cli_cb_bulk_in);
	/* Start Rx */
	cdcdf_acm_read((uint8_t *)usb_buffer, sizeof(usb_buffer));
	//}

	/* No error. */
	return false;
}

void usb_cli_init(void) {
	while (!cdcdf_acm_is_enabled()) {
		// wait cdc acm to be installed
	};

	cdcdf_acm_register_callback(CDCDF_ACM_CB_STATE_C, (FUNC_PTR)usb_device_cb_state_change);
}

void usb_cli_task_set_finished(void) {
	
	usb_task_status = USB_TASK_FINISHED;
	
}

usb_task_status_t get_usb_cli_task_status(void) {
	
	return usb_task_status;
	
}

void usb_cli_get_help(void) {
	char help_buffer[] =    "----------------------------------------\n\r"
							"----------COMMANDS:----------\n\r"
							"getAdcRaw\n\r"
							"getAdcVoltage\n\r"
							"getAdcResolution\n\r"
							"getCurrent\n\r"
							"getCurrentConverted\n\r"
							"getCurrentRange\n\r"
							"startLogging\n\r"
							"----------PARAMETERS:----------\n\r"
							"Trigger:[neg/pos]\n\r"
							"SampleRate:[uint32](units in samples per second)\n\r"
							"RecordLength:[uint32](units in ms)\n\r"
							"Samples:[uint32]\n\r"
							"Resolution:[10/12/16Bit]\n\r"
							"----------CALIBRATION:----------\n\r"
							"deviceCalibration [+ Command]\n\r"
							"ShowCalibration\n\r"
							"SetAdcRef:[float]\n\r"
							"SetAdcGainCorrection:[float]\n\r"
							"SetRangeOffset1..5:[float]\n\r"
							"----------------------------------------\n\r"
							"nanoLogger>";
	
	cdcdf_acm_write((uint8_t*)help_buffer, strlen(help_buffer));
	delay_ms(10);
}

void usb_cli_error(nanoLogger_t *nLogger) {
	uint8_t output_buffer[] = "\nError Unknown Command!\n\r";
	led_set_mode_error();
	nLogger->deviceConfig.deviceMode = DEVICE_ERROR;
	cdcdf_acm_write((uint8_t*)output_buffer, strlen((char*)output_buffer));
	delay_ms(10);
}

void usb_cli_cancel_operation(nanoLogger_t *nLogger) {
	uint8_t output_buffer[] = "\nCanceled\n\r";
	
	if(get_TIMER_LOGGING_status() == TIMER_LOGGING_RUNNING) {
		TIMER_LOGGING_end_task();
	}
	
	nLogger->deviceConfig.deviceMode = DEVICE_IDLE;
	nLogger->deviceConfig.samples = 1;
	nLogger->deviceConfig.triggerConfig = NO_EXT_TRIGGER;
	nLogger->deviceConfig.recordLength = 0;
	nLogger->deviceConfig.sampleRate = 0;
	external_trigger_state_set_triggered();
	led_set_status_off();
	led_set_mode_idle();
	cdcdf_acm_write((uint8_t*)output_buffer, strlen((char*)output_buffer));
	delay_ms(10);
	usb_cli_task_set_finished();
}

void usb_cli_device_calibration(nanoLogger_t *nLogger) {
	nLogger->deviceConfig.deviceMode = DEVICE_CALIBRATION;
}

void usb_cli_get_adc_raw_value(currentMeasurement_t *measurement_s) {
	uint8_t output_buffer[64];
	led_set_mode_single_measurement();
	read_adc_raw(measurement_s);
	sprintf((char*)output_buffer, "\nADC RAW: %d\n\r", measurement_s->adcRawValue);
	cdcdf_acm_write((uint8_t*)output_buffer, strlen((char*)output_buffer));
	delay_ms(10);
}

void usb_cli_get_adc_voltage(currentMeasurement_t *measurement_s) {
	uint8_t output_buffer[64];
	led_set_mode_single_measurement();
	read_adc_voltage(measurement_s);
	sprintf((char*)output_buffer, "\nVoltage: %.2fmV\n\r", measurement_s->adcResult_MV);
	cdcdf_acm_write((uint8_t*)output_buffer, strlen((char*)output_buffer));
	delay_ms(10);
}

void usb_cli_get_adc_raw_average(currentMeasurement_t *measurement_s, uint32_t samples) {
	uint8_t output_buffer[64];
	led_set_mode_single_measurement();
	read_adc_raw_averages(measurement_s, samples);
	sprintf((char*)output_buffer, "\nADC RAW: %d\n\r", measurement_s->adcRawValue);
	cdcdf_acm_write((uint8_t*)output_buffer, strlen((char*)output_buffer));
	delay_ms(10);
	usb_cli_task_set_finished();
}

void usb_cli_get_adc_voltage_average(currentMeasurement_t *measurement_s, uint32_t samples) {
	uint8_t output_buffer[64];
	led_set_mode_single_measurement();
	read_adc_voltage_averages(measurement_s, samples);
	sprintf((char*)output_buffer, "\nVoltage: %.2fmV\n\r", measurement_s->adcResult_MV);
	cdcdf_acm_write((uint8_t*)output_buffer, strlen((char*)output_buffer));
	delay_ms(10);
	usb_cli_task_set_finished();
}

void usb_cli_get_current_range(currentMeasurement_t *measurement_s) {
	uint8_t output_buffer[64];
	get_current_range(measurement_s);
	sprintf((char*)output_buffer, "\nCurrent Range: %d\n\r", measurement_s->currentRange);
	cdcdf_acm_write((uint8_t*)output_buffer, strlen((char*)output_buffer));
	delay_ms(10);
	usb_cli_task_set_finished();
}

void usb_cli_get_current(currentMeasurement_t *measurement_s, uint32_t samples) {
	uint8_t output_buffer[64];
	led_set_mode_single_measurement();
	//read_adc_voltage_averages(measurement_s, samples);
	//get_current(measurement_s);
	float sum = 0;
	for(uint32_t i = 0; i < samples; i++) {
		read_adc_voltage(measurement_s);
		get_current(measurement_s);
		sum += measurement_s->currentResult;
	}
	measurement_s->currentResult = ( sum / (float) samples);
	sprintf((char*)output_buffer, "\nCurrent: %.6fmA\n\r", measurement_s->currentResult);
	cdcdf_acm_write((uint8_t*)output_buffer, strlen((char*)output_buffer));
	delay_ms(10);
	usb_cli_task_set_finished();
}

void usb_cli_get_current_converted(currentMeasurement_t *measurement_s, uint32_t samples) {
	uint8_t output_buffer[64];
	led_set_mode_single_measurement();
	//read_adc_voltage_averages(measurement_s, samples);
	//get_current(measurement_s);
	float sum = 0;
	for(uint32_t i = 0; i < samples; i++) {
		read_adc_voltage(measurement_s);
		get_current(measurement_s);
		sum += measurement_s->currentResult;
	}
	measurement_s->currentResult = ( sum / (float) samples);
	
	//** Convert Current to corresponding units**//
	if(measurement_s->currentResult >= 1000.00) {
		measurement_s->currentResult /= 1000;
		sprintf((char*)output_buffer, "\nCurrent: %.2fA\n\r", measurement_s->currentResult);
		} else if(measurement_s->currentResult >= 1.00) {
		sprintf((char*)output_buffer, "\nCurrent: %.2fmA\n\r", measurement_s->currentResult);
		} else if(measurement_s->currentResult >= 0.001) {
		measurement_s->currentResult *= 1000;
		sprintf((char*)output_buffer, "\nCurrent: %.2fuA\n\r", measurement_s->currentResult);
		} else {
		measurement_s->currentResult *= 1000000;
		sprintf((char*)output_buffer, "\nCurrent: %.0fnA\n\r", measurement_s->currentResult);
	}
	cdcdf_acm_write((uint8_t*)output_buffer, strlen((char*)output_buffer));
	delay_ms(10);
	usb_cli_task_set_finished();
}

void usb_cli_get_adc_resolution(currentMeasurement_t *measurement_s) {
	uint8_t output_buffer[64];
	uint8_t resolution = 0;
	switch (measurement_s->adcResolution)
	{
	case ADC_10BIT: resolution = 10; break;
	case ADC_12BIT: resolution = 12; break;
	case ADC_16BIT: resolution = 16; break;
	}
	sprintf((char*)output_buffer, "\nResolution: %dBit\n\n\rnanoLogger>", resolution);
	cdcdf_acm_write((uint8_t*)output_buffer, strlen((char*)output_buffer));
	delay_ms(10);
}

void usb_cli_set_external_trigger(nanoLogger_t *nLogger) {
	if(nLogger->deviceConfig.triggerConfig != NO_EXT_TRIGGER && nLogger->deviceConfig.deviceMode != DEVICE_ERROR) {
		switch(nLogger->deviceConfig.triggerConfig) {
			case RISING_EDGE: external_trigger_configure_falling_edge(); break; //inverted due to inverting input stage
			case FALLING_EDGE: external_trigger_configure_rising_edge(); break; //inverted due to inverting input stage
			default: break;
		}
		external_trigger_register(external_trigger_state_set_triggered);
		ext_irq_enable(PIN_PB03);
		external_trigger_state_set_armed();
		led_set_status_wait_for_trigger();
		switch(nLogger->deviceConfig.deviceMode){
			case ADC_RAW_MODE: led_set_mode_single_measurement(); break;
			case ADC_VOLTAGE_MODE: led_set_mode_single_measurement(); break;
			case CURRENT_MEASUREMENT_MODE: led_set_mode_single_measurement(); break;
			case CURRENT_MEASUREMENT_CONVERTED_MODE: led_set_mode_single_measurement(); break;
			case LOGGING_MODE: led_set_mode_logging(); break;
			default: break;
		}
		uint8_t output_buffer[64];
		sprintf((char*)output_buffer, "\nWaiting for Trigger...\n\r");
		cdcdf_acm_write((uint8_t*)output_buffer, strlen((char*)output_buffer));
		delay_ms(10);
	}
}

void usb_cli_logging(nanoLogger_t *nLogger) {
	static uint8_t logging_sample_buffer[80000]; //80kb sample buffer to store values until transmit
	static bool transmitSampleMemoryBlockA = true;
	if(get_TIMER_LOGGING_status() == TIMER_LOGGING_HALTED && get_usb_cli_task_status() == USB_TASK_NOT_FINISHED) {
		if(nLogger->deviceConfig.sampleRate > 1000 && nLogger->currentMeasurement.adcResolution == ADC_16BIT) {
			uint8_t output_buffer[] = "\nFor sample rates higher than 1kHz, switch to 10- or 12Bit resolution.\n\r";
			cdcdf_acm_write((uint8_t*)output_buffer, strlen((char*)output_buffer));
			delay_ms(10);
			usb_cli_task_set_finished();
		} else if(nLogger->deviceConfig.sampleRate == 0 || nLogger->deviceConfig.recordLength == 0) {
			uint8_t output_buffer[] = "\nSampleRate or RecordLength can't be 0.\n\r";
			cdcdf_acm_write((uint8_t*)output_buffer, strlen((char*)output_buffer));
			delay_ms(10);
			usb_cli_task_set_finished();
		}
		else TIMER_LOGGING_init_task(nLogger, logging_sample_buffer, &transmitSampleMemoryBlockA);
	}
	if(TIMER_LOGGING_data_status() == LOGGED_DATA_READY) {
		if(get_led_task_status() == LED_TASK_HALTED) timer0_led_usb_task_start();
		led_set_usb_communication_ongoing();
		TIMER_LOGGING_data_transmitted();
		if(transmitSampleMemoryBlockA == true) cdcdf_acm_write(logging_sample_buffer, nLogger->deviceConfig.logging_transmit_buffer_size);
		else if(transmitSampleMemoryBlockA == false) cdcdf_acm_write(logging_sample_buffer + 40000, nLogger->deviceConfig.logging_transmit_buffer_size);
		
	}
}

void usb_cli_handle_response(void) {
	
	// remove last 2 bytes from input buffer (checksum??) which cause issues if no '\n' or '\0' is send to indicate end of command string
	char *ptr;
	ptr = strchr((char*)usb_buffer, '\0');
	memset(ptr - 2, 0, 2);
	
	memcpy(usb_user_buffer, usb_buffer, sizeof(usb_user_buffer));
	memset(usb_buffer, 0, sizeof(usb_buffer));
	usb_data_received_status = USB_DATA_RECEIVED;
}

void usb_cli_process_received_data(nanoLogger_t *nLogger) {
	
	if(usb_data_received_status == USB_NO_DATA_RECEIVED) return;
	
	char *strToken;
	
	timer0_led_usb_task_start();
	led_set_usb_communication_ongoing();
			
	strToken = strtok((char*)usb_user_buffer, " ");
	if(strToken != NULL) {
		if(strncmp("getAdcRaw", strToken, 9) == 0 && nLogger->deviceConfig.deviceMode == DEVICE_IDLE) nLogger->deviceConfig.deviceMode = ADC_RAW_MODE;
		else if(strncmp("getAdcVoltage", strToken, 13) == 0 && nLogger->deviceConfig.deviceMode == DEVICE_IDLE) nLogger->deviceConfig.deviceMode = ADC_VOLTAGE_MODE;
		else if(strncmp("getCurrentRange", strToken, 15) == 0 && nLogger->deviceConfig.deviceMode == DEVICE_IDLE) usb_cli_get_current_range(&nLogger->currentMeasurement);
		else if(strncmp("getCurrentConverted", strToken, 19) == 0 && nLogger->deviceConfig.deviceMode == DEVICE_IDLE) nLogger->deviceConfig.deviceMode = CURRENT_MEASUREMENT_CONVERTED_MODE;
		else if(strncmp("getCurrent", strToken, 10) == 0 && nLogger->deviceConfig.deviceMode == DEVICE_IDLE) nLogger->deviceConfig.deviceMode = CURRENT_MEASUREMENT_MODE;
		else if(strncmp("startLogging", strToken, 12) == 0 && nLogger->deviceConfig.deviceMode == DEVICE_IDLE) nLogger->deviceConfig.deviceMode = LOGGING_MODE;
		else if(strncmp("deviceCalibration", strToken, 17) == 0 && nLogger->deviceConfig.deviceMode == DEVICE_IDLE) nLogger->deviceConfig.deviceMode = DEVICE_CALIBRATION;
		else if(strncmp("getAdcResolution", strToken, 16) == 0 && nLogger->deviceConfig.deviceMode == DEVICE_IDLE) usb_cli_get_adc_resolution(&nLogger->currentMeasurement);
		else if(strncmp("help", strToken, 4) == 0 && nLogger->deviceConfig.deviceMode == DEVICE_IDLE) usb_cli_get_help();
		else if(strncmp("deviceCalibration", strToken, 17) == 0 && nLogger->deviceConfig.deviceMode == DEVICE_IDLE) usb_cli_device_calibration(nLogger);
		else if(strncmp("cancel", strToken, 6) == 0) usb_cli_cancel_operation(nLogger);
		else if(nLogger->deviceConfig.deviceMode == DEVICE_IDLE) usb_cli_error(nLogger);
	}

	while(strToken != NULL) {
		strToken = strtok(NULL, " ");
		if(strToken == NULL) break;
				
		if(strncmp("Trigger:", strToken, 8) == 0) {
			char *ptr;
			ptr = strchr(strToken, ':');
			if(strncmp("neg", ptr + 1, 3) == 0) nLogger->deviceConfig.triggerConfig = FALLING_EDGE;
			else if(strncmp("pos", ptr + 1, 3) == 0) nLogger->deviceConfig.triggerConfig = RISING_EDGE;
			else nLogger->deviceConfig.triggerConfig = NO_EXT_TRIGGER;
			usb_cli_set_external_trigger(nLogger);
		}
				
		else if(strncmp("SampleRate:", strToken, 11) == 0) {
			char *ptr;
			uint32_t value;
			ptr = strchr(strToken, ':');
			value = strtol(ptr + 1, NULL, 10);
			nLogger->deviceConfig.sampleRate = value;
		}
				
		else if(strncmp("RecordLength:", strToken, 13) == 0) {
			char *ptr;
			uint32_t value;
			ptr = strchr(strToken, ':');
			value = strtol(ptr + 1, NULL, 10);
			nLogger->deviceConfig.recordLength = value;
		}
			
		else if(strncmp("Samples:", strToken, 8) == 0) {
			char *ptr;
			uint32_t value;
			ptr = strchr(strToken, ':');
			value = strtol(ptr + 1, NULL, 10);
			nLogger->deviceConfig.samples = value;
		}
		
		else if(strncmp("Resolution:", strToken, 11) == 0) {
			char *ptr;
			ptr = strchr(strToken, ':');
			if(strncmp("10Bit", ptr + 1, 5) == 0) nLogger->currentMeasurement.adcResolution = ADC_10BIT;
			else if(strncmp("12Bit", ptr + 1, 5) == 0) nLogger->currentMeasurement.adcResolution = ADC_12BIT;
			else if(strncmp("16Bit", ptr + 1, 5) == 0) nLogger->currentMeasurement.adcResolution = ADC_16BIT;
			set_adc_resolution(&nLogger->currentMeasurement);
		}
		
		else if(nLogger->deviceConfig.deviceMode == DEVICE_CALIBRATION) {
			if(strncmp("ShowCalibration", strToken, 15) == 0) {
				uint8_t output_buffer[256];
				sprintf((char*)output_buffer, "\nCalibration:\nAdcRef:%.6f\nAdcGainCorrection:%.6f\nRange1:%.6f\nRange2:%.6f\nRange3:%.6f\nRange4:%.6f\nRange5:%.6f\n\r", nLogger->currentMeasurement.adcRefVoltage, nLogger->currentMeasurement.adc_gain_correction, nLogger->currentMeasurement.range1_cal_value, nLogger->currentMeasurement.range2_cal_value, nLogger->currentMeasurement.range3_cal_value, nLogger->currentMeasurement.range4_cal_value,nLogger->currentMeasurement.range5_cal_value);
				cdcdf_acm_write((uint8_t*)output_buffer, strlen((char*)output_buffer));
				delay_ms(10);
			}
			
			else if(strncmp("SetAdcRef:", strToken, 10) == 0) {
				char *ptr;
				ptr = strchr(strToken, ':');
				nLogger->currentMeasurement.adcRefVoltage = strtof(ptr + 1, NULL);
				smarteeprom_write_32bit((uint32_t *) &nLogger->currentMeasurement.adcRefVoltage, SMARTEEPROM_ADC_REF_INDEX);
			}
			
			else if(strncmp("SetRangeOffset1:", strToken, 16) == 0) {
				char *ptr;
				ptr = strchr(strToken, ':');
				nLogger->currentMeasurement.range1_cal_value = strtof(ptr + 1, NULL);
				smarteeprom_write_32bit((uint32_t *) &nLogger->currentMeasurement.range1_cal_value, SMARTEEPROM_RANGE1_CAL_INDEX);
			}
			
			else if(strncmp("SetRangeOffset2:", strToken, 16) == 0) {
				char *ptr;
				ptr = strchr(strToken, ':');
				nLogger->currentMeasurement.range2_cal_value = strtof(ptr + 1, NULL);
				smarteeprom_write_32bit((uint32_t *) &nLogger->currentMeasurement.range2_cal_value, SMARTEEPROM_RANGE2_CAL_INDEX);
			}
			
			else if(strncmp("SetRangeOffset3:", strToken, 16) == 0) {
				char *ptr;
				ptr = strchr(strToken, ':');
				nLogger->currentMeasurement.range3_cal_value = strtof(ptr + 1, NULL);
				smarteeprom_write_32bit((uint32_t *) &nLogger->currentMeasurement.range3_cal_value, SMARTEEPROM_RANGE3_CAL_INDEX);
			}
			
			else if(strncmp("SetRangeOffset4:", strToken, 16) == 0) {
				char *ptr;
				ptr = strchr(strToken, ':');
				nLogger->currentMeasurement.range4_cal_value = strtof(ptr + 1, NULL);
				smarteeprom_write_32bit((uint32_t *) &nLogger->currentMeasurement.range4_cal_value, SMARTEEPROM_RANGE4_CAL_INDEX);
			}
			
			else if(strncmp("SetRangeOffset5:", strToken, 16) == 0) {
				char *ptr;
				ptr = strchr(strToken, ':');
				nLogger->currentMeasurement.range5_cal_value = strtof(ptr + 1, NULL);
				smarteeprom_write_32bit((uint32_t *) &nLogger->currentMeasurement.range5_cal_value, SMARTEEPROM_RANGE5_CAL_INDEX);
			}
			
			else if(strncmp("SetAdcGainCorrection:", strToken, 21) == 0) {
				char *ptr;
				ptr = strchr(strToken, ':');
				nLogger->currentMeasurement.adc_gain_correction = strtof(ptr + 1, NULL);
				smarteeprom_write_32bit((uint32_t *) &nLogger->currentMeasurement.adc_gain_correction, SMARTEEPROM_GAIN_CORR_INDEX);
				set_adc_gaincorrection(&nLogger->currentMeasurement);
			}
			
			else {
				uint8_t output_buffer[] = "\nError Unknown Calibration Command!\n\r";
				led_set_mode_error();
				nLogger->deviceConfig.deviceMode = DEVICE_ERROR;
				cdcdf_acm_write((uint8_t*)output_buffer, strlen((char*)output_buffer));
				delay_ms(10);
			}
		}
		
		else {
			uint8_t output_buffer[] = "\nError Unknown Command Extension!\n\r";
			led_set_mode_error();
			nLogger->deviceConfig.deviceMode = DEVICE_ERROR;
			cdcdf_acm_write((uint8_t*)output_buffer, strlen((char*)output_buffer));
			delay_ms(10);
		}
				
	}	
	
	cdcdf_acm_write(usb_buffer, 0);
	delay_ms(10);
	usb_data_received_status = USB_NO_DATA_RECEIVED;
}

void usb_cli_execute_operation(nanoLogger_t *nLogger) {
	
	if(nLogger->deviceConfig.deviceMode != DEVICE_IDLE && get_external_trigger_state() == EXT_TRIGGER_TRIGGERED) {
		
		if(nLogger->deviceConfig.deviceMode != LOGGING_MODE) timer0_led_task_start();
		
		switch(nLogger->deviceConfig.deviceMode) {
			case ADC_RAW_MODE: usb_cli_get_adc_raw_average(&nLogger->currentMeasurement, nLogger->deviceConfig.samples); break;
			case ADC_VOLTAGE_MODE: usb_cli_get_adc_voltage_average(&nLogger->currentMeasurement, nLogger->deviceConfig.samples); break;
			case CURRENT_MEASUREMENT_MODE: usb_cli_get_current(&nLogger->currentMeasurement, nLogger->deviceConfig.samples); break;
			case CURRENT_MEASUREMENT_CONVERTED_MODE: usb_cli_get_current_converted(&nLogger->currentMeasurement, nLogger->deviceConfig.samples); break;
			case LOGGING_MODE: usb_cli_logging(nLogger); break;
			case DEVICE_CALIBRATION: {
				led_set_mode_calibration();
				usb_cli_task_set_finished();
				break;
			}
			default: usb_cli_task_set_finished(); break;
		}
	}
	
	if(get_usb_cli_task_status() == USB_TASK_FINISHED) {
		uint8_t output_buffer[64];
		if(get_led_task_status() == LED_TASK_HALTED) timer0_led_task_start();
		led_set_usb_communication_ongoing();
		led_mode_task_done();
		led_status_task_done();
		delay_ms(10);
		sprintf((char*)output_buffer, "\nnanoLogger>");
		cdcdf_acm_write((uint8_t*)output_buffer, strlen((char*)output_buffer));
		delay_ms(10);
		nLogger->deviceConfig.deviceMode = DEVICE_IDLE;
		nLogger->deviceConfig.samples = 1;
		nLogger->deviceConfig.sampleRate = 0;
		nLogger->deviceConfig.recordLength = 0;
		usb_task_status = USB_TASK_NOT_FINISHED;
	}
}