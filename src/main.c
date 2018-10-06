/*
******************************************************************************
File:     main.c
Info:     Generated by Atollic TrueSTUDIO(R) 9.0.1   2018-08-27

The MIT License (MIT)
Copyright (c) 2018 STMicroelectronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

******************************************************************************
*/

/*
 * TO DO:
 * - crc8 / crc16 (check if crc8 does not produce any errors)
 * - use a timeout while waiting for usart to be available (systick maybe?)
 * - define error codes in header files
 * - put usart init in sup -> usart should not be accessed in the main file
 * - sup receive function: send and receive can return the same errorcodes for different errors -> generate a unique error table (defines)
 * - possibility: sw interrupt when separator has been received -> no check required in main loop
 * - use a ringbuffer as the send buffer for sup packets. the buffer can be appended with a new command during the transmission of another one. this way the usart does not havt to wait for the calculation of the packets
 */

/* Includes */

#include "stm32f10x.h"
#include "ringbuffer.h"
#include "utils.h"
#include "usart.h"
#include "sup.h"
#include "test_module.h"


int main(void)
{
	RingBuff_t* RingBuffer_ptr;
	uint8_t au8rawData[SUP_BUFFER_SIZE];
	uint16_t u16rawDataLength,u16dataLength,u16temp;
	int8_t s8retVal;
	uint8_t u8temp;


	SystemInit();

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

	GPIOC->CRH = GPIO_CRH_MODE13_0;
	GPIOC->ODR |= GPIO_ODR_ODR13;

	usartInit(115200);

	wait_1ms(10);
	usartClearFlagsAndBuffer();
	wait_1ms(100);
	RingBuffer_ptr = usartGetRingBuffPointer();

	init_module();
	s8retVal = sup_get_properties(&u8temp,0,0);
	s8retVal = sup_get_properties(&u8temp,0,1);
	s8retVal = sup_get_properties(&u8temp,0,2);
	s8retVal = sup_get_properties(&u8temp,1,0);

	s8retVal = sup_get_type(&u8temp,0,0);
	s8retVal = sup_get_type(&u8temp,0,1);
	s8retVal = sup_get_type(&u8temp,0,2);
	s8retVal = sup_get_type(&u8temp,1,0);



	/*
	au8data[0] = 0x12;
	au8data[1] = 0x34;
	au8data[2] = 0x57;
	sup_send_busy();
	sup_send(0x02,au8data,0x03);
	sup_send_busy();
	*/


	/*
	au8data[0] = 0xFF;
	sup_send_busy();
	sup_send(0x0,au8data,0x01);
	sup_send_busy();
	*/

	__enable_irq();


	while(1)
	{
		if(RingBuffer_CountData(RingBuffer_ptr,0x7E))
		{
			u16rawDataLength = RingBuffer_RemoveUntilDelimiter(RingBuffer_ptr,au8rawData,SUP_BUFFER_SIZE,0x7E);
			s8retVal = sup_receive(au8rawData,u16rawDataLength);

		}
//		GPIOC->ODR ^= GPIO_ODR_ODR13;
//		wait_1ms(100);
	}
}

