/*
 * smarteeprom_user_parameters.c
 *
 * Created: 06.02.2022 00:34:52
 *  Author: Tobias
 */ 

#include "smarteeprom_user_parameters.h"

smarteeprom_status_t smarteeprom_write_32bit(uint32_t *data, uint32_t smarteeprom_index) {
	/* Define a pointer to access SmartEEPROM as words (32-bits) */
	uint32_t *SmartEEPROM32 = (uint32_t *)SEEPROM_ADDR;
	if(smarteeprom_index > (SMARTEEPROM_FINAL_BYTE_INDEX - sizeof(uint32_t))) return SMARTEEPROM_ERROR_BAD_ADDRESS;
	while (NVMCTRL->SEESTAT.bit.BUSY);
	SmartEEPROM32[smarteeprom_index] = *data;
	return SMARTEEPROM_SUCCESS;
}

smarteeprom_status_t smarteeprom_read_32bit(uint32_t *data, uint32_t smarteeprom_index) {
	/* Define a pointer to access SmartEEPROM as words (32-bits) */
	uint32_t *SmartEEPROM32 = (uint32_t *)SEEPROM_ADDR;
	if(smarteeprom_index > (SMARTEEPROM_FINAL_BYTE_INDEX - sizeof(uint32_t))) return SMARTEEPROM_ERROR_BAD_ADDRESS;
	while (NVMCTRL->SEESTAT.bit.BUSY);
	*data = SmartEEPROM32[smarteeprom_index];
	return SMARTEEPROM_SUCCESS;
}

smarteeprom_signature_t smarteeprom_load_user_parameters(nanoLogger_t *nLogger) {
	uint32_t smarteeprom_signature = 0;
	
	smarteeprom_read_32bit(&smarteeprom_signature, SMARTEEPROM_SIGNATURE_INDEX);
	if(smarteeprom_signature != SMARTEEPROM_DATA_VALID_SIGNATURE) return SMARTEEPROM_SIGNATURE_INVALID;
	
	smarteeprom_read_32bit((uint32_t *) &nLogger->currentMeasurement.adcRefVoltage, SMARTEEPROM_ADC_REF_INDEX);
	smarteeprom_read_32bit((uint32_t *) &nLogger->currentMeasurement.range1_cal_value, SMARTEEPROM_RANGE1_CAL_INDEX);
	smarteeprom_read_32bit((uint32_t *) &nLogger->currentMeasurement.range2_cal_value, SMARTEEPROM_RANGE2_CAL_INDEX);
	smarteeprom_read_32bit((uint32_t *) &nLogger->currentMeasurement.range3_cal_value, SMARTEEPROM_RANGE3_CAL_INDEX);
	smarteeprom_read_32bit((uint32_t *) &nLogger->currentMeasurement.range4_cal_value, SMARTEEPROM_RANGE4_CAL_INDEX);
	smarteeprom_read_32bit((uint32_t *) &nLogger->currentMeasurement.range5_cal_value, SMARTEEPROM_RANGE5_CAL_INDEX);
	smarteeprom_read_32bit((uint32_t *) &nLogger->currentMeasurement.adc_gain_correction, SMARTEEPROM_GAIN_CORR_INDEX);
	
	leds_load_calibration();
	
	return SMARTEEPROM_SIGNATURE_VALID;
}

smarteeprom_signature_t smarteeprom_write_user_parameters_default(nanoLogger_t* nLogger) {
	uint32_t smarteeprom_signature = SMARTEEPROM_DATA_VALID_SIGNATURE;
	
	//** Default Calibration Values **//
	nLogger->currentMeasurement.adcRefVoltage = 1500.0f;
	nLogger->currentMeasurement.range1_cal_value = 0.0f;
	nLogger->currentMeasurement.range2_cal_value = 0.0f;
	nLogger->currentMeasurement.range3_cal_value = 0.0f;
	nLogger->currentMeasurement.range4_cal_value = 0.0f;
	nLogger->currentMeasurement.range5_cal_value = 0.0f;
	nLogger->currentMeasurement.adc_gain_correction = 1.0f;
	
	smarteeprom_write_32bit((uint32_t *) &smarteeprom_signature, SMARTEEPROM_SIGNATURE_INDEX);
	smarteeprom_write_32bit((uint32_t *) &nLogger->currentMeasurement.adcRefVoltage, SMARTEEPROM_ADC_REF_INDEX);
	smarteeprom_write_32bit((uint32_t *) &nLogger->currentMeasurement.range1_cal_value, SMARTEEPROM_RANGE1_CAL_INDEX);
	smarteeprom_write_32bit((uint32_t *) &nLogger->currentMeasurement.range2_cal_value, SMARTEEPROM_RANGE2_CAL_INDEX);
	smarteeprom_write_32bit((uint32_t *) &nLogger->currentMeasurement.range3_cal_value, SMARTEEPROM_RANGE3_CAL_INDEX);
	smarteeprom_write_32bit((uint32_t *) &nLogger->currentMeasurement.range4_cal_value, SMARTEEPROM_RANGE4_CAL_INDEX);
	smarteeprom_write_32bit((uint32_t *) &nLogger->currentMeasurement.range5_cal_value, SMARTEEPROM_RANGE5_CAL_INDEX);
	smarteeprom_write_32bit((uint32_t *) &nLogger->currentMeasurement.adc_gain_correction, SMARTEEPROM_GAIN_CORR_INDEX);

	leds_restore_calibration();
	
	return SMARTEEPROM_DEFAULTS_RESTORED;
}