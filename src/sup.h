/*
 * sup.h
 *
 *  Created on: 3 Oct 2018
 *      Author: Mario
 */

#ifndef SUP_H_
#define SUP_H_

#include "sup_ll_driver.h"
#include "ringbuffer.h"

typedef enum
{
	UINT8,
	UINT16,
	UINT32,
	INT8,
	INT16,
	INT32
} sup_data_type_t;

#define SUP_DATA_TYPE_UINT8		0x00
#define SUP_DATA_TYPE_UINT16	0x01
#define SUP_DATA_TYPE_UINT32	0x02

#define SUP_DATA_PROPERTY_WRITE 0x01
#define SUP_DATA_PROPERTY_READ 0x02

#define SUP_MAX_MODULE_COUNT 3


typedef struct
{
	void* const data;
	const uint8_t properties;
	const uint8_t type;
} sup_register_t;


typedef struct
{
	sup_register_t** const reg_array;
	const uint8_t reg_count;
} sup_module_t;


#define WRITE_SYS			0x00
#define WRITE_SYS_REPLY		0x01
#define READ_SYS			0x02
#define READ_SYS_REPLY		0x03
#define WRITE_DATA			0x04
#define WRITE_DATA_REPLY	0x05
#define READ_DATA			0x06
#define READ_DATA_REPLY		0x07
#define SETUP_JOB			0x08
#define SETUP_JOB_REPLY		0x09
#define CHANGE_JOB			0x0A
#define CHANGE_JOB_REPLY	0x0B
#define DELETE_JOB			0x0C
#define DELETE_JOB_REPLY	0x0D
#define GET_JOB				0x0E
#define GET_JOB_REPLY		0x0F
#define START_JOB			0x10
#define START_JOB_REPLY		0x11
#define STOP_JOB			0x12
#define STOP_JOB_REPLY		0x13
#define JOB_DATA			0x14

#define SUP_BUFFER_SIZE 	SUP_LL_BUFFER_SIZE
#define SUP_MAX_LENGTH 		SUP_LL_MAX_LENGTH
#define SUP_MIN_LENGTH 		SUP_LL_MIN_LENGTH


int8_t sup_init(uint32_t u32baudrate);
RingBuff_t* sup_get_ringbuffer_ptr(void);
int8_t sup_init_module(uint8_t module_nr,sup_module_t* module);
int8_t sup_get_properties(uint8_t* pu8properties, uint8_t u8module_nr, uint8_t u8reg_nr);
int8_t sup_get_type(uint8_t* pu8type, uint8_t u8module_nr, uint8_t u8reg_nr);
int8_t sup_send(uint8_t* u8dataToSend_ptr, uint16_t u16length);
uint8_t sup_send_isbusy(void);
int8_t sup_receive(uint8_t* u8rawData_ptr, uint16_t u16rawDataLength);


#endif /* SUP_H_ */
