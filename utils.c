#include <stm32f10x.h>
#include "utils.h"

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
	TIM3->ARR = 9; // -> 1ms 
    TIM3->CNT = 0;
	TIM3->DIER = TIM_DIER_UIE;
	TIM3->EGR = TIM_EGR_UG;
	TIM3->SR &= ~TIM_SR_UIF;
	
	NVIC_ClearPendingIRQ(TIM3_IRQn);
	NVIC_EnableIRQ(TIM3_IRQn);
	
	TIM3->CR1 |= TIM_CR1_CEN;
}

void TIM3_IRQHandler(void)
{
	TIM3->SR &= ~TIM_SR_UIF;
	NVIC_ClearPendingIRQ(TIM3_IRQn);
}