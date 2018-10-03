/*
 * sup.h
 *
 *  Created on: 3 Oct 2018
 *      Author: Mario
 */

#ifndef SUP_H_
#define SUP_H_


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
	void* data;
	uint8_t properties;
	uint8_t type;
} sup_register_t;

typedef struct
{
	sup_register_t** reg_array;
	uint8_t reg_count;
} sup_module_t;


int8_t sup_init_module(uint8_t module_nr,sup_module_t module);


#endif /* SUP_H_ */
