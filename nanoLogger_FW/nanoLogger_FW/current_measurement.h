/*
 * current_measurement.h
 *
 * Created: 27.11.2021 23:42:21
 *  Author: Tobias
 */ 


#ifndef CURRENT_MEASUREMENT_H_
#define CURRENT_MEASUREMENT_H_

#include <atmel_start.h>
#include <stdio.h>
#include <hpl_adc_sync.h>
#include "userTypedefs.h"
#include "StatusLeds.h"

int32_t set_adc_samplenum(struct adc_sync_descriptor *const descr, const adc_samplenum_t samplenum);

void _set_adc_samplenum(struct _adc_sync_device *const device, const adc_samplenum_t samplenum);

int32_t adc_set_gaincorrection(struct adc_sync_descriptor *const descr, const hri_adc_gaincorr_reg_t gaincorr);

void _set_adc_gaincorrection(struct _adc_sync_device *const device, const hri_adc_gaincorr_reg_t gaincorr);

void set_adc_gaincorrection(currentMeasurement_t *config);

int32_t set_adc_prescaler(struct adc_sync_descriptor *const descr, const hri_adc_ctrla_reg_t prescaler);

void _set_adc_prescaler(struct _adc_sync_device *const device, const hri_adc_ctrla_reg_t prescaler);

int32_t set_adc_rightshifts(struct adc_sync_descriptor *const descr, const hri_adc_avgctrl_reg_t rightshifts);

void _set_adc_rightshifts(struct _adc_sync_device *const device, const hri_adc_avgctrl_reg_t rightshifts);

void set_adc_resolution(currentMeasurement_t *config);

void read_adc_raw(currentMeasurement_t *measurement_s);

void read_adc_voltage(currentMeasurement_t *measurement_s);

void read_adc_raw_averages(currentMeasurement_t *measurement_s, uint32_t numOfReadings);

void read_adc_voltage_averages(currentMeasurement_t *measurement_s, uint32_t numOfReadings);

void get_current_range(currentMeasurement_t *range);

void get_current(currentMeasurement_t *measurement_s);

#endif /* CURRENT_MEASUREMENT_H_ */