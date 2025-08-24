/*
 * userTypedefs.h
 *
 * Created: 21.12.2021 21:19:42
 *  Author: Tobias
 */ 


#ifndef USERTYPEDEFS_H_
#define USERTYPEDEFS_H_

typedef uint8_t adc_samplenum_t;

typedef enum currentRange_e {
	RANGEERR,
	RANGE1,
	RANGE2,
	RANGE3,
	RANGE4,
	RANGE5
} currentRange_t;

typedef enum adcResolution_e {
	ADC_10BIT = 0x2,
	ADC_12BIT = 0x0,
	ADC_16BIT = 0x1
} adcResolution_t;

typedef enum deviceOperation_e {
	DEVICE_IDLE,
	DEVICE_ERROR,
	ADC_RAW_MODE,
	ADC_VOLTAGE_MODE,
	CURRENT_MEASUREMENT_MODE,
	CURRENT_MEASUREMENT_CONVERTED_MODE,
	LOGGING_MODE,
	DEVICE_CALIBRATION
} deviceOperation_t;

typedef enum externalTrigger_e {
	NO_EXT_TRIGGER,
	RISING_EDGE,
	FALLING_EDGE
} externalTrigger_t;

typedef struct currentMeasurement_s {
	currentRange_t currentRange;
	adcResolution_t adcResolution;
	uint16_t adcRawValue;
	float adcResult_MV;
	float adcRefVoltage;
	float adc_LSB_MV;
	float currentResult;
	float range1_cal_value;
	float range2_cal_value;
	float range3_cal_value;
	float range4_cal_value;
	float range5_cal_value;
	float adc_gain_correction;
} currentMeasurement_t;

typedef struct deviceConfiguration_s {
	deviceOperation_t deviceMode;
	externalTrigger_t triggerConfig;
	uint32_t sampleRate;
	uint32_t recordLength;
	uint32_t samples;
	uint32_t logging_transmit_buffer_size;
} deviceConfiguration_t;

typedef struct nanoLogger_s {
	currentMeasurement_t currentMeasurement;
	deviceConfiguration_t deviceConfig;
}nanoLogger_t;

#endif /* USERTYPEDEFS_H_ */