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
