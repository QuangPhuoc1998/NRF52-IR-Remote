#include "main.h"

#define SHOW_DEBUG_RTC

void Mid_RTCInit(void)
{

}

void Mid_RTCHandle(void *p_event_data, uint16_t event_size)
{
	g_ullUnixTimeStamp++;
	Mid_RTCConvertUnixToDate(g_ullUnixTimeStamp, &g_tCommonTime);
//	NRF_LOG_INFO("Hour : %d", g_tCommonTime.hour);
//	NRF_LOG_INFO("Min : %d", g_tCommonTime.minute);
//	NRF_LOG_INFO("Sec : %d", g_tCommonTime.second);
//	NRF_LOG_INFO("Day : %d", g_tCommonTime.day);
//	NRF_LOG_INFO("Mon : %d", g_tCommonTime.month);
//	NRF_LOG_INFO("Year : %d\n", g_tCommonTime.year);

}

void Mid_RTCSetUnit(uint64_t ullUnixValue)
{
	g_ullUnixTimeStamp = ullUnixValue;
	Mid_RTCConvertUnixToDate(g_ullUnixTimeStamp, &g_tCommonTime);
#ifdef SHOW_DEBUG_RTC
	NRF_LOG_INFO("Set time");
	NRF_LOG_INFO("Unix : %d", g_ullUnixTimeStamp);
	NRF_LOG_INFO("Hour : %d", g_tCommonTime.hour);
	NRF_LOG_INFO("Min : %d", g_tCommonTime.minute);
	NRF_LOG_INFO("Sec : %d", g_tCommonTime.second);
	NRF_LOG_INFO("Day : %d", g_tCommonTime.day);
	NRF_LOG_INFO("Mon : %d", g_tCommonTime.month);
	NRF_LOG_INFO("Year : %d\n", g_tCommonTime.year);
#endif
}

void Mid_RTCConvertUnixToDate(uint64_t ulUnix, date_time_t * pDate)
{
	uint32_t a;
	uint32_t b;
	uint32_t c;
	uint32_t d;
	uint32_t e;
	uint32_t f;

	//Negative Unix time values are not supported
	if(ulUnix < 1)
	{
		ulUnix = 0;
	}
	// Indochina time
//	ulUnix += 7 * 60 * 60;
	//Clear milliseconds
	pDate->milliseconds = 0;

	//Retrieve hours, minutes and seconds
	pDate->second = ulUnix % 60;
	ulUnix /= 60;
	pDate->minute = ulUnix % 60;
	ulUnix /= 60;
	pDate->hour = ulUnix % 24;
	ulUnix /= 24;

	//Convert Unix time to pDate
	a = (uint32_t) ((4 * ulUnix + 102032) / 146097 + 15);
	b = (uint32_t) (ulUnix + 2442113 + a - (a / 4));
	c = (20 * b - 2442) / 7305;
	d = b - 365 * c - (c / 4);
	e = d * 1000 / 30601;
	f = d - e * 30 - e * 601 / 1000;

	//January and February are counted as months 13 and 14 of the previous year
	if(e <= 13)
	{
	  c -= 4716;
	  e -= 1;
	}
	else
	{
	  c -= 4715;
	  e -= 13;
	}

	//Retrieve year, month and day
	pDate->year = c;
	pDate->month = e;
	pDate->day = f;

	//Calculate day of week
	pDate->dayOfWeek = Mid_RTCComputeDayOfWeek(c, e, f);
}

uint64_t Mid_RTCConvertDateToUnixTime(const date_time_t *date)
{
	uint8_t y;
	uint8_t m;
	uint8_t d;
	uint64_t t;

	//Year
	y = date->year;
	//Month of year
	m = date->month;
	//Day of month
	d = date->day+1;

	//January and February are counted as months 13 and 14 of the previous year
	if(m <= 2)
	{
	  m += 12;
	  y -= 1;
	}

	//Convert years to days
	t = (365 * y) + (y / 4) - (y / 100) + (y / 400);
	//Convert months to days
	t += (30 * m) + (3 * (m + 1) / 5) + d;
	//Unix time starts on January 1st, 1970
	t -= 65047;
	//Convert days to seconds
	t *= 86400;
	//Add hours, minutes and seconds
	t += (3600 * date->hour) + (60 * date->minute) + date->second;

	//Return Unix time
	return t;
}

uint8_t Mid_RTCComputeDayOfWeek(uint16_t ulYear, uint8_t ubMonth, uint8_t ubDay)
{
	uint16_t h;
	uint16_t j;
	uint16_t k;

	//January and February are counted as months 13 and 14 of the previous year
	if(ubMonth <= 2)
	{
		ubMonth += 12;
		ulYear -= 1;
	}

	//J is the century
	j = ulYear / 100;
	//K the year of the century
	k = ulYear % 100;

	//Compute H using Zeller's congruence
	h = ubDay + (26 * (ubMonth + 1) / 10) + k + (k / 4) + (5 * j) + (j / 4);

	//Return the day of the week
	return ((h + 5) % 7) + 1;
}
