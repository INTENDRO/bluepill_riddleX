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

void INT_1ms_init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	
	TIM3->CR1 = 0x0000;
	TIM3->CR2 = 0x0000;
	TIM3->SMCR = 0x0000;
	TIM3->CCMR1 = 0x0000;
	TIM3->CCMR2 = 0x0000;
	TIM3->CCER = 0x0000;
	TIM3->PSC = 7199; //10kHz @ 72Mhz
	TIM3->ARR = 19; // -> 2ms (500Hz)
    TIM3->CNT = 0;
	TIM3->DIER = TIM_DIER_UIE;
	TIM3->EGR = TIM_EGR_UG;
	TIM3->SR &= ~TIM_SR_UIF;
	//enable int 29 
	//NVIC_ClearPendingIRQ(TIM3_IRQn);
	//NVIC_EnableIRQ(TIM3_IRQn);
  //NVIC->ICPR[0] = NVIC_ICPR_CLRPEND_29;
  //NVIC->ISER[0] = NVIC_ISER_SETENA_29;
	
	NVIC_ClearPendingIRQ(TIM3_IRQn);
	NVIC_EnableIRQ(TIM3_IRQn);
	
	TIM3->CR1 |= TIM_CR1_CEN;
}

void TIM3_IRQHandler(void)
{
	GPIOC->ODR ^= GPIO_ODR_ODR13;
    INTflag = 1;
	TIM3->SR &= ~TIM_SR_UIF;
	NVIC_ClearPendingIRQ(TIM3_IRQn);
}

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
    
    __enable_irq();
    
    
//-------------------------------------    
    INT_1ms_init();
    
    au8data[0] = 0;
    au8data[1] = 0;
    au8data[2] = 0;
    u32temp = 0;
    u16errorCounter = 0;
    
    for(i=0;i<10;i++)
    {
        au32errors[i] = 0;
    }
    

    while(1)
    {
        if(INTflag)
        {
            if(errorFlag)
            {
                au32errors[u16errorCounter] = u32temp-1;
                u16errorCounter++;
                if(u16errorCounter==10)
                {
                    //toomany errors
                    break;
                }
                
                wait_1ms(10);
                RingBuffer_Clear(RingBuffer_ptr);
                usartClearFlagsAndBuffer();
                wait_1ms(10);
            }
            
            INTflag = 0;
            errorFlag = 1;
            sup_send(au8data,0,3);
            wait_1ms(1); //wait for data to be send (dma uses the array)
            
            //decode loopback here
            
            
            errorFlag = 0;
            u32temp++;
            au8data[0] = u32temp&0xFF;
            au8data[1] = (u32temp>>8)&0xFF;
            au8data[2] = (u32temp>>16)&0xFF;
            if(u32temp == 0x5)
            {
                break;
            }
        }
    }
    
    __disable_irq();
    
    while(1)
    {
        //print errors here
        GPIOC->ODR ^= GPIO_ODR_ODR13;
        usartSendByte((u16errorCounter>>8)&0xFF);
        usartSendByte(u16errorCounter&0xFF);
        wait_1ms(1000);
    }
    
//-------------------------------------    
    
//    RingBuffer_Insert(RingBuffer_ptr,0x7E);
//    RingBuffer_Insert(RingBuffer_ptr,0x02);
//    RingBuffer_Insert(RingBuffer_ptr,0x7C);
//    RingBuffer_Insert(RingBuffer_ptr,0xDF);
//    RingBuffer_Insert(RingBuffer_ptr,0x6A);
//    RingBuffer_Insert(RingBuffer_ptr,0x40);
//    RingBuffer_Insert(RingBuffer_ptr,0x7E);
//    RingBuffer_Insert(RingBuffer_ptr,0x40);
//    RingBuffer_Insert(RingBuffer_ptr,0xC7);
//    RingBuffer_Insert(RingBuffer_ptr,0x7E);
//    
//    while(1)
//    {
//        if(RingBuffer_CountData(RingBuffer_ptr,0x7E) != 0)
//        {
//            u16temp = RingBuffer_RemoveUntilDelimiter(RingBuffer_ptr,au8temp,80,0x7E);
//            if(u16temp != 0)
//            {
//                u16temp = sup_receive(au8temp,&u8dataType,&u8dataLength,au8temp,u16temp);
//                
//                if(u16temp == 0)
//                {
//                    sup_send(au8temp,u8dataType,u8dataLength);
//                }
//            }
//            
//        }
//        
//        wait_1ms(100);
//    }
    
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
    
    
//-------------------------------------
//    au8data[0] = 0xFC;
//    au8data[1] = 0xE2;
//    au8data[2] = 0x37;
//    
//    while(1)
//    {
//        sup_send(&au8data[0],0,3);
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
