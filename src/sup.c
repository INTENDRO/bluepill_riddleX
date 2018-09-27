#include <stm32f10x.h>
#include <string.h>
#include "usart.h"
#include "sup.h"


static uint8_t au8sendBuffer[SUP_BUFFER_SIZE];


uint8_t crc8(uint8_t* u8data_ptr, uint16_t u16length)
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


int8_t sup_get_packet(uint8_t* u8packet_ptr, uint16_t* u16packetLength_ptr, uint8_t* u8rawData_ptr, uint16_t u16rawLength)
{
    if((u16rawLength<SUP_MIN_LENGTH) || (u16rawLength > SUP_MAX_LENGTH))
    {
    	return -1;
    }

	*u8packet_ptr = u16rawLength-1;
    memcpy(u8packet_ptr+1,u8rawData_ptr,u16rawLength);
    *(u8packet_ptr+u16rawLength+1) = crc8(u8packet_ptr,u16rawLength+1);
    *u16packetLength_ptr = u16rawLength + 2;
    return 0;
}


int8_t sup_unpackage(uint8_t* u8data_ptr, uint16_t* u16dataLength_ptr, uint8_t* u8package_ptr, uint16_t u16packageLength)
{
    if(crc8(u8package_ptr,u16packageLength) != 0)
    {
        return -1;
    }
    
    if((((uint16_t)u8package_ptr[0])+3) != u16packageLength)
    {
        return -2;
    }

    *u16dataLength_ptr = u16packageLength-2;
    memcpy(u8data_ptr,u8package_ptr+1,u16packageLength-2);
    
    return 0;
}


int8_t sup_stuff(uint8_t* u8stuffed_ptr, uint16_t* u16stuffedLength_ptr, uint8_t* u8data_ptr, uint16_t u16dataLength)
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
    return 0;
}


int8_t sup_unstuff(uint8_t* u8data_ptr, uint16_t* u16dataLength_ptr, uint8_t* u8stuffed_ptr, uint16_t u16stuffedLength)
{
    uint16_t u16consecutiveOnes, u16dataBitCounter, u16stuffedBitCounter, u16stuffedBitMax, u16stuffBitCount;
    
    if(u8stuffed_ptr[0]==0x7E)
    {
        u8stuffed_ptr++;
        u16stuffedLength--;
        
        if(u16stuffedLength == 0)
        {
            return -2;
        }
    }
    
    if(u8stuffed_ptr[u16stuffedLength-1] == 0x7E)
    {
    	u16stuffedLength--;
        
        if(u16stuffedLength == 0)
        {
            return -2;
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
                    return -1;
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
    return 0;
}



int8_t sup_send(uint8_t* u8dataToSend_ptr, uint16_t u16length)
{
    uint16_t u16stuffedLength,u16packetLength;
    int8_t s8retVal;
    uint8_t au8packet[SUP_BUFFER_SIZE];
    
    s8retVal = sup_get_packet(au8packet,&u16packetLength,u8dataToSend_ptr,u16length);
    if(s8retVal)
    {
    	return -1;
    }
    s8retVal = sup_stuff(au8sendBuffer,&u16stuffedLength,au8packet,u16packetLength);
    if(s8retVal)
	{
		return -2;
	}
    usartDMASend(au8sendBuffer,u16stuffedLength);
    return 0;
}


int8_t sup_receive(uint8_t* u8data_ptr, uint16_t* u16dataLength_ptr, uint8_t* u8rawData_ptr, uint16_t u16rawDataLength)
{
    int8_t s8retVal;
    uint16_t u16packetLength;
    
    uint8_t au8packet[SUP_BUFFER_SIZE];
    
    s8retVal = sup_unstuff(au8packet,&u16packetLength,u8rawData_ptr,u16rawDataLength);
    if(s8retVal)
    {
    	return -1;
    }
    if((u16packetLength < SUP_MIN_LENGTH+2) || (u16packetLength > SUP_MAX_LENGTH+2))
    {
        return -2;
    }
    
    s8retVal = sup_unpackage(u8data_ptr,u16dataLength_ptr,au8packet,u16packetLength);
    if(s8retVal)
    {
        return -3;
    }
    return 0;
}

uint8_t sup_send_busy(void)
{
    return usartBusy();
}