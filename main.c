#include <stm32f10x.h>
#include <string.h>


uint8_t au8usartData[80];
uint8_t au8data[80];
uint8_t u8usartDataReady = 0;
uint8_t u8usartData = 0;
uint8_t au8temp[80];

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

uint8_t crc8(uint8_t* u8data_ptr, uint8_t u8length)
{
	uint8_t u8crc,i,j,u8temp,u8inByte;

	u8crc = 0;

	for(i=0;i<u8length;i++)
	{
		u8inByte = u8data_ptr[i];
		for(j=0;j<8;j++)
		{
			u8temp = (u8crc^u8inByte)&0x80;
			u8crc <<= 1;
			if(u8temp!=0)
			{
				u8crc ^= 0x07;
			}
			u8inByte<<=1;
		}
	}

	return u8crc;
}

void usart_prot_get_packet(uint8_t* u8packet_ptr, uint8_t* u8rawData_ptr, uint8_t u8length, uint8_t u8dataType)
{
    *u8packet_ptr = ((u8dataType&0x03)<<6) | (u8length&0x3F);
    memcpy(u8packet_ptr+1,u8rawData_ptr,u8length);
    *(u8packet_ptr+u8length+1) = crc8(u8packet_ptr,u8length+1);
}

void usart_prot_stuff(uint8_t* u8stuffed_ptr, uint8_t u8stuffedLength, uint8_t* u8data_ptr, uint8_t u8dataLength)
{
    
}


void usart_prot_send(uint8_t* u8dataToSend_ptr,uint8_t u8dataType,uint8_t u8length)
{
    uint16_t u16dataBitCounter,u16destBitCounter,u16dataMaxBit,u16consecutiveOnes,i;
    
    usart_prot_get_packet(&au8temp[0],u8dataToSend_ptr,u8length,u8dataType);
    
    
    au8usartData[0] = 0x7E; //delimiter
    
    u16dataBitCounter = 0;
    u16destBitCounter = 0;
    u16consecutiveOnes = 0;
    u16dataMaxBit = (((uint16_t)u8length)+2)*8;
    
    while(u16dataBitCounter < u16dataMaxBit)
    {
        if(au8temp[u16dataBitCounter/8] & (0x80>>(u16dataBitCounter%8))) //data is a 1
        {
            au8usartData[(u16destBitCounter/8)+1] |= (0x80 >> (u16destBitCounter%8)); //set dest bit
            u16destBitCounter++;
            
            u16consecutiveOnes++;
            if(u16consecutiveOnes==5)
            {
                u16consecutiveOnes = 0;
                au8usartData[(u16destBitCounter/8)+1] &= ~(0x80 >> (u16destBitCounter%8)); //stuff bit
                u16destBitCounter++;
            }
            
        }
        else //data is a 0
        {
            u16consecutiveOnes = 0;
            au8usartData[(u16destBitCounter/8)+1] &= ~(0x80 >> (u16destBitCounter%8)); //clear dest bit
            u16destBitCounter++;
        }
        u16dataBitCounter++;
        
    }
    
    for(i=0;i<((8-(u16destBitCounter%8))%8);i++)
    {
        au8usartData[(u16destBitCounter/8)+1] &= ~(0x80 >> (u16destBitCounter%8)); //clear dest bit
    }
    
    au8usartData[u16destBitCounter/8+2] = 0x7E; //delimiter
    
    usartDMASend(&au8usartData[0],u16destBitCounter/8+3);
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
    
    //__enable_irq();
    
    au8data[0] = 0x7D;
    au8data[1] = 0xBF;
    
    while(1)
    {
        usart_prot_send(&au8data[0],0,2);
        wait_1ms(1000);
    }
    
    /*
    au8data[0] = 0xFF;
    au8data[1] = 0x7E;
    au8data[2] = 0x80;
    au8data[3] = 0x3F;
    
    while(1)
    {
        usart_prot_send(&au8data[0],0,4);
        wait_1ms(1000);
    }
    */
    
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
        usartDMASend(&au8usartData[0],4);
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
