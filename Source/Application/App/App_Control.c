#include "main.h"

const char * c_ubProtocolName[PROCOTL_LEN] = PROTOCOLNAME;
static uint16_t uwBufferLearnID = CLEAR;
static uint16_t uwBufferEraseID = CLEAR;
void App_ControlHandle(void)
{
	if(APP_START_LEARN_IR_FLAG == C_ON)
	{
		if(DETECT_IR_SIGNAL_DONE_FLAG == C_ON)
		{
			if(decodedIRData.protocol != UNKNOWN)
			{
				// Stop current process
				DETECT_IR_SIGNAL_DONE_FLAG = C_OFF;
				APP_START_LEARN_IR_FLAG = C_OFF;
				APP_LEARN_IR_COUNT_START_FLAG = C_OFF;
				// Save IR code
				Mid_InsertNewIRCode(&decodedIRData, uwBufferLearnID);

			}
			else
			{
				// Stop current process
				DETECT_IR_SIGNAL_DONE_FLAG = C_OFF;
				APP_START_LEARN_IR_FLAG = C_OFF;
				APP_LEARN_IR_COUNT_START_FLAG = C_OFF;
				NRF_LOG_INFO("Can't decode IR signal! Please try again");
				lib_ble_noti(NOTI_LEARN_IR_FAIL);
			}
		}
	}

	if(APP_START_EMIT_IR_FLAG == C_ON)
	{

		APP_START_EMIT_IR_FLAG = C_OFF;
	}

	if(APP_START_ERASE_IR_FLAG == C_ON)
	{
		Mid_RemoveIRCode(uwBufferEraseID);
		APP_START_ERASE_IR_FLAG = C_OFF;
	}

	if(KEY_3SEC_CLICK_FLAG == C_ON)
	{
		if(g_ulKeyInValue == KEY_SWITCH_1)
		{
			NRF_LOG_INFO("Key switch 1: 3s");
			NRF_LOG_INFO("=> Erase all data");
			Mid_FlashErase(IR_DATA_START_ADDRESS, 1);
			Mid_EraseAllIRCode();
			Mid_FlashErase(IR_TRIG_START_ADDRESS, 1);
			Mid_EraseAllITrigger();

		}

		if(g_ulKeyInValue == KEY_SWITCH_2)
		{
			NRF_LOG_INFO("Key switch 2: 3s");
		}
	}
	KEY_1CLICK_FLAG = C_OFF;
	KEY_0_5SEC_CLICK_FLAG = C_OFF;
	KEY_1SEC_CLICK_FLAG = C_OFF;
	KEY_1_5SEC_CLICK_FLAG = C_OFF;
	KEY_2SEC_CLICK_FLAG = C_OFF;
	KEY_3SEC_CLICK_FLAG = C_OFF;
	KEY_5SEC_CLICK_FLAG = C_OFF;
	KEY_7SEC_CLICK_FLAG = C_OFF;
	KEY_RELEASE_CLICK_FLAG = C_OFF;
	KEY_PRESS_HOLD_FLAG = C_OFF;
}
void App_ControlStartLearnIR(uint16_t uwIrID)
{
	APP_START_LEARN_IR_FLAG = C_ON;
	APP_LEARN_IR_COUNT_START_FLAG = C_ON;
	APP_LEARN_IR_COUNT_TIMEOUT_FLAG = C_OFF;
	Mid_IrRemoteStartScan();
	g_ubScanIRCount = CLEAR;
	uwBufferLearnID = uwIrID;
}

void App_ControlStartEmitIR(void *pData, uint16_t len)
{
	uint16_t uwBufferEmitID = ConvertID(pData, len);
	int16_t ubIndex = LinearSerachID(t_IRDataCommom, MAX_IR_CODE, uwBufferEmitID);
	if(ubIndex != -1)
	{
		NRF_LOG_INFO("/*--- Emit IR code ---*/")
		NRF_LOG_INFO("ID = %d", t_IRDataCommom[ubIndex].uwID);
		NRF_LOG_INFO("Protocol = %s", c_ubProtocolName[t_IRDataCommom[ubIndex].IRData.protocol]);
		NRF_LOG_INFO("Address = %d", t_IRDataCommom[ubIndex].IRData.address);
		NRF_LOG_INFO("Command = %d", t_IRDataCommom[ubIndex].IRData.command);
		NRF_LOG_INFO("Number of bit = %d", t_IRDataCommom[ubIndex].IRData.numberOfBits);
		NRF_LOG_INFO("Raw data = 0x%X", t_IRDataCommom[ubIndex].IRData.decodedRawData);

		Mid_IrSendCommon(&t_IRDataCommom[ubIndex].IRData, 1);
		Mid_LedControlSet(LED_TYPE_SEND_IR);
	}
	else
	{
		NRF_LOG_INFO("Can't find IR code");
	}
}

void App_ControlStartEmitIRWithID(void *pData, uint16_t len)
{
	uint16_t uwBufferEmitID = ConvertArrayToNumber(pData, len);
	int16_t ubIndex = LinearSerachID(t_IRDataCommom, MAX_IR_CODE, uwBufferEmitID);
	if(ubIndex != -1)
	{
		NRF_LOG_INFO("/*--- Emit IR code ---*/")
		NRF_LOG_INFO("ID = %d", t_IRDataCommom[ubIndex].uwID);
		NRF_LOG_INFO("Protocol = %s", c_ubProtocolName[t_IRDataCommom[ubIndex].IRData.protocol]);
		NRF_LOG_INFO("Address = %d", t_IRDataCommom[ubIndex].IRData.address);
		NRF_LOG_INFO("Command = %d", t_IRDataCommom[ubIndex].IRData.command);
		NRF_LOG_INFO("Number of bit = %d", t_IRDataCommom[ubIndex].IRData.numberOfBits);
		NRF_LOG_INFO("Raw data = 0x%X", t_IRDataCommom[ubIndex].IRData.decodedRawData);

		Mid_IrSendCommon(&t_IRDataCommom[ubIndex].IRData, 1);
		Mid_LedControlSet(LED_TYPE_SEND_IR);
	}
	else
	{
		NRF_LOG_INFO("Can't find IR code");
	}
}

void App_ControlEraseIR(uint16_t uwIrID)
{
	APP_START_ERASE_IR_FLAG = C_ON;
	uwBufferEraseID = uwIrID;
}

