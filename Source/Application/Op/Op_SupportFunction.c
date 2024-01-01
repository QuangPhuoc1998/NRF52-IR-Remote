#include "Op_HeaderFunction.h"

uint32_t power(uint32_t ulXNumber, uint8_t ulYNumber)
{
	uint32_t ulReturnNumber = 1;
    for (uint8_t i = 0; i < ulYNumber; i++)
    {
    	ulReturnNumber *= ulXNumber;
    }

    return ulReturnNumber;
}

uint16_t ConvertID(uint8_t * pData, uint8_t len)
{
    uint16_t uwRetureData = 0;
    for(uint8_t i = 0 ; i < len ; i++)
    {
        uwRetureData += (pData[i]-0x30) * power(10, len-i-1);
    }
    return uwRetureData;
}

void myMemCpy(void *dest, void *src, size_t n)
{
	// Typecast src and dest addresses to (char *)
	char *csrc = (char *)src;
	char *cdest = (char *)dest;

	// Copy contents of src[] to dest[]
	for (int i=0; i<n; i++)
	{
		cdest[i] = csrc[i];
	}
}

int16_t LinearSerachID(IRData_t * pData, uint8_t ubSize, uint16_t uwKey)
{
	for(uint8_t i = 0 ; i < ubSize ; i++)
	{
		if(pData[i].uwID == uwKey)
		{
			return i;
		}
	}
	return -1;
}
