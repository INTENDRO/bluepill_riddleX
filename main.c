#include <stm32f10x.h>

#include "ringbuffer.h"
#include "utils.h"
#include "usart.h"
#include "sup.h"


uint8_t au8usartSendData[80];
uint8_t au8usartReceiveData[80];
uint8_t au8data[80];



//uint8_t au8temp[80];



int main(void)
{
    uint8_t u8dataType,u8dataLength;
    int8_t s8retVal;
    RingBuff_t* RingBuffer_ptr;
    uint8_t au8temp[80];
    uint16_t u16temp;
    
    SystemInit();
    
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	
    GPIOC->CRH = GPIO_CRH_MODE13_0;
    GPIOC->ODR |= GPIO_ODR_ODR13;
    
    usartInit(115200);
    wait_1ms(10);
    usartClearFlagsAndBuffer();
    wait_1ms(100);
    RingBuffer_ptr = usartGetRingBuffPointer();
    
    //__enable_irq();
    
    
    RingBuffer_Insert(RingBuffer_ptr,0x7E);
    RingBuffer_Insert(RingBuffer_ptr,0x02);
    RingBuffer_Insert(RingBuffer_ptr,0x7C);
    RingBuffer_Insert(RingBuffer_ptr,0xDF);
    RingBuffer_Insert(RingBuffer_ptr,0x6A);
    RingBuffer_Insert(RingBuffer_ptr,0x40);
    RingBuffer_Insert(RingBuffer_ptr,0x7E);
    RingBuffer_Insert(RingBuffer_ptr,0x40);
    RingBuffer_Insert(RingBuffer_ptr,0xC7);
    RingBuffer_Insert(RingBuffer_ptr,0x7E);
    
    while(1)
    {
        if(RingBuffer_CountData(RingBuffer_ptr,0x7E) != 0)
        {
            u16temp = RingBuffer_RemoveUntilDelimiter(RingBuffer_ptr,au8temp,80,0x7E);
            if(u16temp != 0)
            {
                u16temp = sup_receive(au8temp,&u8dataType,&u8dataLength,au8temp,u16temp);
                
                if(u16temp == 0)
                {
                    sup_send(au8temp,u8dataType,u8dataLength);
                }
            }
            
        }
        
        wait_1ms(100);
    }
    
//-------------------------------------    
    
//    au8usartSendData[0] = 0x7E;
//    au8usartSendData[1] = 0x02;
//    au8usartSendData[2] = 0x7C;
//    au8usartSendData[3] = 0xDF;
//    au8usartSendData[4] = 0x6A;
//    au8usartSendData[5] = 0x40;
//    au8usartSendData[6] = 0x7E;
//    usartDMASend(au8usartSendData,7);
//    wait_1ms(100);
//    usartSendByte(RingBuffer_GetCount(RingBuffer_ptr));
    
//-------------------------------------
    
    
//    au8usartReceiveData[0] = 0x7E;
//    au8usartReceiveData[1] = 0x02;
//    au8usartReceiveData[2] = 0x7C;
//    au8usartReceiveData[3] = 0xDF;
//    au8usartReceiveData[4] = 0x6A;
//    au8usartReceiveData[5] = 0x40;
//    au8usartReceiveData[6] = 0x7E;
//    
//    while(1)
//    {
//        s8retVal = sup_receive(&au8data[0],&u8dataType,&u8dataLength,&au8usartReceiveData[0],7);
//        //usartDMASend(&s8retVal,1);
//        usartDMASend(&au8data[0],u8dataLength);
//        wait_1ms(1000);
//    }

//-------------------------------------
    
//    au8usartReceiveData[0] = 0x7E;
//    au8usartReceiveData[1] = 0x04;
//    au8usartReceiveData[2] = 0xFB;
//    au8usartReceiveData[3] = 0xBE;
//    au8usartReceiveData[4] = 0xA0;
//    au8usartReceiveData[5] = 0x0F;
//    au8usartReceiveData[6] = 0xA3;
//    au8usartReceiveData[7] = 0xC0;
//    au8usartReceiveData[8] = 0x7E;
//    
//    while(1)
//    {
//        s8retVal = sup_receive(&au8data[0],&u8dataType,&u8dataLength,&au8usartReceiveData[0],9);
//        //usartDMASend(&s8retVal,1);
//        usartDMASend(&au8data[0],u8dataLength);
//        wait_1ms(1000);
//    }
    

//-------------------------------------

//    au8data[0] = 0x7D;
//    au8data[1] = 0xBF;
//    
//    while(1)
//    {
//        sup_send(&au8data[0],0,2);
//        wait_1ms(1000);
//    }
    
//-------------------------------------


//    while(1)
//    {
//        sup_send(&au8data[0],1,0);
//        wait_1ms(1000);
//    }
    
//-------------------------------------
//    au8data[0] = 0xFF;
//    au8data[1] = 0x7E;
//    au8data[2] = 0x80;
//    au8data[3] = 0x3F;
//    
//    while(1)
//    {
//        sup_send(&au8data[0],0,4);
//        wait_1ms(1000);
//    }
    
    
}


/*
while(1)
    {
        if(u8usartDataReady)
        {
            u8usartDataReady = 0;
            usartSendByte(u8usartData);
        }
    }
    
    while(1)
    {
        usartSendByte(0x3A);
        wait_1ms(1000);
    }
    
    while(1)
    {
        usartDMASend(&au8usartSendData[0],4);
        wait_1ms(500);
        usartSendByte(0x3A);
        wait_1ms(500);
    }
    
    
    usartSendByte(0x3A);
    
    while(1)
    {
        if(usartDataAvailable() == 1)
        {
            u8data = usartGetByte();
            usartSendByte(u8data);
        }
        wait_1ms(100);
    }
    

    while(1)
    {
        usartSendByte(u8counter++);
        //USART1->CR1 |= USART_CR1_SBK;
        GPIOC->ODR = 0;
        wait_1ms(100);
        GPIOC->ODR = GPIO_ODR_ODR13;
        wait_1ms(900);
    }
*/
