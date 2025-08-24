/*
 * external_trigger.c
 *
 * Created: 29.01.2022 13:38:23
 *  Author: Tobias
 */ 

#include "external_trigger.h"

static external_trigger_status_t trigger_state = EXT_TRIGGER_TRIGGERED;

void external_trigger_configure_rising_edge(void) {

	if (!hri_eic_is_syncing(EIC, EIC_SYNCBUSY_SWRST)) {
		if (hri_eic_get_CTRLA_reg(EIC, EIC_CTRLA_ENABLE)) {
			hri_eic_clear_CTRLA_ENABLE_bit(EIC);
			hri_eic_wait_for_sync(EIC, EIC_SYNCBUSY_ENABLE);
		}
		hri_eic_write_CTRLA_reg(EIC, EIC_CTRLA_SWRST);
	}
	hri_eic_wait_for_sync(EIC, EIC_SYNCBUSY_SWRST);

	hri_eic_write_CONFIG_reg(EIC,
	0,
	(CONF_EIC_FILTEN0 << EIC_CONFIG_FILTEN0_Pos) | EIC_CONFIG_SENSE0(CONF_EIC_SENSE0)
	| (CONF_EIC_FILTEN1 << EIC_CONFIG_FILTEN1_Pos) | EIC_CONFIG_SENSE1(CONF_EIC_SENSE1)
	| (CONF_EIC_FILTEN2 << EIC_CONFIG_FILTEN2_Pos) | EIC_CONFIG_SENSE2(CONF_EIC_SENSE2)
	| (CONF_EIC_FILTEN3 << EIC_CONFIG_FILTEN3_Pos) | EIC_CONFIG_SENSE3(EIC_NMICTRL_NMISENSE_RISE_Val)
	| (CONF_EIC_FILTEN4 << EIC_CONFIG_FILTEN4_Pos) | EIC_CONFIG_SENSE4(CONF_EIC_SENSE4)
	| (CONF_EIC_FILTEN5 << EIC_CONFIG_FILTEN5_Pos) | EIC_CONFIG_SENSE5(CONF_EIC_SENSE5)
	| (CONF_EIC_FILTEN6 << EIC_CONFIG_FILTEN6_Pos) | EIC_CONFIG_SENSE6(CONF_EIC_SENSE6)
	| (CONF_EIC_FILTEN7 << EIC_CONFIG_FILTEN7_Pos) | EIC_CONFIG_SENSE7(CONF_EIC_SENSE7)
	| 0);
	
	hri_eic_set_CTRLA_ENABLE_bit(EIC);
	NVIC_DisableIRQ(EIC_3_IRQn);
	NVIC_ClearPendingIRQ(EIC_3_IRQn);
	NVIC_EnableIRQ(EIC_3_IRQn);
}

void external_trigger_configure_falling_edge(void) {

	if (!hri_eic_is_syncing(EIC, EIC_SYNCBUSY_SWRST)) {
		if (hri_eic_get_CTRLA_reg(EIC, EIC_CTRLA_ENABLE)) {
			hri_eic_clear_CTRLA_ENABLE_bit(EIC);
			hri_eic_wait_for_sync(EIC, EIC_SYNCBUSY_ENABLE);
		}
		hri_eic_write_CTRLA_reg(EIC, EIC_CTRLA_SWRST);
	}
	hri_eic_wait_for_sync(EIC, EIC_SYNCBUSY_SWRST);

	hri_eic_write_CONFIG_reg(EIC,
	0,
	(CONF_EIC_FILTEN0 << EIC_CONFIG_FILTEN0_Pos) | EIC_CONFIG_SENSE0(CONF_EIC_SENSE0)
	| (CONF_EIC_FILTEN1 << EIC_CONFIG_FILTEN1_Pos) | EIC_CONFIG_SENSE1(CONF_EIC_SENSE1)
	| (CONF_EIC_FILTEN2 << EIC_CONFIG_FILTEN2_Pos) | EIC_CONFIG_SENSE2(CONF_EIC_SENSE2)
	| (CONF_EIC_FILTEN3 << EIC_CONFIG_FILTEN3_Pos) | EIC_CONFIG_SENSE3(EIC_NMICTRL_NMISENSE_FALL_Val)
	| (CONF_EIC_FILTEN4 << EIC_CONFIG_FILTEN4_Pos) | EIC_CONFIG_SENSE4(CONF_EIC_SENSE4)
	| (CONF_EIC_FILTEN5 << EIC_CONFIG_FILTEN5_Pos) | EIC_CONFIG_SENSE5(CONF_EIC_SENSE5)
	| (CONF_EIC_FILTEN6 << EIC_CONFIG_FILTEN6_Pos) | EIC_CONFIG_SENSE6(CONF_EIC_SENSE6)
	| (CONF_EIC_FILTEN7 << EIC_CONFIG_FILTEN7_Pos) | EIC_CONFIG_SENSE7(CONF_EIC_SENSE7)
	| 0);
	
	hri_eic_set_CTRLA_ENABLE_bit(EIC);
	NVIC_DisableIRQ(EIC_3_IRQn);
	NVIC_ClearPendingIRQ(EIC_3_IRQn);
	NVIC_EnableIRQ(EIC_3_IRQn);
}

void external_trigger_register(ext_irq_cb_t cb) {
	ext_irq_register(PIN_PB03, cb);
}

void external_trigger_state_set_armed(void) {
	trigger_state = EXT_TRIGGER_ARMED;
}

void external_trigger_state_set_triggered(void) {
	ext_irq_disable(PIN_PB03);
	trigger_state = EXT_TRIGGER_TRIGGERED;
}

external_trigger_status_t get_external_trigger_state(void)  {
	return trigger_state;
}