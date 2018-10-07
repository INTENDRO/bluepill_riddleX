
#ifndef SUP_LL_DRIVER
#define SUP_LL_DRIVER

#include "ringbuffer.h"

#define SUP_LL_BUFFER_SIZE 312
#define SUP_LL_MAX_LENGTH 256
#define SUP_LL_MIN_LENGTH 1


//#define SUP_LL_CRC8


uint8_t sup_ll_init(uint32_t u32baudrate);
RingBuff_t* sup_ll_get_ringbuffer_ptr(void);
uint8_t sup_ll_send(uint8_t* u8dataToSend_ptr, uint16_t u16length);
uint8_t sup_ll_receive(uint8_t* u8data_ptr, uint16_t* u16dataLength_ptr, uint8_t* u8rawData_ptr, uint16_t u16rawDataLength);
uint8_t sup_ll_send_isbusy(void);

#endif

