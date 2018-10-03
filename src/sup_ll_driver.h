
#ifndef SUP_LL_DRIVER
#define SUP_LL_DRIVER

#define SUP_BUFFER_SIZE 312
#define SUP_MAX_LENGTH 256
#define SUP_MIN_LENGTH 1


#define WRITE_SYS			0x00
#define WRITE_SYS_REPLY		0x01
#define READ_SYS			0x02
#define READ_SYS_REPLY		0x03
#define WRITE_DATA			0x04
#define WRITE_DATA_REPLY	0x05
#define READ_DATA			0x06
#define READ_DATA_REPLY		0x07
#define SETUP_JOB			0x08
#define SETUP_JOB_REPLY		0x09
#define CHANGE_JOB			0x0A
#define CHANGE_JOB_REPLY	0x0B
#define DELETE_JOB			0x0C
#define DELETE_JOB_REPLY	0x0D
#define GET_JOB				0x0E
#define GET_JOB_REPLY		0x0F
#define START_JOB			0x10
#define START_JOB_REPLY		0x11
#define STOP_JOB			0x12
#define STOP_JOB_REPLY		0x13
#define JOB_DATA			0x14


uint8_t crc8(uint8_t* u8data_ptr, uint16_t u16length);
uint16_t crc16(uint8_t* u8data_ptr, uint16_t u16length);
int8_t sup_get_packet(uint8_t* u8packet_ptr, uint16_t* u16packetLength_ptr, uint8_t* u8rawData_ptr, uint16_t u16rawLength);
int8_t sup_stuff(uint8_t* u8stuffed_ptr, uint16_t* u16stuffedLength_ptr, uint8_t* u8data_ptr, uint16_t u16dataLength);
int8_t sup_unstuff(uint8_t* u8data_ptr, uint16_t* u16dataLength_ptr, uint8_t* u8stuffed_ptr, uint16_t u16stuffedLength);
int8_t sup_unpackage(uint8_t* u8data_ptr, uint16_t* u16dataLength_ptr, uint8_t* u8package_ptr, uint16_t u16packageLength);
int8_t sup_send(uint8_t* u8dataToSend_ptr, uint16_t u16length);
int8_t sup_receive(uint8_t* u8data_ptr, uint16_t* u16dataLength_ptr, uint8_t* u8rawData_ptr, uint16_t u16rawDataLength);
uint8_t sup_send_busy(void);

#endif

