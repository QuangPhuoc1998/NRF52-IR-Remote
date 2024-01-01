#include "main.h"

#define SYSTEM_TIMER_ID	2
static uint32_t ulTimer1usValue;
static uint32_t ulTimer1msValue;

const nrf_drv_timer_t SYSTEM_TIMER =
{
    .p_reg            = NRFX_CONCAT_2(NRF_TIMER, SYSTEM_TIMER_ID),
    .instance_id      = NRFX_CONCAT_3(NRFX_TIMER, SYSTEM_TIMER_ID, _INST_IDX),
    .cc_channel_count = NRF_TIMER_CC_CHANNEL_COUNT(SYSTEM_TIMER_ID),
};

#define SYSTEM_TIMER_CONFIG                                                   		 \
{                                                                                    \
    .frequency          = (nrf_timer_frequency_t)NRF_TIMER_FREQ_16MHz,\
    .mode               = (nrf_timer_mode_t)NRFX_TIMER_DEFAULT_CONFIG_MODE,          \
    .bit_width          = (nrf_timer_bit_width_t)NRF_TIMER_BIT_WIDTH_32,\
    .interrupt_priority = NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY,                    \
    .p_context          = NULL                                                       \
}

void SystimerInit(void)
{
	uint32_t err_code = NRF_SUCCESS;

	nrf_drv_timer_config_t timer_cfg = SYSTEM_TIMER_CONFIG;
	err_code = nrf_drv_timer_init(&SYSTEM_TIMER, &timer_cfg, NULL);
	APP_ERROR_CHECK(err_code);

	nrf_drv_timer_enable(&SYSTEM_TIMER);
}

uint32_t SysTimerGetUsValue(void)
{
	NRF_TIMER2->TASKS_CAPTURE[0] = 1;
	ulTimer1usValue = NRF_TIMER2->CC[0] / 16;
	return ulTimer1usValue;
}

uint32_t SysTimerGetMsValue(void)
{
	NRF_TIMER2->TASKS_CAPTURE[0] = 1;
	ulTimer1msValue = (NRF_TIMER2->CC[0] / 16)/1000;

	return ulTimer1msValue;
}

