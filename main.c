#include <stm32f10x.h>

#include "ringbuffer.h"
#include "utils.h"
#include "usart.h"
#include "sup.h"


uint8_t au8usartSendData[80];
uint8_t au8usartReceiveData[80];
uint8_t au8data[80];
uint8_t INTflag = 0;



//uint8_t au8temp[80];



int main(void)
{
    uint8_t u8dataType,u8dataLength;
    int8_t s8retVal;
    RingBuff_t* RingBuffer_ptr;
    uint8_t au8temp[80];
    uint16_t u16temp,i,u16errorCounter;
    uint32_t u32temp;
    uint32_t au32errors[10];
    uint8_t errorFlag = 0;
    
    
    SystemInit();
    
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	
    GPIOC->CRH = GPIO_CRH_MODE13_0;
    GPIOC->ODR |= GPIO_ODR_ODR13;
    
    usartInit(115200);
    wait_1ms(10);
    usartClearFlagsAndBuffer();
    wait_1ms(100);
    RingBuffer_ptr = usartGetRingBuffPointer();
    
    INT_1ms_init();
    
    __enable_irq();


    while(1)
    {
        
    }

}



