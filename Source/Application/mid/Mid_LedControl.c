#include "main.h"

uint8_t Lable[3] = {PIN_RED_LED, PIN_GREEN_LED, PIN_BLUE_LED};

uint8_t g_aubLedData[TOTAL_LED_TYPE][5] =
{
    // Color - On time * 100ms - Off time * 100ms - Continues times - Delay times * 100ms
    {CLEAR, CLEAR, CLEAR, CLEAR, CLEAR},
    {LED_MODE_0_MASK|COLOR_RED_MASK, TIME_100MS_BY_100MS, TIME_100MS_BY_100MS, 3, 0}, // LED_TYPE_BLE_CONNECT
    {LED_MODE_0_MASK|COLOR_RED_MASK, TIME_500MS_BY_100MS, TIME_100MS_BY_100MS, 3, 0}, // LED_TYPE_BLE_DISCONNECT
    {LED_MODE_0_MASK|COLOR_BLUE_MASK, TIME_100MS_BY_100MS, TIME_100MS_BY_100MS, 4, 0}, // LED_TYPE_BLE_RECEIVE
    {LED_MODE_0_MASK|COLOR_GREEN_MASK, TIME_100MS_BY_100MS, TIME_100MS_BY_100MS, 5, 0}, // LED_TYPE_SEND_IR
    {LED_MODE_0_MASK|COLOR_RED_MASK, TIME_1S_BY_100MS, TIME_1S_BY_100MS, 0xFF, 0}, // LED_TYPE_ERROR
    {LED_MODE_1_MASK|COLOR_GREEN_MASK|COLOR_BLUE_MASK, TIME_1S_BY_100MS, TIME_1S_BY_100MS, 0xFF, 0}, // LED_TYPE_RESET
    {LED_MODE_0_MASK|COLOR_BLUE_MASK, TIME_5S_BY_100MS, TIME_100MS_BY_100MS, 1, 0},// LED_TYPE_MOTION_DETECT
    {COLOR_RED_MASK|COLOR_GREEN_MASK|COLOR_BLUE_MASK, TIME_1S_BY_100MS, TIME_1S_BY_100MS, 2, TIME_1S_BY_100MS}
};

void Mid_LedControlInit(void)
{
	nrf_gpio_cfg_output(PIN_RED_LED);
	nrf_gpio_cfg_output(PIN_GREEN_LED);
	nrf_gpio_cfg_output(PIN_BLUE_LED);

	nrf_gpio_pin_set(PIN_RED_LED);
	nrf_gpio_pin_set(PIN_GREEN_LED);
	nrf_gpio_pin_set(PIN_BLUE_LED);
}

void Mid_LedControlHandle(void) // every 100ms
{
    if(LED_START_CONTROL_FLAG == C_ON)
    {
        if(g_ubLedControlState == LED_STATE_ON)
        {
            if(LED_START_ON_FLAG == C_ON)
            {
                if(((g_aubLedData[g_ubLedType][CONFIG_INDEX] & 0x18) >> 3) == 0)
                {
                    if(bit_is_set(g_aubLedData[g_ubLedType][CONFIG_INDEX], COLOR_RED_BIT))
                    {
                    	nrf_gpio_pin_clear(PIN_RED_LED);
                    }
                    if(bit_is_set(g_aubLedData[g_ubLedType][CONFIG_INDEX], COLOR_GREEN_BIT))
                    {
                    	nrf_gpio_pin_clear(PIN_GREEN_LED);
                    }
                    if(bit_is_set(g_aubLedData[g_ubLedType][CONFIG_INDEX], COLOR_BLUE_BIT))
                    {
                    	nrf_gpio_pin_clear(PIN_BLUE_LED);
                    }
                }
                else if(((g_aubLedData[g_ubLedType][CONFIG_INDEX] & 0x18) >> 3) == 1)
                {
                    for(uint8_t i = 0 ; i < 3 ; i++)
                    {
                        if(bit_is_set(g_aubLedData[g_ubLedType][CONFIG_INDEX], i))
                        {
                        	nrf_gpio_pin_clear(Lable[i]);
                            break;
                        }
                    }
                }
                LED_START_ON_FLAG = C_OFF;
            }
            g_ubLedCountTime++;
            if(g_ubLedCountTime >= g_aubLedData[g_ubLedType][ON_TIME_INDEX])
            {
                g_ubLedCountTime = 0;
                g_ubLedControlState = LED_STATE_OFF;
                LED_START_OFF_FLAG = C_ON;
            }
        }
        if(g_ubLedControlState == LED_STATE_OFF)
        {
            if(LED_START_OFF_FLAG == C_ON)
            {
                if(((g_aubLedData[g_ubLedType][CONFIG_INDEX] & 0x18) >> 3) == 0)
                {
                    if(bit_is_set(g_aubLedData[g_ubLedType][CONFIG_INDEX], COLOR_RED_BIT))
                    {
                    	nrf_gpio_pin_set(PIN_RED_LED);
                    }
                    if(bit_is_set(g_aubLedData[g_ubLedType][CONFIG_INDEX], COLOR_GREEN_BIT))
                    {
                    	nrf_gpio_pin_set(PIN_GREEN_LED);
                    }
                    if(bit_is_set(g_aubLedData[g_ubLedType][CONFIG_INDEX], COLOR_BLUE_BIT))
                    {
                    	nrf_gpio_pin_set(PIN_BLUE_LED);
                    }
                }
                else if(((g_aubLedData[g_ubLedType][CONFIG_INDEX] & 0x18) >> 3) == 1)
                {
                    for(int8_t i = 2 ; i >= 0 ; i--)
                    {
                        if(bit_is_set(g_aubLedData[g_ubLedType][CONFIG_INDEX], i))
                        {
                        	nrf_gpio_pin_set(Lable[i]);
                            break;
                        }
                    }
                }
                LED_START_OFF_FLAG = C_OFF;
            }
            g_ubLedCountTime++;
            if(g_ubLedCountTime >= g_aubLedData[g_ubLedType][OFF_TIME_INDEX])
            {
                g_ubLedCountTime = 0;
                g_ubLedControlState = LED_STATE_DELAY;
            }
        }
        if(g_ubLedControlState == LED_STATE_DELAY)
        {
            g_ubLedCountTime++;
            if(g_ubLedCountTime >= g_aubLedData[g_ubLedType][DEPLAY_TIME_INDEX])
            {
                g_ubLedCountTime = 0;
                g_ubLedCountContinuesTime++;
                if(g_ubLedCountContinuesTime >= g_aubLedData[g_ubLedType][CONTINUES_TIMESS_INDEX] \
                    && g_aubLedData[g_ubLedType][CONTINUES_TIMESS_INDEX] != 0xFF)
                {
                    Mid_LedControlSet(LED_TYPE_OFF);
                }
                else
                {
                    g_ubLedControlState = LED_STATE_ON;
                    LED_START_ON_FLAG = C_ON;
                }
            }

        }
    }
}

void Mid_LedControlSet(uint8_t ubLedType)
{
    g_ubLedType = ubLedType;
    if(ubLedType == LED_TYPE_OFF)
    {
        g_ubLedCountContinuesTime =  CLEAR;
        g_ubLedCountTime = CLEAR;
        LED_START_CONTROL_FLAG = C_OFF;
        LED_START_ON_FLAG = C_OFF;
        LED_START_OFF_FLAG = C_OFF;
        nrf_gpio_pin_set(PIN_RED_LED);
        nrf_gpio_pin_set(PIN_GREEN_LED);
        nrf_gpio_pin_set(PIN_BLUE_LED);
    }
    else if(ubLedType > LED_TYPE_OFF && ubLedType < TOTAL_LED_TYPE)
    {
        g_ubLedControlState = LED_STATE_ON;
        if(LED_START_CONTROL_FLAG == C_OFF)
        {
            LED_START_CONTROL_FLAG = C_ON;
            LED_START_ON_FLAG = C_ON;
            NRF_LOG_INFO("Set led: %d", g_ubLedType);
        }
        else
        {
            g_ubLedCountContinuesTime =  CLEAR;
            g_ubLedCountTime = CLEAR;
            LED_START_OFF_FLAG = C_OFF;
            LED_START_ON_FLAG = C_OFF;
        }

    }
}
