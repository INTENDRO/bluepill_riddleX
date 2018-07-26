#include <stm32f10x.h>

void wait_1ms(uint16_t u16Factor) // using timer 4
{
	uint16_t i;
	
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	TIM4->CR1 = 0x0000;
	TIM4->CR2 = 0x0000;
	TIM4->SMCR = 0x0000;
	TIM4->DIER = 0x0000;
	TIM4->CCMR1 = 0x0000;
	TIM4->CCMR2 = 0x0000;
	TIM4->CCER = 0x0000;
	TIM4->PSC = 1; //prescaler:2
	TIM4->ARR = SystemCoreClock/2000; //-> 1ms/((fsys/prescaler)^(-1))
	TIM4->EGR = TIM_EGR_UG;
	
	TIM4->SR &= ~TIM_SR_UIF;
	TIM4->CR1 |= TIM_CR1_CEN;
	
	for(i=0;i<u16Factor;i++)
	{
		while(!(TIM4->SR & TIM_SR_UIF));
		TIM4->SR &= ~TIM_SR_UIF;
	}
	TIM4->CR1 = 0x0000;
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
    //(DMA)
    
    //set baudrate in BRR
    USART1->BRR = SystemCoreClock/u32baudrate; //mantissa: 0x27 (->39) fraction: 0x1 (->0.0625)
    //set TE bit in CR1 (->idle frame)
    USART1->CR1 |= USART_CR1_TE;
    USART1->CR1 |= USART_CR1_RE;
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
    return USART1->DR;
}



int main(void)
{
    uint8_t u8counter = 0;
    uint8_t u8data;
    
    SystemInit();
    
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	
	GPIOC->CRH = GPIO_CRH_MODE13_0;
    GPIOC->ODR |= GPIO_ODR_ODR13;
    
    usartInit(115200);
    wait_1ms(10);
    usartClearFlagsAndBuffer();
    wait_1ms(100);
    
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
}
