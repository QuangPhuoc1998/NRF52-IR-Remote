#include "main.h"

const char * c_ubProtocolName[PROCOTL_LEN] = PROTOCOLNAME;
static uint16_t uwBufferLearnID = CLEAR;
static uint16_t uwBufferEmitID = CLEAR;
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
				t_IRDataCommom[g_ubIRCount].uwID = uwBufferLearnID;
				myMemCpy(&t_IRDataCommom[g_ubIRCount].IRData, &decodedIRData, sizeof(decodedIRData));
				// Show infor IR code
				NRF_LOG_INFO("/*--- Detect IR code ---*/")
				NRF_LOG_INFO("ID = %d", t_IRDataCommom[g_ubIRCount].uwID);
				NRF_LOG_INFO("Protocol = %s", c_ubProtocolName[t_IRDataCommom[g_ubIRCount].IRData.protocol]);
				NRF_LOG_INFO("Address = %d", t_IRDataCommom[g_ubIRCount].IRData.address);
				NRF_LOG_INFO("Command = %d", t_IRDataCommom[g_ubIRCount].IRData.command);
				NRF_LOG_INFO("Number of bit = %d", t_IRDataCommom[g_ubIRCount].IRData.numberOfBits);
				NRF_LOG_INFO("Raw data = 0x%X", t_IRDataCommom[g_ubIRCount].IRData.decodedRawData);
				NRF_LOG_INFO("=> Saving IR code...");
				NRF_LOG_INFO("=> Noti to app...")
				g_ubIRCount++;
			}
			else
			{
				// Stop current process
				DETECT_IR_SIGNAL_DONE_FLAG = C_OFF;
				APP_START_LEARN_IR_FLAG = C_OFF;
				APP_LEARN_IR_COUNT_START_FLAG = C_OFF;
				NRF_LOG_INFO("Can't decode IR signal! Please try again");
			}
		}
	}

	if(APP_START_EMIT_IR_FLAG == C_ON)
	{
		int16_t ubIndex = LinearSerachID(t_IRDataCommom, 64, uwBufferEmitID);
		if(ubIndex != -1)
		{
			NRF_LOG_INFO("/*--- Emit IR code ---*/")
			NRF_LOG_INFO("ID = %d", t_IRDataCommom[ubIndex].uwID);
			NRF_LOG_INFO("Protocol = %s", c_ubProtocolName[t_IRDataCommom[ubIndex].IRData.protocol]);
			NRF_LOG_INFO("Address = %d", t_IRDataCommom[ubIndex].IRData.address);
			NRF_LOG_INFO("Command = %d", t_IRDataCommom[ubIndex].IRData.command);
			NRF_LOG_INFO("Number of bit = %d", t_IRDataCommom[ubIndex].IRData.numberOfBits);
			NRF_LOG_INFO("Raw data = 0x%X", t_IRDataCommom[ubIndex].IRData.decodedRawData);

			sendPanasonic(t_IRDataCommom[ubIndex].IRData.address, t_IRDataCommom[ubIndex].IRData.command, 1);

		}
		else
		{
			NRF_LOG_INFO("Can't find IR code");
		}
		APP_START_EMIT_IR_FLAG = C_OFF;
	}
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

void App_ControlStartEmitIR(uint16_t uwIrID)
{
	APP_START_EMIT_IR_FLAG = C_ON;
	uwBufferEmitID = uwIrID;
}

