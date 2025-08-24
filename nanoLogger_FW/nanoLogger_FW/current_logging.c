/*
 * current_logging.c
 *
 * Created: 04.02.2022 22:14:04
 *  Author: Tobias
 */ 

#include "driver_init.h"
#include "utils.h"
#include "external_trigger.h"
#include "current_measurement.h"
#include "userTypedefs.h"
#include "StatusLeds.h"
#include "user_cli.h"

static struct timer_task TIMER_LOGGING_task;
static nanoLogger_t* nLoggerptr;
static timer_logging_status_t timer_logging_status = TIMER_LOGGING_HALTED;
static logged_data_status_t logger_data_status = LOGGED_DATA_NOT_READY;
static uint8_t *sample_buffer; // pointer to main 80kb sample buffer
static uint32_t samples_to_record = 0;
static uint32_t samples_taken = 0;
static uint32_t samples_in_buffer = 0;
static bool sampleToMemoryBlockA = true;
static bool *transmitSampleMemoryBlockA_ptr;
static currentRange_t rangeBeforeMeasurement;

//** This implementation of float to bytes only works on little-endian machines **//
static union {
	float float_value;
	uint8_t bytes[4];
} float_to_bytes;


timer_logging_status_t get_TIMER_LOGGING_status(void) {
	return timer_logging_status;
}

logged_data_status_t TIMER_LOGGING_data_status(void) {
	return logger_data_status;
}

void TIMER_LOGGING_data_transmitted(void) {
	logger_data_status = LOGGED_DATA_NOT_READY;
}

void TIMER_LOGGING_end_task(void) {
	timer_stop(&TIMER_LOGGING);
	timer_remove_task(&TIMER_LOGGING, &TIMER_LOGGING_task);
	timer_logging_status = TIMER_LOGGING_HALTED;
}

static void TIMER_LOGGING_task_cb(const struct timer_task *const timer_task) {
	if(samples_taken < samples_to_record) {
		
		do {
			get_current_range(&nLoggerptr->currentMeasurement);
			rangeBeforeMeasurement = nLoggerptr->currentMeasurement.currentRange;
			read_adc_voltage(&nLoggerptr->currentMeasurement);
			get_current(&nLoggerptr->currentMeasurement);
		} while(rangeBeforeMeasurement != nLoggerptr->currentMeasurement.currentRange); // make sure no range switching has happened during measurement
		
		float_to_bytes.float_value = nLoggerptr->currentMeasurement.currentResult;
		
		// select Sample Memory Block to use
		if(sampleToMemoryBlockA == true) {
			sample_buffer[samples_in_buffer] = float_to_bytes.bytes[0];
			sample_buffer[samples_in_buffer + 1] = float_to_bytes.bytes[1];
			sample_buffer[samples_in_buffer + 2] = float_to_bytes.bytes[2];
			sample_buffer[samples_in_buffer + 3] = float_to_bytes.bytes[3];
		} else {
			sample_buffer[samples_in_buffer + 40000] = float_to_bytes.bytes[0];
			sample_buffer[samples_in_buffer + 40001] = float_to_bytes.bytes[1];
			sample_buffer[samples_in_buffer + 40002] = float_to_bytes.bytes[2];
			sample_buffer[samples_in_buffer + 40003] = float_to_bytes.bytes[3];
		}
		
		
		samples_taken++;
		
		if(samples_in_buffer == nLoggerptr->deviceConfig.logging_transmit_buffer_size - 4) {
			*transmitSampleMemoryBlockA_ptr = sampleToMemoryBlockA;
			logger_data_status = LOGGED_DATA_READY;
			samples_in_buffer = 0;
			sampleToMemoryBlockA = !sampleToMemoryBlockA;
		} else samples_in_buffer += 4; // 4 bytes are necessary to accommodate a single float
		
	} else {	
		samples_taken = 0;
		samples_in_buffer = 0;
		nLoggerptr->deviceConfig.recordLength = 0;
		nLoggerptr->deviceConfig.sampleRate = 0;
		//nLoggerptr->deviceConfig.logging_transmit_buffer_size = 0;
		nLoggerptr->deviceConfig.triggerConfig = NO_EXT_TRIGGER;
		usb_cli_task_set_finished();
		TIMER_LOGGING_end_task();
	}
}


void TIMER_LOGGING_init_task(nanoLogger_t *nLogger, uint8_t *buffer, bool *tramsmitMemoryblock) {
	
	timer_logging_status = TIMER_LOGGING_RUNNING;
	logger_data_status = LOGGED_DATA_NOT_READY;
	
	nLoggerptr = nLogger;
	sample_buffer = buffer;
	
	samples_taken = 0;
	samples_in_buffer = 0;
	
	transmitSampleMemoryBlockA_ptr = tramsmitMemoryblock;
	
	if(nLogger->deviceConfig.sampleRate % 50 == 0 && nLogger->deviceConfig.sampleRate > 49) {
		nLogger->deviceConfig.logging_transmit_buffer_size = (nLogger->deviceConfig.sampleRate / 50) * 4; // samples are stored as Bytes (float = 4 Bytes)
	} else {
		nLogger->deviceConfig.logging_transmit_buffer_size = nLogger->deviceConfig.sampleRate * 4; // samples are stored as Bytes (float = 4 Bytes)
	}
		
	
	uint32_t timer_interval = (1 / (double)nLogger->deviceConfig.sampleRate) * 10000;
	samples_to_record = nLogger->deviceConfig.recordLength / ((1 / (double)nLogger->deviceConfig.sampleRate) * 1000);
		
	TIMER_LOGGING_task.interval = timer_interval; // 1 tick = 100us -> max. Sample Rate 10ksps
	TIMER_LOGGING_task.cb       = TIMER_LOGGING_task_cb;
	TIMER_LOGGING_task.mode     = TIMER_TASK_REPEAT;
	
	timer_add_task(&TIMER_LOGGING, &TIMER_LOGGING_task);
	timer_start(&TIMER_LOGGING);
	led_set_mode_logging();
}