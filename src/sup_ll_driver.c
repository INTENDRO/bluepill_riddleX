#include <stm32f10x.h>
#include <string.h>
#include "utils.h"
#include "sup_ll_driver.h"
#include "sup_error.h"
#include "usart.h"
#include "ringbuffer.h"


static uint8_t au8sendBuffer[SUP_LL_BUFFER_SIZE];

#ifdef SUP_LL_CRC8
static uint8_t sup_ll_crc8(uint8_t* u8data_ptr, uint16_t u16length);
#else
static uint16_t sup_ll_crc16(uint8_t* u8data_ptr, uint16_t u16length);
#endif

static uint8_t sup_ll_package(uint8_t* u8packet_ptr, uint16_t* u16packetLength_ptr, uint8_t* u8rawData_ptr, uint16_t u16rawLength);
static uint8_t sup_ll_stuff(uint8_t* u8stuffed_ptr, uint16_t* u16stuffedLength_ptr, uint8_t* u8data_ptr, uint16_t u16dataLength);
static uint8_t sup_ll_unstuff(uint8_t* u8data_ptr, uint16_t* u16dataLength_ptr, uint8_t* u8stuffed_ptr, uint16_t u16stuffedLength);
static uint8_t sup_ll_unpackage(uint8_t* u8data_ptr, uint16_t* u16dataLength_ptr, uint8_t* u8package_ptr, uint16_t u16packageLength);


#ifdef SUP_LL_CRC8
static uint8_t sup_ll_crc8(uint8_t* u8data_ptr, uint16_t u16length)
{
	uint8_t u8crc,j,u8temp,u8inByte;
	uint16_t i;

	u8crc = 0;

	for(i=0;i<u16length;i++)
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

#else

static uint16_t sup_ll_crc16(uint8_t* u8data_ptr, uint16_t u16length)
{
	uint8_t j;
	uint16_t i,u16crc,u16temp,u16inByte;

	u16crc = 0xFFFF;

	for(i=0;i<u16length;i++)
	{
		u16inByte = (uint16_t)(u8data_ptr[i]);
		for(j=0;j<8;j++)
		{
			u16temp = (u16crc^u16inByte)&0x0001;
			u16crc >>= 1;
			if(u16temp)
			{
				u16crc ^= 0x8408;
			}
			u16inByte>>=1;
		}
	}

	return u16crc;
}
#endif


static uint8_t sup_ll_package(uint8_t* u8packet_ptr, uint16_t* u16packetLength_ptr, uint8_t* u8rawData_ptr, uint16_t u16rawLength)
{
#ifdef SUP_LL_CRC8
	uint8_t u8crc;
#else
	uint16_t u16crc;
#endif

	if((u16rawLength<SUP_LL_MIN_LENGTH) || (u16rawLength > SUP_LL_MAX_LENGTH))
    {
    	return SUP_ERROR_DATA_LENGTH;
    }

	*u8packet_ptr = u16rawLength-1;
    memcpy(u8packet_ptr+1,u8rawData_ptr,u16rawLength);

#ifdef SUP_LL_CRC8
    u8crc = sup_ll_crc8(u8packet_ptr,u16rawLength+1);
	*(u8packet_ptr+u16rawLength+1) = u8crc;
    *u16packetLength_ptr = u16rawLength + 2;
#else
    u16crc = sup_ll_crc16(u8packet_ptr,u16rawLength+1);
	*(u8packet_ptr+u16rawLength+1) = (uint8_t)u16crc;
	*(u8packet_ptr+u16rawLength+2) = (uint8_t)(u16crc>>8);
	*u16packetLength_ptr = u16rawLength + 3;
#endif
    return SUP_SUCCESS;
}


static uint8_t sup_ll_unpackage(uint8_t* u8data_ptr, uint16_t* u16dataLength_ptr, uint8_t* u8package_ptr, uint16_t u16packageLength)
{
#ifdef SUP_LL_CRC8
	if(sup_ll_crc8(u8package_ptr,u16packageLength))
    {
        return SUP_ERROR_CRC;
    }

	if((((uint16_t)u8package_ptr[0])+3) != u16packageLength)
	{
		return SUP_ERROR_DATA_LENGTH;
	}

	*u16dataLength_ptr = u16packageLength-2;
	memcpy(u8data_ptr,u8package_ptr+1,u16packageLength-2);
#else
	if(sup_ll_crc16(u8package_ptr,u16packageLength))
	{
		return SUP_ERROR_CRC;
	}

	if((((uint16_t)u8package_ptr[0])+4) != u16packageLength)
	{
		return SUP_ERROR_DATA_LENGTH;
	}

	*u16dataLength_ptr = u16packageLength-3;
	memcpy(u8data_ptr,u8package_ptr+1,u16packageLength-3);
#endif
    
    return SUP_SUCCESS;
}


static uint8_t sup_ll_stuff(uint8_t* u8stuffed_ptr, uint16_t* u16stuffedLength_ptr, uint8_t* u8data_ptr, uint16_t u16dataLength)
{
    uint16_t u16dataBitCounter,u16destBitCounter,u16dataMaxBit,u16consecutiveOnes;
    
    u8stuffed_ptr[0] = 0x7E; //delimiter
    
    u16dataBitCounter = 0;
    u16destBitCounter = 0;
    u16consecutiveOnes = 0;
    u16dataMaxBit = (u16dataLength)*8;
    
    while(u16dataBitCounter < u16dataMaxBit)
    {
        if(u8data_ptr[u16dataBitCounter/8] & (0x80>>(u16dataBitCounter%8))) //data is a 1
        {
            u8stuffed_ptr[(u16destBitCounter/8)+1] |= (0x80 >> (u16destBitCounter%8)); //set dest bit
            u16destBitCounter++;
            
            u16consecutiveOnes++;
            if(u16consecutiveOnes==5)
            {
                u16consecutiveOnes = 0;
                u8stuffed_ptr[(u16destBitCounter/8)+1] &= ~(0x80 >> (u16destBitCounter%8)); //stuff bit
                u16destBitCounter++;
            }
        }
        else //data is a 0
        {
            u16consecutiveOnes = 0;
            u8stuffed_ptr[(u16destBitCounter/8)+1] &= ~(0x80 >> (u16destBitCounter%8)); //clear dest bit
            u16destBitCounter++;
        }
        u16dataBitCounter++;
        
    }
    
    while((u16destBitCounter%8)!=0)
    {
        u8stuffed_ptr[(u16destBitCounter/8)+1] &= ~(0x80 >> (u16destBitCounter%8)); //clear dest bit
        u16destBitCounter++;
    }
    
    u8stuffed_ptr[u16destBitCounter/8+1] = 0x7E; //delimiter
    *u16stuffedLength_ptr = u16destBitCounter/8+2; //stuffed length
    return SUP_SUCCESS;
}


static uint8_t sup_ll_unstuff(uint8_t* u8data_ptr, uint16_t* u16dataLength_ptr, uint8_t* u8stuffed_ptr, uint16_t u16stuffedLength)
{
    uint16_t u16consecutiveOnes, u16dataBitCounter, u16stuffedBitCounter, u16stuffedBitMax, u16stuffBitCount;
    
    if(u8stuffed_ptr[0]==0x7E)
    {
        u8stuffed_ptr++;
        u16stuffedLength--;
        
        if(u16stuffedLength == 0)
        {
            return SUP_ERROR_DATA_LENGTH;
        }
    }
    
    if(u8stuffed_ptr[u16stuffedLength-1] == 0x7E)
    {
    	u16stuffedLength--;
        
        if(u16stuffedLength == 0)
        {
            return SUP_ERROR_DATA_LENGTH;
        }
    }
    
    u16consecutiveOnes = 0;
    u16dataBitCounter = 0;
    u16stuffedBitCounter = 0;
    u16stuffBitCount = 0;
    u16stuffedBitMax = u16stuffedLength*8;
    
    while(u16stuffedBitCounter<u16stuffedBitMax)
    {
        if(u8stuffed_ptr[u16stuffedBitCounter/8] & (0x80 >> (u16stuffedBitCounter%8))) //stuffed bitstream is 1
        {
            u8data_ptr[u16dataBitCounter/8] |= (0x80 >> (u16dataBitCounter%8)); //set data bit to 1
            u16dataBitCounter++;
            u16stuffedBitCounter++;
            
            u16consecutiveOnes++;
            if(u16consecutiveOnes==5)
            {
                u16consecutiveOnes = 0;
                //should be a stuff bit -> 0
                if(u8stuffed_ptr[u16stuffedBitCounter/8] & (0x80 >> (u16stuffedBitCounter%8)))
                {
                    //error
                    return SUP_ERROR_STUFFING;
                }
                u16stuffedBitCounter++;
                u16stuffBitCount++;
            }
        }
        else //stuffed bitstream is 0
        {
            u16consecutiveOnes = 0;
            u8data_ptr[u16dataBitCounter/8] &= ~(0x80 >> (u16dataBitCounter%8)); //set data bit to 0
            u16dataBitCounter++;
            u16stuffedBitCounter++;
        }
    }
    *u16dataLength_ptr = u16stuffedLength - ((u16stuffBitCount+7)/8);
    return SUP_SUCCESS;
}

uint8_t sup_ll_init(uint32_t u32baudrate)
{
	usartInit(u32baudrate);
	wait_1ms(1);
	usartClearFlagsAndBuffer();
	return SUP_SUCCESS;
}

RingBuff_t* sup_ll_get_ringbuffer_ptr(void)
{
	return usartGetRingBuffPointer();
}

uint8_t sup_ll_send(uint8_t* u8dataToSend_ptr, uint16_t u16length)
{
    uint16_t u16stuffedLength,u16packetLength;
    uint8_t u8retVal;
    uint8_t au8packet[SUP_LL_BUFFER_SIZE];
    
    u8retVal = sup_ll_package(au8packet,&u16packetLength,u8dataToSend_ptr,u16length);
    if(u8retVal)
    {
    	return SUP_ERROR_PACKET;
    }
    u8retVal = sup_ll_stuff(au8sendBuffer,&u16stuffedLength,au8packet,u16packetLength);
    if(u8retVal)
	{
		return SUP_ERROR_STUFFING;
	}
    usartDMASend(au8sendBuffer,u16stuffedLength);
    return SUP_SUCCESS;
}


uint8_t sup_ll_receive(uint8_t* u8data_ptr, uint16_t* u16dataLength_ptr, uint8_t* u8rawData_ptr, uint16_t u16rawDataLength)
{
    uint8_t u8retVal;
    uint16_t u16packetLength;
    
    uint8_t au8packet[SUP_LL_BUFFER_SIZE];
    
    u8retVal = sup_ll_unstuff(au8packet,&u16packetLength,u8rawData_ptr,u16rawDataLength);
    if(u8retVal)
    {
    	return SUP_ERROR_STUFFING;
    }
    if((u16packetLength < SUP_LL_MIN_LENGTH+2) || (u16packetLength > SUP_LL_MAX_LENGTH+2))
    {
        return SUP_ERROR_DATA_LENGTH;
    }
    
    u8retVal = sup_ll_unpackage(u8data_ptr,u16dataLength_ptr,au8packet,u16packetLength);
    if(u8retVal)
    {
        return SUP_ERROR_PACKET;
    }
    return SUP_SUCCESS;
}

uint8_t sup_ll_send_isbusy(void)
{
    return usartBusy();
}
