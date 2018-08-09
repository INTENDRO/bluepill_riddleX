#include <stm32f10x.h>
#include <string.h>
#include "usart.h"
#include "usart_prot.h"





uint8_t crc8(uint8_t* u8data_ptr, uint8_t u8length)
{
	uint8_t u8crc,i,j,u8temp,u8inByte;

	u8crc = 0;

	for(i=0;i<u8length;i++)
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


void usart_prot_get_packet(uint8_t* u8packet_ptr, uint8_t* u8rawData_ptr, uint8_t u8length, uint8_t u8dataType)
{
    *u8packet_ptr = ((u8dataType&0x03)<<6) | (u8length&0x3F);
    memcpy(u8packet_ptr+1,u8rawData_ptr,u8length);
    *(u8packet_ptr+u8length+1) = crc8(u8packet_ptr,u8length+1);
}


int8_t usart_prot_unpackage(uint8_t* u8data_ptr, uint8_t* u8dataLength, uint8_t* u8dataType, uint8_t* u8package_ptr, uint8_t u8packageLength)
{
    if(crc8(u8package_ptr,u8packageLength) != 0)
    {
        return -1;
    }
    
    if(((u8package_ptr[0]&0x3F)+2) != u8packageLength)
    {
        return -2;
    }
    *u8dataType = (u8package_ptr[0]>>6)&0x03;
    *u8dataLength = u8packageLength-2;
    
    memcpy(u8data_ptr,u8package_ptr+1,u8packageLength-2);
    
    return 0;
}


uint8_t usart_prot_stuff(uint8_t* u8stuffed_ptr, uint8_t* u8data_ptr, uint8_t u8dataLength)
{
    uint16_t u16dataBitCounter,u16destBitCounter,u16dataMaxBit,u16consecutiveOnes;
    
    u8stuffed_ptr[0] = 0x7E; //delimiter
    
    u16dataBitCounter = 0;
    u16destBitCounter = 0;
    u16consecutiveOnes = 0;
    u16dataMaxBit = ((uint16_t)u8dataLength)*8;
    
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
    return u16destBitCounter/8+2; //return stuffed length
}


int8_t usart_prot_unstuff(uint8_t* u8data_ptr, uint8_t* u8stuffed_ptr, uint8_t u8stuffedLength)
{
    uint16_t u16consecutiveOnes, u16dataBitCounter, u16stuffedBitCounter, u16stuffedBitMax, u16stuffBitCount;
    
    if(u8stuffed_ptr[0]==0x7E)
    {
        u8stuffed_ptr++;
        u8stuffedLength--;
        
        if(u8stuffedLength == 0)
        {
            return -2;
        }
    }
    
    if(u8stuffed_ptr[u8stuffedLength-1] == 0x7E)
    {
        u8stuffedLength--;
        
        if(u8stuffedLength == 0)
        {
            return -2;
        }
    }
    
    u16consecutiveOnes = 0;
    u16dataBitCounter = 0;
    u16stuffedBitCounter = 0;
    u16stuffBitCount = 0;
    u16stuffedBitMax = (uint16_t)u8stuffedLength*8;
    
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
    return (int8_t)(u8stuffedLength - (u16stuffBitCount/8 +1));
}


void usart_prot_send(uint8_t* u8dataToSend_ptr,uint8_t u8dataType,uint8_t u8length)
{
    uint8_t u8stuffedLength;
    uint8_t au8temp[80];
    
    usart_prot_get_packet(&au8temp[0],u8dataToSend_ptr,u8length,u8dataType);
    
    u8stuffedLength = usart_prot_stuff(&au8usartSendData[0],&au8temp[0],u8length+2);
    usartDMASend(&au8usartSendData[0],u8stuffedLength);
}


int8_t usart_prot_receive(uint8_t* u8data_ptr, uint8_t* u8dataType, uint8_t* u8dataLength, uint8_t* u8rawData_ptr, uint8_t u8rawDataLength)
{
    int8_t s8unstuffedLength,s8retVal;
    
    uint8_t au8temp[80];
    
    s8unstuffedLength = usart_prot_unstuff(&au8temp[0],u8rawData_ptr,u8rawDataLength);
    if(s8unstuffedLength < 2) //minimum length: 2 (opcode and crc). also: negative numbers are also errors
    {
        return -1;
    }
    
    s8retVal = usart_prot_unpackage(u8data_ptr,u8dataLength,u8dataType,&au8temp[0],(uint8_t)s8unstuffedLength);
    if(s8retVal != 0)
    {
        return -2;
    }
    return 0;
}

