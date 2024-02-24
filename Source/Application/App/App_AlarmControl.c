#include "main.h"

void App_AlarmInit(void)
{

}

void App_AlarmOnTimeHandler(void)
{
	static uint8_t uuPreMinutes = CLEAR;
	if(uuPreMinutes != g_tCommonTime.minute)
	{
		for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
		{
			if(g_atIRDataTrigger[i].ubMode == ONE_TRIGGER)
			{
				if(bit_is_set(g_atIRDataTrigger[i].ubDays, CONVERT_DAYS_TO_INDED(g_tCommonTime.dayOfWeek)))
				{
					if(g_atIRDataTrigger[i].tTime1.ubHour == g_tCommonTime.hour && \
					   g_atIRDataTrigger[i].tTime1.ubMin == g_tCommonTime.minute)
					{
						app_sched_event_put(&g_atIRDataTrigger[i].uwIrCode, sizeof(g_atIRDataTrigger[i].uwIrCode), App_ControlStartEmitIRWithID);
					}
				}

			}
		}
		uuPreMinutes = g_tCommonTime.minute;
	}
}

void App_AlarmMotionTriggerHandler(void)
{
	static uint8_t uuPreMinutes = CLEAR;
	if(MOTION_SENSOR_START_SENSING == C_ON)
	{
		g_ubMotionSensorCountTime++;
		if(g_ubMotionSensorCountTime >= TIME_2S_BY_100MS)
		{
			MOTION_SENSOR_DISABLE = C_ON;
			MOTION_SENSOR_SENSING_DONE = C_ON;
			MOTION_SENSOR_START_SENSING = C_OFF;
			g_ubMotionSensorCountTime = CLEAR;
			//NRF_LOG_INFO("Sensor leve: %d - %d", g_ubMotionSensorGrade, g_ubMotionCount);
			for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
			{
				if(g_atIRDataTrigger[i].ubMode == MOTION_TRIGGER)
				{
//					if(g_atIRDataTrigger[i].ubMotionSen >= g_ubMotionSensorGrade-1	&&
//					   g_atIRDataTrigger[i].ubMotionSen <= g_ubMotionSensorGrade+1)
					if(g_atIRDataTrigger[i].ubMotionSen == g_ubMotionSensorGrade)
					{
						NRF_LOG_INFO("MOTION TRIGGER: %d", g_atIRDataTrigger[i].uwIrCode);
						app_sched_event_put(&g_atIRDataTrigger[i].uwIrCode, sizeof(g_atIRDataTrigger[i].uwIrCode), App_ControlStartEmitIRWithID);
					}
				}
				if(g_atIRDataTrigger[i].ubMode == WINDOW_TRIGGER)
				{
					if(bit_is_set(g_atIRDataTrigger[i].ubDays, CONVERT_DAYS_TO_INDED(g_tCommonTime.dayOfWeek)))
					{
						if(uuPreMinutes != g_tCommonTime.minute)
						{
							if((CONVERT_HOUR_TO_MIN(g_tCommonTime.hour, g_tCommonTime.minute) >= CONVERT_HOUR_TO_MIN(g_atIRDataTrigger[i].tTime1.ubHour, g_atIRDataTrigger[i].tTime1.ubMin)) && \
							   (CONVERT_HOUR_TO_MIN(g_tCommonTime.hour, g_tCommonTime.minute) <= CONVERT_HOUR_TO_MIN(g_atIRDataTrigger[i].tTime2.ubHour, g_atIRDataTrigger[i].tTime2.ubMin)))
							{
								if(g_atIRDataTrigger[i].ubMotionSen == g_ubMotionSensorGrade)
								{
									NRF_LOG_INFO("WINDOW TRIGGER: %d", g_atIRDataTrigger[i].uwIrCode);
									app_sched_event_put(&g_atIRDataTrigger[i].uwIrCode, sizeof(g_atIRDataTrigger[i].uwIrCode), App_ControlStartEmitIRWithID);
								}
							}
							uuPreMinutes = g_tCommonTime.minute;
						}
					}
				}
			}
			g_ubMotionSensorGrade = SENSITIVITY_LEVEL_NONE;
		}
	}
}

void App_AlarmMotionTimeOutTrigger(void)
{

	for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
	{
		if(g_atIRDataTrigger[i].ubMode == TIMEOUT_TRIGGER)
		{
			if(bit_is_set(g_atIRDataTrigger[i].ubDays, CONVERT_DAYS_TO_INDED(g_tCommonTime.dayOfWeek)))
			{
				if(g_ulMotionTimeOffCount >= g_atIRDataTrigger[i].ulTimeout && g_atIRDataTrigger[i].ubTriggerDone != TRIGGER_DONE_MASK)
				{
					NRF_LOG_INFO("TRIGGER TIMEOUT: %d", i);
					app_sched_event_put(&g_atIRDataTrigger[i].uwIrCode, sizeof(g_atIRDataTrigger[i].uwIrCode), App_ControlStartEmitIRWithID);
					g_atIRDataTrigger[i].ubTriggerDone = TRIGGER_DONE_MASK;
				}
			}
		}
	}
}
