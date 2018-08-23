#include <stm32f10x.h>

#include "ringbuffer.h"
#include "utils.h"
#include "usart.h"
#include "sup.h"




int main(void)
{
    RingBuff_t* RingBuffer_ptr;
    uint8_t au8rawData[80];
    uint8_t au8data[80];
    uint16_t u16rawDataLength;
    uint8_t u8dataLength,u8dataType;
    int8_t s8retVal;
    
    
    SystemInit();
    
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	
    GPIOC->CRH = GPIO_CRH_MODE13_0;
    GPIOC->ODR |= GPIO_ODR_ODR13;
    
    usartInit(115200);
    wait_1ms(10);
    usartClearFlagsAndBuffer();
    wait_1ms(100);
    RingBuffer_ptr = usartGetRingBuffPointer();
    
    __enable_irq();


    while(1)
    {
        if(RingBuffer_CountData(RingBuffer_ptr,0x7E))
        {
            u16rawDataLength = RingBuffer_RemoveUntilDelimiter(RingBuffer_ptr,au8rawData,80,0x7E);
            s8retVal = sup_receive(au8data,&u8dataType,&u8dataLength,au8rawData,u16rawDataLength);
            if(s8retVal == 0)
            {
                while(usartBusy());
                sup_send(au8data,u8dataType,u8dataLength);
            }
        }
    }
}



