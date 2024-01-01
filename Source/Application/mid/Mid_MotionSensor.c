#include "Mid_MotionSensor.h"

#define SAADC_CHANNEL_COUNT   1
#define SAADC_SAMPLE_INTERVAL_MS 100

static volatile bool HandlerIsReady = true;
static volatile bool MotionIsDetect = false;
static uint8_t s_ubMotionCount = 0;
static nrf_saadc_value_t samples[SAADC_CHANNEL_COUNT];
static nrfx_saadc_channel_t channels[SAADC_CHANNEL_COUNT] = {NRFX_SAADC_DEFAULT_CHANNEL_SE(NRF_SAADC_INPUT_AIN3, 0)};

static void Mid_MotionSensorHandler(nrfx_saadc_evt_t const * p_event);
static void Mid_MotionSensorTimerHandler(void * p_context);
static void Mid_MotionSensorTimerInit(void);

void Mid_MotionSensorInit(void)
{
	ret_code_t err_code;
	err_code = nrfx_saadc_init(NRFX_SAADC_CONFIG_IRQ_PRIORITY);
	APP_ERROR_CHECK(err_code);
	err_code = nrfx_saadc_channels_config(channels, SAADC_CHANNEL_COUNT);
	APP_ERROR_CHECK(err_code);
	Mid_MotionSensorTimerInit();
}

bool Mid_MotionSensorIsDetect(void)
{
	return MotionIsDetect;
}

static void Mid_MotionSensorTimerInit(void)
{
    ret_code_t err_code;

    APP_TIMER_DEF(m_sample_timer_id);

    err_code = app_timer_create(&m_sample_timer_id,
                                APP_TIMER_MODE_REPEATED,
								Mid_MotionSensorTimerHandler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_sample_timer_id, APP_TIMER_TICKS(SAADC_SAMPLE_INTERVAL_MS), NULL);
    APP_ERROR_CHECK(err_code);
}

static void Mid_MotionSensorHandler(nrfx_saadc_evt_t const * p_event)
{
    if (p_event->type == NRFX_SAADC_EVT_DONE)
    {
    	if(p_event->data.done.p_buffer[0] < 100)
    	{
    		s_ubMotionCount++;
    		if(s_ubMotionCount >= 8)
    		{
    			MotionIsDetect = true;
    			s_ubMotionCount = 0;
    		}
    	}
    	else
    	{
    		s_ubMotionCount = 0;
    		MotionIsDetect = false;
    	}
    	//NRF_LOG_INFO("ADC = %d", p_event->data.done.p_buffer[0]);
        HandlerIsReady = true;
    }
}

static void Mid_MotionSensorTimerHandler(void * p_context)
{
    if(HandlerIsReady)
    {
        ret_code_t err_code;

        err_code = nrfx_saadc_simple_mode_set((1<<0),
        									  NRF_SAADC_RESOLUTION_14BIT,
                                              NRF_SAADC_OVERSAMPLE_DISABLED,
											  Mid_MotionSensorHandler);
        APP_ERROR_CHECK(err_code);

        err_code = nrfx_saadc_buffer_set(samples, SAADC_CHANNEL_COUNT);
        APP_ERROR_CHECK(err_code);

        err_code = nrfx_saadc_mode_trigger();
        APP_ERROR_CHECK(err_code);

        HandlerIsReady = false;
    }
}

