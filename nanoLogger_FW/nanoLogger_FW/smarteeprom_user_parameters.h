/*
* smarteeprom_user_parameters.h
*
* Created: 06.02.2022 00:35:37
*  Author: Tobias
*/


#ifndef SMARTEEPROM_USER_PARAMETERS_H_
#define SMARTEEPROM_USER_PARAMETERS_H_
#include "driver_init.h"
#include "utils.h"
#include "userTypedefs.h"
#include "StatusLeds.h"
#include <string.h>

/* SBLK = 1 and PSZ = 3, thus total size is 4096 bytes */
#define SMARTEEPROM_FINAL_BYTE_INDEX 4095

#define SMARTEEPROM_DATA_VALID_SIGNATURE 0xB5B5B5B5

typedef enum smarteeprom_signature_e {
	SMARTEEPROM_SIGNATURE_INVALID,
	SMARTEEPROM_SIGNATURE_VALID,
	SMARTEEPROM_DEFAULTS_RESTORED
} smarteeprom_signature_t;
	
typedef enum smarteeprom_status_e {
	SMARTEEPROM_ERROR_BAD_ADDRESS,
	SMARTEEPROM_SUCCESS
} smarteeprom_status_t;
	
typedef enum smarteeprom_user_parameter_index_e {
	SMARTEEPROM_SIGNATURE_INDEX = 0,
	SMARTEEPROM_ADC_REF_INDEX,
	SMARTEEPROM_RANGE1_CAL_INDEX,
	SMARTEEPROM_RANGE2_CAL_INDEX,
	SMARTEEPROM_RANGE3_CAL_INDEX,
	SMARTEEPROM_RANGE4_CAL_INDEX,
	SMARTEEPROM_RANGE5_CAL_INDEX,
	SMARTEEPROM_GAIN_CORR_INDEX
} smarteeprom_user_parameter_index_t;
	
	
smarteeprom_status_t smarteeprom_write_32bit(uint32_t *data, uint32_t smarteeprom_index);

smarteeprom_status_t smarteeprom_read_32bit(uint32_t *data, uint32_t smarteeprom_index);

smarteeprom_signature_t smarteeprom_load_user_parameters(nanoLogger_t *nLogger);

smarteeprom_signature_t smarteeprom_write_user_parameters_default(nanoLogger_t *nLogger);


#endif /* FLASH_USER_PARAMETERS_H_ */