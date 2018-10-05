/*
 * sup.c
 *
 *  Created on: 3 Oct 2018
 *      Author: Mario
 */

#include "stm32f10x.h"
#include "sup.h"

static sup_module_t* module_array[SUP_MAX_MODULE_COUNT] = {0};



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


