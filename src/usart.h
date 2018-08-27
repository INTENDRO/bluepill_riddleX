
#ifndef USART_H
#define USART_H

#include "ringbuffer.h"

#define USART_MAX_DATA 80


enum error_state_t
{
    NONE,
    DATA_OVERFLOW,
    USART_ERROR
};


void usartSendByte(uint8_t u8data);
void usartInit(uint32_t u32baudrate);
RingBuff_t* usartGetRingBuffPointer(void);
void usartDMASend(uint8_t* u32data_ptr, uint16_t u16length);
uint8_t usartBusy(void);
void usartClearFlagsAndBuffer(void);
uint8_t usartDataAvailable(void);
uint8_t usartGetByte(void);
void USART1_IRQHandler(void);


#endif
