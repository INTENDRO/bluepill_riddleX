
#ifndef SUP_H
#define SUP_H

#define SUP_MAX_LENGTH 80

uint8_t crc8(uint8_t* u8data_ptr, uint8_t u8length);
uint8_t sup_get_packet(uint8_t* u8packet_ptr, uint8_t* u8rawData_ptr, uint8_t u8length, uint8_t u8dataType);
uint8_t sup_stuff(uint8_t* u8stuffed_ptr, uint8_t* u8data_ptr, uint8_t u8dataLength);
int8_t sup_unstuff(uint8_t* u8data_ptr, uint8_t* u8stuffed_ptr, uint8_t u8stuffedLength);
int8_t sup_unpackage(uint8_t* u8data_ptr, uint8_t* u8dataLength, uint8_t* u8dataType, uint8_t* u8package_ptr, uint8_t u8packageLength);
void sup_send(uint8_t u8dataType, uint8_t* u8dataToSend_ptr, uint8_t u8length);
int8_t sup_receive(uint8_t* u8dataType_ptr, uint8_t* u8data_ptr, uint8_t* u8dataLength_ptr, uint8_t* u8rawData_ptr, uint8_t u8rawDataLength);
uint8_t sup_send_busy(void);

#endif

