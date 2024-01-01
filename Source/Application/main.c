#define GLOBAL_DEFINE (1)

#include "main.h"

#include "Mid_IrRemote.h"
#include "Mid_MotionSensor.h"
#include "ir_transmitter.h"
#include "ir_decoder.h"

#include "hw_tests.h"

#define SCHED_MAX_EVENT_DATA_SIZE   MAX(sizeof(nrf_drv_gpiote_pin_t), APP_TIMER_SCHED_EVENT_DATA_SIZE)
#define SCHED_QUEUE_SIZE            10

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

}
void SysTimer10msCall(void * p_context)
{

}
void SysTimer100msCall(void * p_context)
{
	if(APP_START_LEARN_IR_FLAG == C_ON && APP_LEARN_IR_COUNT_START_FLAG == C_ON)
	{
		g_ubScanIRCount++;
		if(g_ubScanIRCount >= TIME_5S_BY_100MS)
		{
			g_ubScanIRCount = CLEAR;
			APP_LEARN_IR_COUNT_START_FLAG = C_OFF;
			APP_LEARN_IR_COUNT_TIMEOUT_FLAG = C_OFF;
			Mid_IrRemoteStopScan();
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
}
void SysTimer1000msCall(void * p_context)
{

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
