/*
 * external_trigger.h
 *
 * Created: 29.01.2022 13:38:43
 *  Author: Tobias
 */ 


#ifndef EXTERNAL_TRIGGER_H_
#define EXTERNAL_TRIGGER_H_

#include "hal_ext_irq.h"
#include "hpl_eic_config.h"
#include "hri_eic_d51.h"
#include "samd51g18a.h"

typedef enum external_trigger_status_e {
	EXT_TRIGGER_ARMED,
	EXT_TRIGGER_TRIGGERED
	} external_trigger_status_t;

void external_trigger_configure_rising_edge(void);

void external_trigger_configure_falling_edge(void);

void external_trigger_register(ext_irq_cb_t cb);

void external_trigger_state_set_armed(void);

void external_trigger_state_set_triggered(void);

external_trigger_status_t get_external_trigger_state(void);

#endif /* EXTERNAL_TRIGGER_H_ */