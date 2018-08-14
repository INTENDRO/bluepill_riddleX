#include <stm32f10x.h>
#include "usart.h"

//IRQ ONLY!!!////////////
uint8_t u8usartDataReady = 0;
uint8_t u8usartData = 0;
///////////////////////

void USART1_IRQHandler(void)
{
    volatile uint32_t u32temp;
    u32temp = USART1->SR;
    if(u32temp & 0x0000000F)
    {
        u32temp = USART1->DR;
    }
    else
    {
        u8usartData = USART1->DR;
        u8usartDataReady = 1;
    }
}

void usartSendByte(uint8_t u8data)
{
    while(!(USART1->SR & USART_SR_TXE));
    USART1->DR = u8data;
}

void usartInit(uint32_t u32baudrate)
{ 
    //enable gpio clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    GPIOA->CRH &= ~GPIO_CRH_CNF9 & ~GPIO_CRH_MODE9;
    GPIOA->CRH |= GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9_0;
    
    //enable usart clock
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    //reset registers
    USART1->CR1 = 0;
    USART1->CR2 = 0;
    USART1->CR3 = 0;
    //enable UE bit in CR1
    USART1->CR1 |= USART_CR1_UE;
    USART1->CR1 |= USART_CR1_RXNEIE;
    //DMA
    USART1->CR3 |= USART_CR3_DMAT;
    //set baudrate in BRR
    USART1->BRR = SystemCoreClock/u32baudrate; //mantissa: 0x27 (->39) fraction: 0x1 (->0.0625)
    //set TE bit in CR1 (->idle frame)
    USART1->CR1 |= USART_CR1_TE;
    USART1->CR1 |= USART_CR1_RE;
    
    NVIC_ClearPendingIRQ(USART1_IRQn);
	NVIC_EnableIRQ(USART1_IRQn);
}

void usartDMASend(uint8_t* u32data_ptr, uint16_t u16length)
{
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    
    DMA1_Channel4->CPAR = 0x40013804;
    DMA1_Channel4->CMAR = (uint32_t)u32data_ptr;
    DMA1_Channel4->CCR = 0;
    DMA1_Channel4->CCR |= DMA_CCR4_MINC;
    DMA1_Channel4->CCR |= DMA_CCR4_DIR;
    DMA1->IFCR = DMA_IFCR_CGIF4;
    DMA1_Channel4->CNDTR = (uint32_t)u16length;
    USART1->SR &= ~USART_SR_TC; //clear bit TC by writing 0 (other must not be affected! ->1)
    
    DMA1_Channel4->CCR |= DMA_CCR4_EN;
}

void usartClearFlagsAndBuffer(void)
{
    volatile uint8_t u8dummy;
    
    u8dummy = USART1->SR; //clear flags
    u8dummy = USART1->DR;
}

uint8_t usartDataAvailable(void)
{
    uint32_t u32temp;
    volatile uint8_t u8dummy;
    
    u32temp = USART1->SR;
    
    if(u32temp & USART_SR_RXNE)
    {
        if(u32temp & 0x0000000F) //any error has occurred
        {
            u8dummy = USART1->SR;
            u8dummy = USART1->DR;
            return 2;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 0;
    }
}

uint8_t usartGetByte(void)
{
    return (uint8_t)USART1->DR;
}



