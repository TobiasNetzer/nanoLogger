#include <atmel_start.h>
#include <stdio.h>
#include "current_measurement.h"
#include "user_cli.h"
#include "StatusLeds.h"
#include "smarteeprom_user_parameters.h"

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	adc_sync_enable_channel(&ADC_0, 0);
	
	usb_cli_init();
	
	//** Default Device Config **//
	nanoLogger_t nanoLogger;
	nanoLogger.deviceConfig.deviceMode = DEVICE_IDLE;
	nanoLogger.deviceConfig.triggerConfig = NO_EXT_TRIGGER;
	nanoLogger.deviceConfig.sampleRate = 0;
	nanoLogger.deviceConfig.recordLength = 0;
	nanoLogger.deviceConfig.samples = 1;
	nanoLogger.currentMeasurement.adcResolution = ADC_16BIT;
	
	//** Load Calibration Parameters from SmartEEPROM or Write Default Values **//
	if(smarteeprom_load_user_parameters(&nanoLogger) == SMARTEEPROM_SIGNATURE_INVALID) smarteeprom_write_user_parameters_default(&nanoLogger);
	
	//** Set Default ADC Resolution & Gain correction **//
	set_adc_resolution(&nanoLogger.currentMeasurement);
	set_adc_gaincorrection(&nanoLogger.currentMeasurement);
	
	//** Init LED Task **//
	timer0_led_task_init();

	while (1) {
		
		usb_cli_process_received_data(&nanoLogger);
		usb_cli_execute_operation(&nanoLogger);
		
	}
}
