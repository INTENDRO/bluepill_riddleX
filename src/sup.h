
#ifndef SUP_H
#define SUP_H

#define SUP_BUFFER_SIZE 312
#define SUP_MAX_LENGTH 256
#define SUP_MIN_LENGTH 1

uint8_t crc8(uint8_t* u8data_ptr, uint16_t u16length);
int8_t sup_get_packet(uint8_t* u8packet_ptr, uint16_t* u16packetLength_ptr, uint8_t* u8rawData_ptr, uint16_t u16rawLength);
int8_t sup_stuff(uint8_t* u8stuffed_ptr, uint16_t* u16stuffedLength_ptr, uint8_t* u8data_ptr, uint16_t u16dataLength);
int8_t sup_unstuff(uint8_t* u8data_ptr, uint16_t* u16dataLength_ptr, uint8_t* u8stuffed_ptr, uint16_t u16stuffedLength);
int8_t sup_unpackage(uint8_t* u8data_ptr, uint16_t* u16dataLength_ptr, uint8_t* u8package_ptr, uint16_t u16packageLength);
int8_t sup_send(uint8_t* u8dataToSend_ptr, uint16_t u16length);
int8_t sup_receive(uint8_t* u8data_ptr, uint16_t* u16dataLength_ptr, uint8_t* u8rawData_ptr, uint16_t u16rawDataLength);
uint8_t sup_send_busy(void);

#endif

