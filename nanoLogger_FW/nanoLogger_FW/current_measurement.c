/*
 * current_measurement.c
 *
 * Created: 27.11.2021 23:42:54
 *  Author: Tobias
 */ 

#include "current_measurement.h"

int32_t set_adc_samplenum(struct adc_sync_descriptor *const descr, const adc_samplenum_t samplenum) {
	ASSERT(descr);
	_set_adc_samplenum(&descr->device, samplenum);
	
	return ERR_NONE;
}

void _set_adc_samplenum(struct _adc_sync_device *const device, const adc_samplenum_t samplenum) {
	hri_adc_write_AVGCTRL_SAMPLENUM_bf(device->hw, samplenum);
}

int32_t adc_set_gaincorrection(struct adc_sync_descriptor *const descr, const hri_adc_gaincorr_reg_t gaincorr) {
	ASSERT(descr);
	_set_adc_gaincorrection(&descr->device, gaincorr);

	return ERR_NONE;
}

void _set_adc_gaincorrection(struct _adc_sync_device *const device, const hri_adc_gaincorr_reg_t gaincorr) {
	hri_adc_write_GAINCORR_reg(device->hw, gaincorr);
}

void set_adc_gaincorrection(currentMeasurement_t *config) {
	adc_set_gaincorrection(&ADC_0, (2048.0 / config->adc_gain_correction));
}

int32_t set_adc_prescaler(struct adc_sync_descriptor *const descr, const hri_adc_ctrla_reg_t prescaler) {
	ASSERT(descr);
	_set_adc_prescaler(&descr->device, prescaler);

	return ERR_NONE;
}

void _set_adc_prescaler(struct _adc_sync_device *const device, const hri_adc_ctrla_reg_t prescaler) {
	hri_adc_write_CTRLA_PRESCALER_bf(device->hw, prescaler);
}

int32_t set_adc_rightshifts(struct adc_sync_descriptor *const descr, const hri_adc_avgctrl_reg_t rightshifts) {
	ASSERT(descr);
	_set_adc_rightshifts(&descr->device, rightshifts);
	
	return ERR_NONE;
}

void _set_adc_rightshifts(struct _adc_sync_device *const device, const hri_adc_avgctrl_reg_t rightshifts) {
	hri_adc_write_AVGCTRL_ADJRES_bf(device->hw, rightshifts);
}

void set_adc_resolution(currentMeasurement_t *config) {
	if(config->adcResolution == ADC_10BIT) {
		config->adc_LSB_MV = config->adcRefVoltage / 1023.0f;
		adc_sync_set_resolution(&ADC_0, ADC_CTRLB_RESSEL_16BIT_Val);
		set_adc_rightshifts(&ADC_0, 3);
		set_adc_samplenum(&ADC_0, ADC_AVGCTRL_SAMPLENUM_2);
		adc_sync_disable_channel(&ADC_0, 0);
		set_adc_prescaler(&ADC_0, ADC_CTRLA_PRESCALER_DIV4_Val);
		adc_sync_enable_channel(&ADC_0, 0);
		
	}
	if(config->adcResolution == ADC_12BIT) {
		config->adc_LSB_MV = config->adcRefVoltage / 4095.0f;
		adc_sync_set_resolution(&ADC_0, ADC_CTRLB_RESSEL_16BIT_Val);
		set_adc_rightshifts(&ADC_0, 1);
		set_adc_samplenum(&ADC_0, ADC_AVGCTRL_SAMPLENUM_2);
		adc_sync_disable_channel(&ADC_0, 0);
		set_adc_prescaler(&ADC_0, ADC_CTRLA_PRESCALER_DIV4_Val);
		adc_sync_enable_channel(&ADC_0, 0);
	}
	if(config->adcResolution == ADC_16BIT) {
		config->adc_LSB_MV = config->adcRefVoltage / 65535.0f;
		adc_sync_set_resolution(&ADC_0, ADC_CTRLB_RESSEL_16BIT_Val);
		set_adc_rightshifts(&ADC_0, 0);
		set_adc_samplenum(&ADC_0, ADC_AVGCTRL_SAMPLENUM_256);
		adc_sync_disable_channel(&ADC_0, 0);
		set_adc_prescaler(&ADC_0, ADC_CTRLA_PRESCALER_DIV2_Val);
		adc_sync_enable_channel(&ADC_0, 0);
	}
}

void read_adc_raw(currentMeasurement_t *measurement_s) {
	uint8_t value8Bit[2];
	led_set_status_measurement_ongoing();
	adc_sync_read_channel(&ADC_0, 0, value8Bit, 2);
	measurement_s->adcRawValue = (value8Bit[0] | (value8Bit[1] << 8));
	if(get_led_task_status() == LED_TASK_HALTED) led_set_status_off();
}

void read_adc_voltage(currentMeasurement_t *measurement_s) {
	read_adc_raw(measurement_s);
	measurement_s->adcResult_MV = (measurement_s->adcRawValue * measurement_s->adc_LSB_MV * 2.0f);
}

void read_adc_raw_averages(currentMeasurement_t *measurement_s, uint32_t numOfReadings) {
	uint32_t sum = 0;
	for(uint32_t i = 0; i < numOfReadings; i++) {
		read_adc_raw(measurement_s);
		sum += measurement_s->adcRawValue;
	}
}

void read_adc_voltage_averages(currentMeasurement_t *measurement_s, uint32_t numOfReadings) {
	uint32_t sum = 0;
	for(uint32_t i = 0; i < numOfReadings; i++) {
		read_adc_raw(measurement_s);
		sum += measurement_s->adcRawValue;
	}
	measurement_s->adcResult_MV = ((((float) sum / (float) numOfReadings) * measurement_s->adc_LSB_MV) * 2.0f);
}

void get_current_range(currentMeasurement_t *range) {
	uint32_t portIN = REG_PORT_IN0;
	range->currentRange = RANGEERR;
	if((portIN & (1 << 12)) == 0) range->currentRange = RANGE1;
	else if((portIN & (1 << 11)) == 0) range->currentRange = RANGE2;
	else if((portIN & (1 << 10)) == 0) range->currentRange = RANGE3;
	else if((portIN & (1 << 9)) == 0) range->currentRange = RANGE4;
	else if((portIN & (1 << 8)) == 0) range->currentRange = RANGE5;
	
	/*if(gpio_get_pin_level(RANGE_SWITCH_POSITION_1) == false) range->currentRange = RANGE1;
	else if(gpio_get_pin_level(RANGE_SWITCH_POSITION_2) == false) range->currentRange = RANGE2;
	else if(gpio_get_pin_level(RANGE_SWITCH_POSITION_3) == false) range->currentRange = RANGE3;
	else if(gpio_get_pin_level(RANGE_SWITCH_POSITION_4) == false) range->currentRange = RANGE4;
	else if(gpio_get_pin_level(RANGE_SWITCH_POSITION_5) == false) range->currentRange = RANGE5;*/
}

void get_current(currentMeasurement_t *measurement_s) {
	get_current_range(measurement_s);
	switch (measurement_s->currentRange)
	{
	case RANGE1: {
		measurement_s->currentResult = measurement_s->adcResult_MV / 1000000 + measurement_s->range1_cal_value;
		if(measurement_s->currentResult < 0) measurement_s->currentResult = 0;
		break;
	}
	case RANGE2: measurement_s->currentResult = measurement_s->adcResult_MV / 10000 + measurement_s->range2_cal_value; break;
	case RANGE3: measurement_s->currentResult = measurement_s->adcResult_MV / 1000 + measurement_s->range3_cal_value; break;
	case RANGE4: measurement_s->currentResult = measurement_s->adcResult_MV / 10 + measurement_s->range4_cal_value; break;
	case RANGE5: measurement_s->currentResult = measurement_s->adcResult_MV / 1 + measurement_s->range5_cal_value; break;
	case RANGEERR: break; // handle ranging error
	}
	
}