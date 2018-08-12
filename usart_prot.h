
#ifndef USART_PROT_H
#define USART_PROT_H

uint8_t crc8(uint8_t* u8data_ptr, uint8_t u8length);
uint8_t usart_prot_get_packet(uint8_t* u8packet_ptr, uint8_t* u8rawData_ptr, uint8_t u8length, uint8_t u8dataType);
uint8_t usart_prot_stuff(uint8_t* u8stuffed_ptr, uint8_t* u8data_ptr, uint8_t u8dataLength);
int8_t usart_prot_unstuff(uint8_t* u8data_ptr, uint8_t* u8stuffed_ptr, uint8_t u8stuffedLength);
int8_t usart_prot_unpackage(uint8_t* u8data_ptr, uint8_t* u8dataLength, uint8_t* u8dataType, uint8_t* u8package_ptr, uint8_t u8packageLength);
void usart_prot_send(uint8_t* u8dataToSend_ptr,uint8_t u8dataType,uint8_t u8length);
int8_t usart_prot_receive(uint8_t* u8data_ptr, uint8_t* u8dataType, uint8_t* u8dataLength, uint8_t* u8rawData_ptr, uint8_t u8rawDataLength);


#endif

