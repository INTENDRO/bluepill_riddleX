/*
 * sup.c
 *
 *  Created on: 3 Oct 2018
 *      Author: Mario
 */

#include "stm32f10x.h"
#include "utils.h"
#include "sup.h"
#include "sup_ll_driver.h"
#include "ringbuffer.h"

static sup_module_t* module_array[SUP_MAX_MODULE_COUNT] = {0};


int8_t sup_init(uint32_t u32baudrate)
{
	return sup_ll_init(u32baudrate);
}

RingBuff_t* sup_get_ringbuffer_ptr(void)
{
	return sup_ll_get_ringbuffer_ptr();
}

int8_t sup_init_module(uint8_t module_nr,sup_module_t* module)
{
	module_array[module_nr] = module;
	return 0;
}

int8_t sup_get_properties(uint8_t* pu8properties, uint8_t u8module_nr, uint8_t u8reg_nr)
{
	if(u8module_nr >= SUP_MAX_MODULE_COUNT)
	{
		return -1;
	}

	if(module_array[u8module_nr] == 0)
	{
		return -2;
	}


	if(u8reg_nr >= module_array[u8module_nr]->reg_count)
	{
		return -3;
	}

	*pu8properties = module_array[u8module_nr]->reg_array[u8reg_nr]->properties;

	return 0;
}

int8_t sup_get_type(uint8_t* pu8type, uint8_t u8module_nr, uint8_t u8reg_nr)
{
	if(u8module_nr >= SUP_MAX_MODULE_COUNT)
	{
		return -1;
	}

	if(module_array[u8module_nr] == 0)
	{
		return -2;
	}

	if(u8reg_nr >= module_array[u8module_nr]->reg_count)
	{
		return -3;
	}

	*pu8type = module_array[u8module_nr]->reg_array[u8reg_nr]->type;

	return 0;
}

int8_t sup_send(uint8_t* u8dataToSend_ptr, uint16_t u16length)
{
	return sup_ll_send(u8dataToSend_ptr,u16length);
}

uint8_t sup_send_isbusy(void)
{
    return sup_ll_send_isbusy();
}

int8_t sup_receive(uint8_t* u8rawData_ptr, uint16_t u16rawDataLength)
{
	int8_t s8retVal;
	uint8_t au8data[SUP_MAX_LENGTH];
	uint16_t u16dataLength;
	uint32_t u32timeout_hold,u32temp;


	s8retVal = sup_ll_receive(au8data,&u16dataLength,u8rawData_ptr,u16rawDataLength);

	if(s8retVal == 0)
	{
		switch(au8data[0])
		{
		case WRITE_SYS:

		break;

		case WRITE_SYS_REPLY:

		break;

		case READ_SYS:

		break;

		case READ_SYS_REPLY:

		break;

		case WRITE_DATA:

		break;

		case WRITE_DATA_REPLY:

		break;

		case READ_DATA:

		break;

		case READ_DATA_REPLY:

		break;

		case SETUP_JOB:

		break;

		case SETUP_JOB_REPLY:

		break;

		case CHANGE_JOB:

		break;

		case CHANGE_JOB_REPLY:

		break;

		case DELETE_JOB:

		break;

		case DELETE_JOB_REPLY:

		break;

		case GET_JOB:

		break;

		case GET_JOB_REPLY:

		break;

		case START_JOB:

		break;

		case START_JOB_REPLY:

		break;

		case STOP_JOB:

		break;

		case STOP_JOB_REPLY:

		break;

		case JOB_DATA:

		break;
		}

		u32timeout_hold = get_sys_tick();
		while(sup_send_isbusy())
		{
			u32temp = get_sys_tick();
			if(u32temp < u32timeout_hold)
			{
				if((0xFFFFFFFF - u32timeout_hold - u32temp + 1) >= 50)
				{
					//UART FROZEN! NEEDS TO BE RESET!!!!
					return -1;
				}
			}
			else
			{
				if((u32temp - u32timeout_hold) >= 50)
				{
					//UART FROZEN! NEEDS TO BE RESET!!!!
					return -1;
				}
			}
		}
		s8retVal = sup_send(au8data,u16dataLength);
		return s8retVal;
	}
	return s8retVal;
}

