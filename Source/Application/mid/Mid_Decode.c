#include "main.h"

#define SHOW_DEBUG_DECODE

void Mid_DecodeInit(void)
{
    for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
    {
        g_atIRDataTrigger[i].ubMode = NONE;
        g_atIRTimeOut[i].ubStatus = TIMEOUT_DISABLE;
    }
}

void Mid_DecodeCommand(uint8_t ubCommandIndex, uint8_t * pData, uint8_t len)
{
    switch (ubCommandIndex )
    {
    case CUS_UUID_RTC_SET_INDEX:
    {
    	app_sched_event_put(pData, len, Mid_DecodeRTCSet);
        break;
    }
    case CUS_UUID_MOT_SENS_INDEX:
    {
        break;
    }
    case CUS_UUID_MOT_TOUT_INDEX:
    {
    	app_sched_event_put(pData, len, Mid_DecodeTimeOut); // requirement 1.5
        break;
    }
    case CUS_UUID_SCHEDULE_INDEX:
    {
    	app_sched_event_put(pData, len, Mid_DecodeTimeBasedScheduling); // requirement 1.3
        break;
    }
    case CUS_UUID_BATT_VOL_INDEX:
    {
        break;
    }
    case CUS_UUID_IR_LEARN_INDEX:
    {
        break;
    }
    case CUS_UUID_IR_EMMIT_INDEX:
    {
        break;
    }

    default:
        break;
    }
}

void Mid_DecodeTimeBasedScheduling(void *pData, uint16_t len)
{
    uint8_t ubCommand = 0;
    IrDataTrigger_t tIRDataTrigger = {0};
    uint8_t ubNumOfCompenent =  0;
    if(Mid_DecodeTimeBasedSchedulingDecode(&ubCommand, &tIRDataTrigger, &ubNumOfCompenent, pData, len) == true)
    {
        if(ubNumOfCompenent == 6)
        {
            tIRDataTrigger.ubMode = WINDOW_TRIGGER;
        }
        else if(ubNumOfCompenent == 5)
        {
            tIRDataTrigger.ubMode = ONE_TRIGGER;
        }
        else if(ubNumOfCompenent == 3)
        {
            tIRDataTrigger.ubMode = MOTION__TRIGGER;
        }
        else
        {
            return;
        }

        if(ubCommand == CREATE_COMMAND)
        {
            Mid_InsertNewIRTrigger(&tIRDataTrigger);
        }
        else if(ubCommand == DELETE_COMMAND)
        {
            Mid_RemoveIRTrigger(tIRDataTrigger.uwTrigID);
        }
    }
    else
    {

    }
}

void Mid_DecodeTimeOut(void * pData, uint16_t len)
{
    uint8_t ubCommand = 0;
    IrDataTimeout_t tIRDataTimeout = {0};
    uint8_t ubNumOfCompenent =  0;
    uint8_t ubReturnValue = Mid_DecodeTimeOutDecode(&ubCommand, &tIRDataTimeout, &ubNumOfCompenent, pData, len);
    if(ubReturnValue == NO_ERROR)
    {
        if(ubCommand == CREATE_COMMAND)
        {
            tIRDataTimeout.ubStatus = TIMEOUT_ENABLE;
            Mid_InsertNewIRTimeOut(&tIRDataTimeout);
#ifdef SHOW_DEBUG_DECODE
            NRF_LOG_INFO("Command: %s", (ubCommand == CREATE_COMMAND) ? "Create" : "Delete");
            NRF_LOG_INFO("Status: %d", tIRDataTimeout.ubStatus);
            NRF_LOG_INFO("TrigID: %d", tIRDataTimeout.uwTrigID);
            NRF_LOG_INFO("IrCode: %d", tIRDataTimeout.uwIrCode);
            NRF_LOG_INFO("Days: %d", tIRDataTimeout.ubDays);
            NRF_LOG_INFO("Timeout: %d", tIRDataTimeout.ulTimeout);
#endif
        }
        else if(ubCommand == DELETE_COMMAND)
        {
            tIRDataTimeout.ubStatus = TIMEOUT_DISABLE;
            Mid_RemoveIRTimeOut(tIRDataTimeout.uwTrigID);
#ifdef SHOW_DEBUG_DECODE
            NRF_LOG_INFO("Command: %s", (ubCommand == CREATE_COMMAND) ? "Create" : "Delete");
            NRF_LOG_INFO("TrigID: %d", tIRDataTimeout.uwTrigID);
#endif
        }
    }
    else
    {
#ifdef SHOW_DEBUG_DECODE
    	NRF_LOG_INFO("Error code: %d", ubReturnValue);
#endif
    }
}

bool Mid_DecodeTimeBasedSchedulingDecode(uint8_t * ubCommand, IrDataTrigger_t * tIRDataTrigger,  uint8_t * ubNumOfCompenent, uint8_t * pData, uint8_t len)
{
	char * token;
	uint8_t TempData[16];
	strncpy((char * )TempData, (char *)pData, len);
	token = my_strtok((char *)TempData, "-");
    while(token != NULL && *ubNumOfCompenent < 6)
    {
        switch (*ubNumOfCompenent)
        {
            case COMMAND_INDEX:
            {
                if(strlen(token) == 1)
                {
                    if(token[0] == CREATE_COMMAND || token[0] == DELETE_COMMAND)
                    {
                        *ubCommand = token[0];
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
                break;
            }
            case TRIGGER_ID_INDEX:
            {
                uint8_t ubTemp = 0;
                if(ConvertStringToHex((uint8_t *)token, strlen(token), &ubTemp) == true)
                {
                    tIRDataTrigger->uwTrigID = ubTemp;
                }
                else
                {
                    return false;
                }
                break;
            }
            case IR_CODE_INDEX:
            {
                uint8_t ubTemp = 0;
                if(ConvertStringToHex((uint8_t *)token, strlen(token), &ubTemp) == true)
                {
                    tIRDataTrigger->uwIrCode = ubTemp;
                }
                else
                {
                    return false;
                }
                break;
            }
            case DAYS_INDEX:
            {
                uint8_t ubTemp = 0;
                if(ConvertStringToHex((uint8_t *)token, strlen(token), &ubTemp) == true)
                {
                    tIRDataTrigger->ubDays = ubTemp;
                }
                else
                {
                    return false;
                }
                break;
            }
            case TIME1_INDEX:
            {
                if(strlen(token) == 4)
                {
                    if(CheckStringIsNumber((uint8_t *)token, strlen(token)) == true)
                    {
                        tIRDataTrigger->tTime1.ubHour = StrToInt((uint8_t *)(&token[0]), 2);
                        tIRDataTrigger->tTime1.ubMin = StrToInt((uint8_t *)(&token[2]), 2);
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
                break;
            }
            case TIME2_INDEX:
            {
                if(strlen(token) == 4)
                {
                    if(CheckStringIsNumber((uint8_t *)token, strlen(token)) == true)
                    {
                        tIRDataTrigger->tTime2.ubHour = StrToInt((uint8_t *)(&token[0]), 2);
                        tIRDataTrigger->tTime2.ubMin = StrToInt((uint8_t *)(&token[2]), 2);
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
                break;
            }
            default:
                break;
        }
        token = my_strtok(NULL, "-");
        (*ubNumOfCompenent)++;
    }
    return true;
}

int16_t Mid_FindIRTrigger(uint16_t uwTrigID)
{
    for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
    {
        if(g_atIRDataTrigger[i].uwTrigID == uwTrigID)
        {
            return i;
        }
    }
    return -1;
}

bool Mid_InsertNewIRTrigger(IrDataTrigger_t * tIRDataTrigger)
{
    for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
    {
        if(g_atIRDataTrigger[i].uwTrigID == tIRDataTrigger->uwTrigID && g_atIRDataTrigger[i].ubMode != NONE)
        {
            return false;
        }
    }
    for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
    {
        if(g_atIRDataTrigger[i].ubMode == NONE)
        {
            myMemCpy(&g_atIRDataTrigger[i], tIRDataTrigger, sizeof(IrDataTrigger_t));
            g_ubIRTriggerCount++;
            return true;
        }
    }
    return false;
}

bool Mid_RemoveIRTrigger(uint16_t uwTrigID)
{
    for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
    {
        if(g_atIRDataTrigger[i].uwTrigID == uwTrigID)
        {
            g_atIRDataTrigger[i].ubMode = NONE;
            g_ubIRTriggerCount--;
            return true;
        }
    }
    return false;
}

uint8_t Mid_DecodeTimeOutDecode(uint8_t * ubCommand, IrDataTimeout_t * tIRDataTimeout,  uint8_t * ubNumOfCompenent, uint8_t * pData, uint8_t len)
{
	char * token;
	uint8_t TempData[16];
	myMemCpy(TempData, pData, len);
	TempData[len] = '\0';
	token = my_strtok((char *)TempData, "-");
    while(token != NULL && *ubNumOfCompenent < 6)
    {
        switch (*ubNumOfCompenent)
        {
            case TIMEOUT_COMMAND_INDEX:
            {
                if(strlen(token) == 1)
                {
                    if(token[0] == CREATE_COMMAND || token[0] == DELETE_COMMAND)
                    {
                        *ubCommand = token[0];
                    }
                    else
                    {
                        return TIMEOUT_COMMAND_INVALID;
                    }
                }
                else
                {
                    return TIMEOUT_COMMAND_INVALID;
                }
                break;
            }
            case TIMEOUT_TRIGGER_ID_INDEX:
            {
                uint8_t ubTemp = 0;
                if(ConvertStringToHex((uint8_t *)token, strlen(token), &ubTemp) == true)
                {
                    tIRDataTimeout->uwTrigID = ubTemp;
                }
                else
                {
                    return TIMEOUT_TRIGGER_ID_INVALID;
                }
                break;
            }
            case TIMEOUT_IR_CODE_INDEX:
            {
                uint8_t ubTemp = 0;
                if(ConvertStringToHex((uint8_t *)token, strlen(token), &ubTemp) == true)
                {
                    tIRDataTimeout->uwIrCode = ubTemp;
                }
                else
                {
                    return TIMEOUT_TRIGGER_IR_INVALID;
                }
                break;
            }
            case TIMEOUT_DAYS:
            {
                uint8_t ubTemp = 0;
                if(ConvertStringToHex((uint8_t *)token, strlen(token), &ubTemp) == true)
                {
                    tIRDataTimeout->ubDays = ubTemp;
                }
                else
                {
                    return TIMEOUT_DAYS_INVALID;
                }
                break;
            }
            case TIMEOUT_INDEX:
            {
                uint32_t ulTemp = 0;
                if(ConvertStringToHex32Bit((uint8_t *)token, strlen(token), &ulTemp) == true)
                {
                    tIRDataTimeout->ulTimeout = ulTemp;
                }
                else
                {
                	NRF_LOG_INFO("%s - %d", token, strlen(token));
                    return TIMEOUT_INDEX_INVALID;
                }
                break;
            }
            default:
                break;
        }
        token = my_strtok(NULL, "-");
        (*ubNumOfCompenent)++;
    }
    return NO_ERROR;
}

int16_t Mid_FindIRTimeout(uint16_t uwTrigID)
{
    for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
    {
        if(g_atIRTimeOut[i].uwTrigID == uwTrigID)
        {
            return i;
        }
    }
    return -1;
}

bool Mid_InsertNewIRTimeOut(IrDataTimeout_t * tIRDataTimeOut)
{
    for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
    {
        if(g_atIRTimeOut[i].uwTrigID == tIRDataTimeOut->uwTrigID && g_atIRTimeOut[i].ubStatus == TIMEOUT_ENABLE)
        {
            return false;
        }
    }
    for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
    {
        if(g_atIRTimeOut[i].ubStatus == TIMEOUT_DISABLE)
        {
            myMemCpy(&g_atIRTimeOut[i], tIRDataTimeOut, sizeof(IrDataTimeout_t));
            g_ubIRTimeOutCount++;
            Mid_FlashErase(IR_DATA_START_ADDRESS, 1);
            Mid_FlashWrite(IR_DATA_START_ADDRESS, t_IRDataCommom, SIZE_OF_IR_DATA_VAR);
            return true;
        }
    }
    return false;
}

bool Mid_RemoveIRTimeOut(uint16_t uwTrigID)
{
    for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
    {
        if(g_atIRTimeOut[i].uwTrigID == uwTrigID)
        {
            g_atIRTimeOut[i].ubStatus = TIMEOUT_DISABLE;
            g_ubIRTimeOutCount--;
            return true;
        }
    }
    return false;
}

bool Mid_InsertNewIRCode(IRData * tIRData, uint16_t uwLearnID)
{
	for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
	{
		if(t_IRDataCommom[i].ulStatus != IR_CODE_ENABLE)
		{
			t_IRDataCommom[i].ulStatus = IR_CODE_ENABLE;
			t_IRDataCommom[i].uwID = uwLearnID;
			myMemCpy(&t_IRDataCommom[i].IRData, tIRData, sizeof(decodedIRData));
			g_ubIRCount = i;
            Mid_FlashErase(IR_DATA_START_ADDRESS, 1);
            Mid_FlashWrite(IR_DATA_START_ADDRESS, t_IRDataCommom, SIZE_OF_IR_DATA_VAR);
#ifdef SHOW_DEBUG_DECODE
			NRF_LOG_INFO("/*--- Detect IR code ---*/")
			NRF_LOG_INFO("Index = %d", g_ubIRCount);
			NRF_LOG_INFO("ID = %d", t_IRDataCommom[g_ubIRCount].uwID);
			NRF_LOG_INFO("Protocol = %s", c_ubProtocolName[t_IRDataCommom[g_ubIRCount].IRData.protocol]);
			NRF_LOG_INFO("Address = %d", t_IRDataCommom[g_ubIRCount].IRData.address);
			NRF_LOG_INFO("Command = %d", t_IRDataCommom[g_ubIRCount].IRData.command);
			NRF_LOG_INFO("Number of bit = %d", t_IRDataCommom[g_ubIRCount].IRData.numberOfBits);
			NRF_LOG_INFO("Raw data = 0x%X", t_IRDataCommom[g_ubIRCount].IRData.decodedRawData);
			NRF_LOG_INFO("=> Saving IR code...");
			NRF_LOG_INFO("=> Noti to app...");
#endif
			return true;
		}
	}
	return false;
}

void Mid_DecodeRTCSet(void *p_event_data, uint16_t event_size)
{
	Mid_RTCSetUnit(ConvertStringToTimeStamp(p_event_data, event_size));
}
