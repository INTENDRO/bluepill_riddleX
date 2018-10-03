/*
 * sup.c
 *
 *  Created on: 3 Oct 2018
 *      Author: Mario
 */

#include "stm32f10x.h"
#include "sup.h"

static sup_module_t module_array[SUP_MAX_MODULE_COUNT] = {0};

int8_t sup_init_module(uint8_t module_nr,sup_module_t module)
{
	module_array[module_nr] = module;
}
