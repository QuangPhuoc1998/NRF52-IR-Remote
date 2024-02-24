#include "Op_HeaderFunction.h"
#include "main.h"

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
	uint16_t ulResult = 0;
	uint8_t ubCharToHex = 0;

	if(len > 2)
	{
		return false;
	}

	if(pData[1] == 0x20)
	{
		len = 1;
	}

	for(uint8_t i = 0 ; i < len ; i++)
	{
		if(ConvertCharToHex(pData[i], &ubCharToHex) == false)
		{
			return 0;
		}
		ulResult |= ubCharToHex << ((len-i-1) * 4);
	}
	return ulResult;
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

void myMemSet(void *dest, uint8_t ubData, size_t n)
{
	char *cdest = (char *)dest;
	for (int i=0; i<n; i++)
	{
		cdest[i] = ubData;
	}
}

int16_t LinearSerachID(IRData_t * pData, uint8_t ubSize, uint16_t uwKey)
{
	for(uint8_t i = 0 ; i < ubSize ; i++)
	{
		if(pData[i].uwID == uwKey && pData[i].ulStatus == IR_CODE_ENABLE)
		{
			return i;
		}
	}
	return -1;
}

uint32_t StrToInt(uint8_t * str, uint8_t ubLen)
{
    uint32_t ulRet = 0;
    for(uint8_t i = 0; i < ubLen; ++i)
    {
        ulRet = ulRet * 10 + (str[i] - '0');
    }
    return ulRet;
}

bool CheckStringIsNumber(uint8_t * pData, uint8_t len)
{
    for(uint8_t i = 0 ; i < len ; i++)
    {
        if(pData[i] < '0' || pData[i] > '9')
        {
            return false;
        }
    }
    return true;
}

bool ConvertCharToHex(uint8_t ubData, uint8_t * ubResult)
{
    if(ubData >= '0' &&  ubData <= '9')
    {
        *ubResult = ubData - 48;
    }
    else if (ubData >= 'a' &&  ubData <= 'f')
    {
        *ubResult = ubData - 87;
    }
    else if (ubData >= 'A' &&  ubData <= 'F')
    {
        *ubResult = ubData - 55;
    }
    else
    {
        return false;
    }
    return true;
}

bool ConvertStringToHex(uint8_t * pData, uint8_t len, uint8_t * ubResult)
{
    uint8_t ubCharToHex = 0;

    if(len > 2)
    {
        return false;
    }

    for(uint8_t i = 0 ; i < len ; i++)
    {
        if(ConvertCharToHex(pData[i], &ubCharToHex) == false)
        {
            return false;
        }
        *ubResult |= ubCharToHex << ((len-i-1) * 4);
    }
    return true;
}

bool ConvertStringToHex32Bit(uint8_t * pData, uint8_t len, uint32_t * ulResult)
{
    uint8_t ubCharToHex = 0;

    for(uint8_t i = 0 ; i < len ; i++)
    {
        if(ConvertCharToHex(pData[i], &ubCharToHex) == false)
        {
            return false;
        }
        *ulResult |= ubCharToHex << ((len-i-1) * 4);
    }
    return true;
}

uint64_t ConvertStringToTimeStamp(uint8_t * pData, uint8_t len)
{
    uint64_t ullRetureData = 0;
    for(uint8_t i = 0 ; i < len ; i++)
    {
        if(pData[i] == 0x20)
        {
            ullRetureData /= power(10, len-i);
        }
        else
        {
            ullRetureData += (pData[i]-0x30) * power(10, len-i-1);
        }
    }
    return ullRetureData;
}

uint8_t is_delim(char c, char *delim)
{
    while(*delim != '\0')
    {
        if(c == *delim)
            return 1;
        delim++;
    }
    return 0;
}
char * my_strtok(char *srcString, char *delim)
{
    static char *backup_string; // start of the next search
    if(!srcString)
    {
        srcString = backup_string;
    }
    if(!srcString)
    {
        // user is bad user
        return NULL;
    }
    // handle beginning of the string containing delims
    while(1)
    {
        if(is_delim(*srcString, delim))
        {
            srcString++;
            continue;
        }
        if(*srcString == '\0')
        {
            // we've reached the end of the string
            return NULL;
        }
        break;
    }
    char *ret = srcString;
    while(1)
    {
        if(*srcString == '\0')
        {
            /*end of the input string and
            next exec will return NULL*/
            backup_string = srcString;
            return ret;
        }
        if(is_delim(*srcString, delim))
        {
            *srcString = '\0';
            backup_string = srcString + 1;
            return ret;
        }
        srcString++;
    }
}

uint16_t ConvertArrayToNumber(uint8_t * pData, uint8_t ubLen)
{
    uint16_t ulResult = 0;
    for(uint8_t i = 0 ; i < ubLen ; i++)
    {
        ulResult += pData[i] << (8 * (ubLen-1-i));
    }
    return ulResult;
}

uint8_t ConvertSensitivityToLevel(uint8_t ubSens)
{
	uint8_t Ret = CLEAR;
	ubSens = CONVERT_SENSITIVITY_TO_VALUE(ubSens * 2);
	if(ubSens <= MOTION_SENSOR_SENSITIVITY_LEVEL_0)
	{
		if(ubSens >= MOTION_SENSOR_SENSITIVITY_LEVEL_NONE)
		{
			Ret = SENSITIVITY_LEVEL_0;
		}
	}
	else if(ubSens <= MOTION_SENSOR_SENSITIVITY_LEVEL_1)
	{
		Ret = SENSITIVITY_LEVEL_1;
	}
	else if(ubSens <= MOTION_SENSOR_SENSITIVITY_LEVEL_2)
	{
		Ret = SENSITIVITY_LEVEL_2;
	}
	else if(ubSens <= MOTION_SENSOR_SENSITIVITY_LEVEL_3)
	{
		Ret = SENSITIVITY_LEVEL_3;
	}
	else if(ubSens <= MOTION_SENSOR_SENSITIVITY_LEVEL_4)
	{
		Ret = SENSITIVITY_LEVEL_4;
	}
	return Ret;
}
