#define GLOBAL_DEFINE (1)

#include "main.h"

#include "Mid_IrRemote.h"
#include "Mid_MotionSensor.h"

#include "hw_tests.h"

#define SCHED_MAX_EVENT_DATA_SIZE   24
#define SCHED_QUEUE_SIZE            24
#define TEST_IR_MODULE
extern const char * c_ubProtocolName[PROCOTL_LEN];

void InitVarialbe(void);
void ProcessMain(void);
void TimerAndScherInit(void);
void SysTimer10msCall(void * p_context);
void SysTimer100msCall(void * p_context);
void SysTimer1000msCall(void * p_context);
void IdleStateHandle(void);
int main(void)
{
	lib_ble_init();
	lib_ble_advertising_start();
    bsp_init();
    Mid_MotionSensorInit();
    TimerAndScherInit();
    Mid_IrRemoteInit();
    SystimerInit();
    Mid_RTCInit();
    Mid_FlashInit();
    Mid_KeyControlInit();
    Mid_LedControlInit();

#ifdef TEST_IR_MODULE
    Mid_IrRemoteStartScan();
#endif
    NRF_LOG_INFO("/*--- APP START ---*/");
    for (;;)
    {
    	ProcessMain();
    	app_sched_execute();
    	IdleStateHandle();
    }
}
void ProcessMain(void)
{
	// App control
	App_ControlHandle();
	// Mid IR control
#ifndef TEST_IR_MODULE
    if(IR_SCAN_DELAY_FLAG == C_OFF)
    {
    	if(Mid_IrRemoteDecode())
    	{
    		Mid_IrRemoteStopScan();
			Ir_RemoteResume();
			IR_SCAN_DELAY_FLAG = C_ON;
			DETECT_IR_SIGNAL_DONE_FLAG = C_ON;
    	}
    }
#else
    if(Mid_IrRemoteDecode())
	{
		NRF_LOG_INFO("/*--- Detect IR code ---*/")
		NRF_LOG_INFO("Protocol = %s", c_ubProtocolName[decodedIRData.protocol]);
		NRF_LOG_INFO("Address = %d", decodedIRData.address);
		NRF_LOG_INFO("Command = %d", decodedIRData.command);
		NRF_LOG_INFO("Number of bit = %d", decodedIRData.numberOfBits);
    	Ir_RemoteResume();
	}
#endif

}
void SysTimer10msCall(void * p_context)
{
	static uint8_t ubCount20ms = CLEAR;
	if(ubCount20ms >= TIME_20MS_BY_10MS)
	{
		app_sched_event_put(NULL, 0, Mid_KeyHandler);
		ubCount20ms = CLEAR;
	}
	ubCount20ms++;
}
void SysTimer100msCall(void * p_context)
{
#ifndef TEST_IR_MODULE
	if(APP_START_LEARN_IR_FLAG == C_ON && APP_LEARN_IR_COUNT_START_FLAG == C_ON)
	{
		g_ubScanIRCount++;
		if(g_ubScanIRCount >= TIME_5S_BY_100MS)
		{
			g_ubScanIRCount = CLEAR;
			APP_LEARN_IR_COUNT_START_FLAG = C_OFF;
			APP_LEARN_IR_COUNT_TIMEOUT_FLAG = C_OFF;
			Mid_IrRemoteStopScan();
			if(decodedIRData.protocol == UNKNOWN)
			{
				lib_ble_noti(NOTI_LEARN_IR_FAIL);
			}
		}
	}
	//
	if(IR_SCAN_DELAY_FLAG == C_ON)
	{
		if(APP_START_LEARN_IR_FLAG == C_ON)
		{
			g_ubDelayCount++;
			if(g_ubDelayCount >= TIME_2S_BY_100MS)
			{
				g_ubDelayCount = CLEAR;
				IR_SCAN_DELAY_FLAG = C_OFF;
				Mid_IrRemoteStartScan();
			}
		}
		else
		{
			g_ubDelayCount = CLEAR;
			IR_SCAN_DELAY_FLAG = C_OFF;
		}
	}
#endif
	//
	App_AlarmOnTimeHandler();
	//
	App_AlarmMotionTriggerHandler();
	//
	if(MOTION_SENSOR_DISABLE == C_ON)
	{
		g_ubMotionSensorDisableTime++;
		if(g_ubMotionSensorDisableTime >= TIME_3S_BY_100MS)
		{
			MOTION_SENSOR_DISABLE = C_OFF;
			g_ubMotionSensorDisableTime = CLEAR;
		}
	}
	//
	Mid_LedControlHandle();
}
void SysTimer1000msCall(void * p_context)
{
	app_sched_event_put(NULL, sizeof(NULL), Mid_RTCHandle);
	//
	if(MOTION_SENSOR_TIMEOUT_COUNT == C_ON)
	{
		g_ulMotionTimeOffCount++;
	}
	else
	{
		g_ulMotionTimeOffCount = CLEAR;
	}
	//
	if(MOTION_SENSOR_PRE_TIMEOUT_COUNT != MOTION_SENSOR_TIMEOUT_COUNT)
	{
		if(MOTION_SENSOR_TIMEOUT_COUNT == C_ON)
		{
			for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
			{
				if(g_atIRDataTrigger[i].ubMode == TIMEOUT_TRIGGER)
				{
					g_atIRDataTrigger[i].ubTriggerDone = 0xFF;
				}
			}
		}
		MOTION_SENSOR_PRE_TIMEOUT_COUNT = MOTION_SENSOR_TIMEOUT_COUNT;
	}
	//
	App_AlarmMotionTimeOutTrigger();
	//
//	g_ubBatteryLevel++;
//	if(BLE_CONNECTED_FLAG == C_ON)
//	{
//		lib_ble_noti(NOTI_LEARN_IR_FAIL);
//	}
//	if(g_ubBatteryLevel >= 100)
//	{
//		g_ubBatteryLevel = 0;
//	}
//	lib_ble_update_bat_level(g_ubBatteryLevel);
}

void TimerAndScherInit(void)
{
	ret_code_t err_code;

	APP_TIMER_DEF(Timer10msID);
    APP_TIMER_DEF(Timer100msID);
    APP_TIMER_DEF(Timer1000msID);
    // Timer 10ms init
    err_code = app_timer_create(&Timer10msID, APP_TIMER_MODE_REPEATED, SysTimer10msCall);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_start(Timer10msID, APP_TIMER_TICKS(10), NULL);
	APP_ERROR_CHECK(err_code);
	// Timer 100ms init
    err_code = app_timer_create(&Timer100msID, APP_TIMER_MODE_REPEATED, SysTimer100msCall);
    APP_ERROR_CHECK(err_code);
    err_code = app_timer_start(Timer100msID, APP_TIMER_TICKS(100), NULL);
    APP_ERROR_CHECK(err_code);
    // Timer 1000ms init
    err_code = app_timer_create(&Timer1000msID, APP_TIMER_MODE_REPEATED, SysTimer1000msCall);
    APP_ERROR_CHECK(err_code);
    err_code = app_timer_start(Timer1000msID, APP_TIMER_TICKS(1000), NULL);
    APP_ERROR_CHECK(err_code);
    // App scheduler init
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}
void IdleStateHandle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}
void InitVarialbe(void)
{
	g_ubMotionSensTemp = 0xFF;
}
