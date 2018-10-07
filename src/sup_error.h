/*
 * sup_error.h
 *
 *  Created on: 7 Oct 2018
 *      Author: Mario
 */

#ifndef SUP_ERROR_H_
#define SUP_ERROR_H_


#define SUP_ERROR_BASE_NUM 	(0)

#define SUP_SUCCESS 				(SUP_ERROR_BASE_NUM + 0)
#define SUP_ERROR_DATA_LENGTH		(SUP_ERROR_BASE_NUM + 1)
#define SUP_ERROR_CRC				(SUP_ERROR_BASE_NUM + 2)
#define SUP_ERROR_STUFFING			(SUP_ERROR_BASE_NUM + 3)
#define SUP_ERROR_PACKET			(SUP_ERROR_BASE_NUM + 4)
#define SUP_ERROR_NO_MODULE			(SUP_ERROR_BASE_NUM + 5)
#define SUP_ERROR_NO_REG			(SUP_ERROR_BASE_NUM + 6)
#define SUP_ERROR_USART_TIMEOUT		(SUP_ERROR_BASE_NUM + 7)






#endif /* SUP_ERROR_H_ */
