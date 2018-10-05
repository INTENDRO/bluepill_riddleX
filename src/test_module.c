/*
 * test_module.c
 *
 *  Created on: 3 Oct 2018
 *      Author: Mario
 */

#include "stm32f10x.h"
#include "test_module.h"
#include "sup.h"

static uint8_t u8reg_control_value = 7;
static sup_register_t reg_control = {(void*)(&u8reg_control_value),SUP_DATA_PROPERTY_WRITE | SUP_DATA_PROPERTY_READ,SUP_DATA_TYPE_UINT8};

static uint16_t u16reg_data_value = 0x1234;
static sup_register_t reg_data = {(void*)(&u16reg_data_value),SUP_DATA_PROPERTY_READ,SUP_DATA_TYPE_UINT16};

static sup_register_t* reg_array[2] = {&reg_control,&reg_data};

static sup_module_t test_module = {reg_array,2};





void init_module(void)
{
	sup_init_module(0,&test_module);

}
