
#ifndef SUP_LL_DRIVER
#define SUP_LL_DRIVER

#define SUP_LL_BUFFER_SIZE 312
#define SUP_LL_MAX_LENGTH 256
#define SUP_LL_MIN_LENGTH 1



int8_t sup_ll_send(uint8_t* u8dataToSend_ptr, uint16_t u16length);
int8_t sup_ll_receive(uint8_t* u8data_ptr, uint16_t* u16dataLength_ptr, uint8_t* u8rawData_ptr, uint16_t u16rawDataLength);
uint8_t sup_ll_send_isbusy(void);

#endif

