
#ifndef USART_H
#define USART_H

void usartSendByte(uint8_t u8data);
void usartInit(uint32_t u32baudrate);
void usartDMASend(uint8_t* u32data_ptr, uint16_t u16length);
void usartClearFlagsAndBuffer(void);
uint8_t usartDataAvailable(void);
uint8_t usartGetByte(void);
void USART1_IRQHandler(void);


#endif
