#include "main.h"

#define SHOW_DEBUG_DECODE

extern const char * c_ubProtocolName[PROCOTL_LEN];

void Mid_DecodeInit(void)
{

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
    	g_ubTriggerType = TIMEOUT_TYPE;
    	app_sched_event_put(pData, len, Mid_DecodeTimeBasedScheduling); // requirement 1.5
        break;
    }
    case CUS_UUID_SCHEDULE_INDEX:
    {
    	g_ubTriggerType = SCHEDULING_TYPE;
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
        	if(g_ubTriggerType == SCHEDULING_TYPE)
        	{
        		tIRDataTrigger.ubMode = ONE_TRIGGER;
        	}
        	else if(g_ubTriggerType == TIMEOUT_TYPE)
        	{
        		tIRDataTrigger.ubMode = TIMEOUT_TRIGGER;
        	}
        }
        else if(ubNumOfCompenent == 3)
        {
            tIRDataTrigger.ubMode = MOTION_TRIGGER;
        }
        else
        {
        	// do nothing
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
    	NRF_LOG_INFO("Can't decode command");
    }
}

bool Mid_DecodeTimeBasedSchedulingDecode(uint8_t * ubCommand, IrDataTrigger_t * tIRDataTrigger,  uint8_t * ubNumOfCompenent, uint8_t * pData, uint8_t len)
{
	char * token;
	uint8_t TempData[16];
	strncpy((char * )TempData, (char *)pData, len);
	TempData[len] = '\0';
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
            	if(g_ubTriggerType == SCHEDULING_TYPE)
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
            	}
            	else if(g_ubTriggerType == TIMEOUT_TYPE)
            	{
					uint32_t ulTimeOutTemp = 0;
					if(ConvertStringToHex32Bit((uint8_t *)(token), strlen(token), &ulTimeOutTemp))
					{
						tIRDataTrigger->ulTimeout = ulTimeOutTemp;
					}
					else
					{
						return false;
					}
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
        	NRF_LOG_INFO("Can't inster new IR trigger - duplicate");
            return false;
        }
    }
    for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
    {
        if(g_atIRDataTrigger[i].ubMode != MOTION_TRIGGER &&
			g_atIRDataTrigger[i].ubMode != ONE_TRIGGER &&
			g_atIRDataTrigger[i].ubMode != WINDOW_TRIGGER &&
			g_atIRDataTrigger[i].ubMode != TIMEOUT_TRIGGER)
        {
            myMemCpy(&g_atIRDataTrigger[i], tIRDataTrigger, sizeof(IrDataTrigger_t));
        	NRF_LOG_INFO("Insert new IR trigger");
        	NRF_LOG_INFO("Mode: %d", g_atIRDataTrigger[i].ubMode);
        	NRF_LOG_INFO("TrigID: %d", g_atIRDataTrigger[i].uwTrigID);
        	NRF_LOG_INFO("IR Code: %d", g_atIRDataTrigger[i].uwIrCode);
        	NRF_LOG_INFO("Days: 0x%X", g_atIRDataTrigger[i].ubDays);
        	NRF_LOG_INFO("Time 1: %d:%d", g_atIRDataTrigger[i].tTime1.ubHour, g_atIRDataTrigger[i].tTime1.ubMin);
        	NRF_LOG_INFO("Time 2: %d:%d", g_atIRDataTrigger[i].tTime2.ubHour, g_atIRDataTrigger[i].tTime2.ubMin);
        	NRF_LOG_INFO("Time out: %d", g_atIRDataTrigger[i].ulTimeout);
            g_ubIRTriggerCount++;
            Mid_FlashErase(IR_TRIG_START_ADDRESS, 1);
            Mid_FlashWrite(IR_TRIG_START_ADDRESS, g_atIRDataTrigger, SIZE_OF_IR_TRIG_VAR);
            return true;
        }
    }
    NRF_LOG_INFO("Can't inster new IR trigger");
    return false;
}

bool Mid_RemoveIRTrigger(uint16_t uwTrigID)
{
    for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
    {
        if(g_atIRDataTrigger[i].uwTrigID == uwTrigID && g_atIRDataTrigger[i].ubMode != NONE)
        {
            g_atIRDataTrigger[i].ubMode = NONE;
            NRF_LOG_INFO("Delete TR Trigger: %d", g_atIRDataTrigger[i].uwTrigID);
            Mid_FlashErase(IR_TRIG_START_ADDRESS, 1);
            Mid_FlashWrite(IR_TRIG_START_ADDRESS, t_IRDataCommom, SIZE_OF_IR_DATA_VAR);
            g_ubIRTriggerCount--;
            return true;
        }
    }
    return false;
}

void Mid_EraseAllITrigger(void)
{
	for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
	{
		g_atIRDataTrigger[i].uwTrigID = CLEAR;
		g_atIRDataTrigger[i].ubMode = NONE;
	}
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
			NRF_LOG_INFO("Raw data = 0x%X", t_IRDataCommom[g_ubIRCount].IRData.decodedRawDataArray[0]);
			NRF_LOG_INFO("Raw data = 0x%X", t_IRDataCommom[g_ubIRCount].IRData.decodedRawDataArray[1]);
			NRF_LOG_INFO("Raw data = 0x%X", t_IRDataCommom[g_ubIRCount].IRData.decodedRawDataArray[2]);
			NRF_LOG_INFO("Raw data = 0x%X", t_IRDataCommom[g_ubIRCount].IRData.decodedRawDataArray[3]);
			NRF_LOG_INFO("=> Saving IR code...");
			NRF_LOG_INFO("=> Noti to app...");
#endif
			return true;
		}
	}
	return false;
}

bool Mid_RemoveIRCode(uint16_t ulEraseID)
{
	for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
	{
		if(t_IRDataCommom[i].uwID == ulEraseID && t_IRDataCommom[i].ulStatus == IR_CODE_ENABLE)
		{
			t_IRDataCommom[i].uwID = CLEAR;
			t_IRDataCommom[i].ulStatus = IR_CODE_DISABLE;
            Mid_FlashErase(IR_DATA_START_ADDRESS, 1);
            Mid_FlashWrite(IR_DATA_START_ADDRESS, t_IRDataCommom, SIZE_OF_IR_DATA_VAR);
		}
	}
	return false;
}

void Mid_EraseAllIRCode(void)
{
	for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
	{
		t_IRDataCommom[i].uwID = CLEAR;
		t_IRDataCommom[i].ulStatus = IR_CODE_DISABLE;
	}
}

void Mid_DecodeRTCSet(void *p_event_data, uint16_t event_size)
{
	Mid_RTCSetUnit(ConvertStringToTimeStamp(p_event_data, event_size));
}
