/*
 * current_logging.h
 *
 * Created: 04.02.2022 22:14:22
 *  Author: Tobias
 */ 


#ifndef CURRENT_LOGGING_H_
#define CURRENT_LOGGING_H_

typedef enum timer_logging_status_e {
	TIMER_LOGGING_HALTED,
	TIMER_LOGGING_RUNNING
}timer_logging_status_t;

typedef enum logged_data_status_e {
	LOGGED_DATA_NOT_READY,
	LOGGED_DATA_READY
	}logged_data_status_t;

timer_logging_status_t get_TIMER_LOGGING_status(void);

logged_data_status_t TIMER_LOGGING_data_status(void);

void TIMER_LOGGING_data_transmitted(void);

void TIMER_LOGGING_end_task(void);

void TIMER_LOGGING_init_task(nanoLogger_t* nLogger, uint8_t* buffer, bool *tramsmitMemoryblock);


#endif /* CURRENT_LOGGING_H_ */