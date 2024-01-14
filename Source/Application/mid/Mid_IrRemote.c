#include "Mid_IrRemote.h"

#define TIMER_ID	1

const nrf_drv_timer_t TIMER_LED =
{
    .p_reg            = NRFX_CONCAT_2(NRF_TIMER, TIMER_ID),
    .instance_id      = NRFX_CONCAT_3(NRFX_TIMER, TIMER_ID, _INST_IDX),
    .cc_channel_count = NRF_TIMER_CC_CHANNEL_COUNT(TIMER_ID),
};

#define CUSTOM_TIMER_CONFIG                                                   		 \
{                                                                                    \
    .frequency          = (nrf_timer_frequency_t)NRF_TIMER_FREQ_1MHz,\
    .mode               = (nrf_timer_mode_t)NRFX_TIMER_DEFAULT_CONFIG_MODE,          \
    .bit_width          = (nrf_timer_bit_width_t)NRF_TIMER_BIT_WIDTH_32,\
    .interrupt_priority = NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY,                    \
    .p_context          = NULL                                                       \
}

uint16_t periodTimeMicros;
uint16_t periodOnTimeMicros;

void timer_led_event_handler(nrf_timer_event_t event_type, void* p_context)
{
    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:
        {
        	uint8_t tIRInputLevel = nrf_gpio_pin_read(PIN_IR_RECEIVE_CUSTOM);

            /*
             * Increase TickCounter and clip it at maximum 0xFFFF / 3.2 seconds at 50 us ticks
             */
            if (irparams.TickCounterForISR < UINT16_MAX_CUSTOM)
            {
                irparams.TickCounterForISR++;  // One more 50uS tick
            }

            if (irparams.StateForISR == IR_REC_STATE_IDLE)
            {
            	if (tIRInputLevel == INPUT_MARK)
            	{
            		if (irparams.TickCounterForISR > RECORD_GAP_TICKS)
            		{
                        irparams.OverflowFlag = false;
                        irparams.rawbuf[0] = irparams.TickCounterForISR;
                        irparams.rawlen = 1;
                        irparams.StateForISR = IR_REC_STATE_MARK;
            		}
            		irparams.TickCounterForISR = 0; // reset counter in both cases
            	}
            }
            else if (irparams.StateForISR == IR_REC_STATE_MARK)
            {  // Timing mark
				if (tIRInputLevel != INPUT_MARK)
				{
					/*
					 * Mark ended here. Record mark time in rawbuf array
					 */
					irparams.rawbuf[irparams.rawlen++] = irparams.TickCounterForISR; // record mark
					irparams.StateForISR = IR_REC_STATE_SPACE;
					irparams.TickCounterForISR = 0; // This resets the tick counter also at end of frame :-)
				}

			}
            else if (irparams.StateForISR == IR_REC_STATE_SPACE)
            {
				// Timing space
				if (tIRInputLevel == INPUT_MARK)
				{
				  /*
				   * Space ended here. Check for overflow and record space time in rawbuf array
				   */
				  if (irparams.rawlen >= RAW_BUFFER_LENGTH) {
					  // Flag up a read OverflowFlag; Stop the state machine
					  irparams.OverflowFlag = true;
					  irparams.StateForISR = IR_REC_STATE_STOP;
#if !IR_REMOTE_DISABLE_RECEIVE_COMPLETE_CALLBACK
					  /*
					   * Call callback if registered (not NULL)
					   */
					  if (irparams.ReceiveCompleteCallbackFunction != NULL) {
						  irparams.ReceiveCompleteCallbackFunction();
					  }
#endif
				  } else {
#if defined(_IR_MEASURE_TIMING) && defined(_IR_TIMING_TEST_PIN)
				//                digitalWriteFast(_IR_TIMING_TEST_PIN, HIGH); // 2 clock cycles
#endif
					  irparams.rawbuf[irparams.rawlen++] = irparams.TickCounterForISR; // record space
					  irparams.StateForISR = IR_REC_STATE_MARK;
				  }
				  irparams.TickCounterForISR = 0;

				}
				else if (irparams.TickCounterForISR > RECORD_GAP_TICKS)
				{
				  /*
				   * Maximum space duration reached here.
				   * Current code is ready for processing!
				   * We received a long space, which indicates gap between codes.
				   * Switch to IR_REC_STATE_STOP
				   * Don't reset TickCounterForISR; keep counting width of next leading space
				   */
				  irparams.StateForISR = IR_REC_STATE_STOP;
#if !IR_REMOTE_DISABLE_RECEIVE_COMPLETE_CALLBACK
				  /*
				   * Call callback if registered (not NULL)
				   */
				  if (irparams.ReceiveCompleteCallbackFunction != NULL)
				  {
					  irparams.ReceiveCompleteCallbackFunction();
				  }
#endif
				}
			}
            else if (irparams.StateForISR == IR_REC_STATE_STOP)
            {
				/*
				 * Complete command received
				 * stay here until resume() is called, which switches state to IR_REC_STATE_IDLE
				 */
#if defined(_IR_MEASURE_TIMING) && defined(_IR_TIMING_TEST_PIN)
		//        digitalWriteFast(_IR_TIMING_TEST_PIN, HIGH); // 2 clock cycles
#endif
				if (tIRInputLevel == INPUT_MARK) {
					// Reset gap TickCounterForISR, to prepare for detection if we are in the middle of a transmission after call of resume()
					irparams.TickCounterForISR = 0;
				}
			}
        }
		break;

        default:
            break;
    }
}

void Mid_IrRemoteInit(void)
{
	uint32_t time_us = 50; //Time(in us) between consecutive compare events.
	uint32_t time_ticks;
	uint32_t err_code = NRF_SUCCESS;

	nrf_drv_timer_config_t timer_cfg = CUSTOM_TIMER_CONFIG;
	err_code = nrf_drv_timer_init(&TIMER_LED, &timer_cfg, timer_led_event_handler);
	APP_ERROR_CHECK(err_code);

	time_ticks = nrf_drv_timer_us_to_ticks(&TIMER_LED, time_us);

	nrf_drv_timer_extended_compare(&TIMER_LED, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

	nrf_gpio_cfg_input(PIN_IR_RECEIVE_CUSTOM, NRF_GPIO_PIN_NOPULL);

	nrf_gpio_cfg_output(PIN_IR_TRANSMIT);

	Ir_RemoteResume();
}

void Mid_IrRemoteStartScan(void)
{
	NRF_LOG_INFO("Start scan IR signal...");
	irparams.TickCounterForISR = 0;
	g_ubScanIRCount = CLEAR;
	if(!nrfx_timer_is_enabled(&TIMER_LED))
	{
		nrf_drv_timer_enable(&TIMER_LED);
	}
}

void Mid_IrRemoteStopScan(void)
{
	NRF_LOG_INFO("Stop scan IR signal...");
	if(nrfx_timer_is_enabled(&TIMER_LED))
	{
		nrf_drv_timer_disable(&TIMER_LED);
	}
}

bool Mid_IrRemoteDecode(void)
{
    if (irparams.StateForISR != IR_REC_STATE_STOP)
    {
        return false;
    }

    Ir_RemoteInitDecodedIRData();

    if (decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW)
    {
        decodedIRData.protocol = UNKNOWN;
        return true;
    }
    // Attempting NEC/Onkyo decode
    if (decodeNec())
    {
		return true;
	}
    // Attempting Panasonic/Kaseikyo decode
    if (decodeKaseikyo())
    {
        return true;
    }
    // Attempting Denon/Sharp decode
	if (decodeDenon()) {
		return true;
	}
    //Attempting universal Distance Width decode
    if (decodeDistanceWidth())
    {
        return true;
    }

    return true;
}

void sendPulseDistanceWidthFromArray(PulseDistanceWidthProtocolConstants *aProtocolConstants,
        IRRawDataType *aDecodedRawDataArray, uint16_t aNumberOfBits, uint8_t aNumberOfRepeats)
{
	enableIROut(aProtocolConstants->FrequencyKHz);

	uint8_t tNumberOf32Or64BitChunks = ((aNumberOfBits - 1) / BITS_IN_RAW_DATA_TYPE) + 1;

#if defined(LOCAL_DEBUG)
	// fist data
	Serial.print(F("Data[0]=0x"));
	Serial.print(aDecodedRawDataArray[0], HEX);
	if (tNumberOf32Or64BitChunks > 1) {
		Serial.print(F(" Data[1]=0x"));
		Serial.print(aDecodedRawDataArray[1], HEX);
	}
	Serial.print(F(" #="));
	Serial.println(aNumberOfBits);
	Serial.flush();
#endif

	uint8_t tNumberOfCommands = aNumberOfRepeats + 1;
	while (tNumberOfCommands > 0) {
		uint32_t tStartOfFrameMillis = SysTimerGetMsValue();
		uint16_t tNumberOfBits = aNumberOfBits; // refresh value for repeats

		// Header
		mark(aProtocolConstants->DistanceWidthTimingInfo.HeaderMarkMicros);
		space(aProtocolConstants->DistanceWidthTimingInfo.HeaderSpaceMicros);
		uint8_t tOriginalFlags = aProtocolConstants->Flags;

		for (uint8_t i = 0; i < tNumberOf32Or64BitChunks; ++i)
		{
			uint8_t tNumberOfBitsForOneSend;

			uint8_t tFlags;
			if (i == (tNumberOf32Or64BitChunks - 1)) {
				// End of data
				tNumberOfBitsForOneSend = tNumberOfBits;
				tFlags = tOriginalFlags;
			} else {
				// intermediate data
				tNumberOfBitsForOneSend = BITS_IN_RAW_DATA_TYPE;
				tFlags = tOriginalFlags | SUPPRESS_STOP_BIT_FOR_THIS_DATA; // No stop bit for leading data
			}

			sendPulseDistanceWidthDataV1(aProtocolConstants->DistanceWidthTimingInfo.OneMarkMicros,
					aProtocolConstants->DistanceWidthTimingInfo.OneSpaceMicros,
					aProtocolConstants->DistanceWidthTimingInfo.ZeroMarkMicros,
					aProtocolConstants->DistanceWidthTimingInfo.ZeroSpaceMicros, aDecodedRawDataArray[i], tNumberOfBitsForOneSend,
					tFlags);
			tNumberOfBits -= BITS_IN_RAW_DATA_TYPE;
		}

		tNumberOfCommands--;
		// skip last delay!
		if (tNumberOfCommands > 0) {
			/*
			 * Check and fallback for wrong RepeatPeriodMillis parameter. I.e the repeat period must be greater than each frame duration.
			 */
			uint32_t tFrameDurationMillis = SysTimerGetMsValue() - tStartOfFrameMillis;
			if (aProtocolConstants->RepeatPeriodMillis > tFrameDurationMillis)
			{
				nrf_delay_ms(aProtocolConstants->RepeatPeriodMillis - tFrameDurationMillis);
			}
		}
	}
}

/**
 * Sends PulseDistance data from array
 * For LSB First the LSB of array[0] is sent first then all bits until MSB of array[0]. Next is LSB of array[1] and so on.
 * The output always ends with a space
 * Stop bit is always sent
 */
void sendPulseDistanceWidthFromArrayV1(uint8_t aFrequencyKHz, uint16_t aHeaderMarkMicros, uint16_t aHeaderSpaceMicros,
        uint16_t aOneMarkMicros, uint16_t aOneSpaceMicros, uint16_t aZeroMarkMicros, uint16_t aZeroSpaceMicros,
        IRRawDataType *aDecodedRawDataArray, uint16_t aNumberOfBits, bool aMSBFirst, bool aSendStopBit,
        uint16_t aRepeatPeriodMillis, uint8_t aNumberOfRepeats)
{
    uint8_t tFlags = 0;
    if (aMSBFirst) {
        tFlags = PROTOCOL_IS_MSB_FIRST;
    }
    (void) aSendStopBit;

    sendPulseDistanceWidthFromArrayV3(aFrequencyKHz, aHeaderMarkMicros, aHeaderSpaceMicros, aOneMarkMicros, aOneSpaceMicros,
            aZeroMarkMicros, aZeroSpaceMicros, aDecodedRawDataArray, aNumberOfBits, tFlags, aRepeatPeriodMillis, aNumberOfRepeats);
}

void sendPulseDistanceWidthFromArrayV2(uint8_t aFrequencyKHz, struct DistanceWidthTimingInfoStruct *aDistanceWidthTimingInfo,
        IRRawDataType *aDecodedRawDataArray, uint16_t aNumberOfBits, uint8_t aFlags, uint16_t aRepeatPeriodMillis,
		uint8_t aNumberOfRepeats)
{
    sendPulseDistanceWidthFromArrayV3(aFrequencyKHz, aDistanceWidthTimingInfo->HeaderMarkMicros,
            aDistanceWidthTimingInfo->HeaderSpaceMicros, aDistanceWidthTimingInfo->OneMarkMicros,
            aDistanceWidthTimingInfo->OneSpaceMicros, aDistanceWidthTimingInfo->ZeroMarkMicros,
            aDistanceWidthTimingInfo->ZeroSpaceMicros, aDecodedRawDataArray, aNumberOfBits, aFlags, aRepeatPeriodMillis,
            aNumberOfRepeats);
}

void sendPulseDistanceWidthFromArrayV3(uint8_t aFrequencyKHz, uint16_t aHeaderMarkMicros, uint16_t aHeaderSpaceMicros,
        uint16_t aOneMarkMicros, uint16_t aOneSpaceMicros, uint16_t aZeroMarkMicros, uint16_t aZeroSpaceMicros,
        IRRawDataType *aDecodedRawDataArray, uint16_t aNumberOfBits, uint8_t aFlags, uint16_t aRepeatPeriodMillis,
		uint8_t aNumberOfRepeats)
{

    // Set IR carrier frequency
    enableIROut(aFrequencyKHz);

    uint8_t tNumberOfCommands = aNumberOfRepeats + 1;
    uint8_t tNumberOf32Or64BitChunks = ((aNumberOfBits - 1) / BITS_IN_RAW_DATA_TYPE) + 1;

#if defined(LOCAL_DEBUG)
    // fist data
    Serial.print(F("Data[0]=0x"));
    Serial.print(aDecodedRawDataArray[0], HEX);
    if (tNumberOf32Or64BitChunks > 1) {
        Serial.print(F(" Data[1]=0x"));
        Serial.print(aDecodedRawDataArray[1], HEX);
    }
    Serial.print(F(" #="));
    Serial.println(aNumberOfBits);
    Serial.flush();
#endif

    while (tNumberOfCommands > 0)
    {
        unsigned long tStartOfFrameMillis = SysTimerGetMsValue();

        // Header
        mark(aHeaderMarkMicros);
        space(aHeaderSpaceMicros);

        for (uint_fast8_t i = 0; i < tNumberOf32Or64BitChunks; ++i) {
            uint8_t tNumberOfBitsForOneSend;

            // Manage stop bit
            uint8_t tFlags;
            if (i == (tNumberOf32Or64BitChunks - 1)) {
                // End of data
                tNumberOfBitsForOneSend = aNumberOfBits;
                tFlags = aFlags;
            } else {
                // intermediate data
                tNumberOfBitsForOneSend = BITS_IN_RAW_DATA_TYPE;
                tFlags = aFlags | SUPPRESS_STOP_BIT_FOR_THIS_DATA; // No stop bit for leading data
            }

            sendPulseDistanceWidthDataV1(aOneMarkMicros, aOneSpaceMicros, aZeroMarkMicros, aZeroSpaceMicros, aDecodedRawDataArray[i],
                    tNumberOfBitsForOneSend, tFlags);
            aNumberOfBits -= BITS_IN_RAW_DATA_TYPE;
        }

        tNumberOfCommands--;
        // skip last delay!
        if (tNumberOfCommands > 0) {
            /*
             * Check and fallback for wrong RepeatPeriodMillis parameter. I.e the repeat period must be greater than each frame duration.
             */
            uint32_t tFrameDurationMillis = SysTimerGetMsValue() - tStartOfFrameMillis;
            if (aRepeatPeriodMillis > tFrameDurationMillis)
            {
            	nrf_delay_ms(aRepeatPeriodMillis - tFrameDurationMillis);
            }
        }
    }
}

/**
 * Sends PulseDistance frames and repeats and enables receiver again
 * @param aProtocolConstants    The constants to use for sending this protocol.
 * @param aData             uint32 or uint64 holding the bits to be sent.
 * @param aNumberOfBits     Number of bits from aData to be actually sent.
 * @param aNumberOfRepeats  If < 0 and a aProtocolConstants->SpecialSendRepeatFunction() is specified
 *                          then it is called without leading and trailing space.
 */
void sendPulseDistanceWidthV1(PulseDistanceWidthProtocolConstants *aProtocolConstants, IRRawDataType aData,
        uint8_t aNumberOfBits, uint8_t aNumberOfRepeats)
{

#if defined(LOCAL_DEBUG)
    Serial.print(F("Data=0x"));
    Serial.print(aData, HEX);
    Serial.print(F(" #="));
    Serial.println(aNumberOfBits);
    Serial.flush();
#endif

    if (aNumberOfRepeats < 0) {
        if (aProtocolConstants->SpecialSendRepeatFunction != NULL) {
            aProtocolConstants->SpecialSendRepeatFunction();
            return;
        } else {
            aNumberOfRepeats = 0; // send a plain frame as repeat
        }
    }

    // Set IR carrier frequency
    enableIROut(aProtocolConstants->FrequencyKHz);

    uint_fast8_t tNumberOfCommands = aNumberOfRepeats + 1;
    while (tNumberOfCommands > 0) {
        unsigned long tStartOfFrameMillis = SysTimerGetMsValue();

        if (tNumberOfCommands < ((uint8_t) aNumberOfRepeats + 1) && aProtocolConstants->SpecialSendRepeatFunction != NULL) {
            // send special repeat
            aProtocolConstants->SpecialSendRepeatFunction();
        } else {
            /*
             * Send Header and regular frame
             */
            mark(aProtocolConstants->DistanceWidthTimingInfo.HeaderMarkMicros);
            space(aProtocolConstants->DistanceWidthTimingInfo.HeaderSpaceMicros);
            sendPulseDistanceWidthDataV3(aProtocolConstants, aData, aNumberOfBits);
        }

        tNumberOfCommands--;
        // skip last delay!
        if (tNumberOfCommands > 0) {
            /*
             * Check and fallback for wrong RepeatPeriodMillis parameter. I.e the repeat period must be greater than each frame duration.
             */
            uint32_t tFrameDurationMillis = SysTimerGetMsValue() - tStartOfFrameMillis;
            if (aProtocolConstants->RepeatPeriodMillis > tFrameDurationMillis)
            {
                nrf_delay_ms(aProtocolConstants->RepeatPeriodMillis - tFrameDurationMillis);
            }
        }
    }
}

/**
 * Sends PulseDistance frames and repeats.
 * @param aFrequencyKHz, aHeaderMarkMicros, aHeaderSpaceMicros, aOneMarkMicros, aOneSpaceMicros, aZeroMarkMicros, aZeroSpaceMicros, aFlags, aRepeatPeriodMillis     Values to use for sending this protocol, also contained in the PulseDistanceWidthProtocolConstants of this protocol.
 * @param aData             uint32 or uint64 holding the bits to be sent.
 * @param aNumberOfBits     Number of bits from aData to be actually sent.
 * @param aNumberOfRepeats  If < 0 and a aProtocolConstants->SpecialSendRepeatFunction() is specified
 *                          then it is called without leading and trailing space.
 * @param aSpecialSendRepeatFunction    If NULL, the first frame is repeated completely, otherwise this function is used for sending the repeat frame.
 */
void sendPulseDistanceWidthV2(uint8_t aFrequencyKHz, uint16_t aHeaderMarkMicros, uint16_t aHeaderSpaceMicros,
        uint16_t aOneMarkMicros, uint16_t aOneSpaceMicros, uint16_t aZeroMarkMicros, uint16_t aZeroSpaceMicros, IRRawDataType aData,
		uint8_t aNumberOfBits, bool aMSBFirst, bool aSendStopBit, uint16_t aRepeatPeriodMillis, int_fast8_t aNumberOfRepeats,
        void (*aSpecialSendRepeatFunction)())
{
    uint8_t tFlags = 0;
    if (aMSBFirst) {
        tFlags = PROTOCOL_IS_MSB_FIRST;
    }
    (void) aSendStopBit;
    sendPulseDistanceWidthV3(aFrequencyKHz, aHeaderMarkMicros, aHeaderSpaceMicros, aOneMarkMicros, aOneSpaceMicros, aZeroMarkMicros,
            aZeroSpaceMicros, aData, aNumberOfBits, tFlags, aRepeatPeriodMillis, aNumberOfRepeats, aSpecialSendRepeatFunction);

}
void sendPulseDistanceWidthV3(uint8_t aFrequencyKHz, uint16_t aHeaderMarkMicros, uint16_t aHeaderSpaceMicros,
        uint16_t aOneMarkMicros, uint16_t aOneSpaceMicros, uint16_t aZeroMarkMicros, uint16_t aZeroSpaceMicros, IRRawDataType aData,
		uint8_t aNumberOfBits, uint8_t aFlags, uint16_t aRepeatPeriodMillis, uint8_t aNumberOfRepeats,
        void (*aSpecialSendRepeatFunction)())
{

    if (aNumberOfRepeats < 0) {
        if (aSpecialSendRepeatFunction != NULL) {
            aSpecialSendRepeatFunction();
            return;
        } else {
            aNumberOfRepeats = 0; // send a plain frame as repeat
        }
    }

    // Set IR carrier frequency
    enableIROut(aFrequencyKHz);

    uint_fast8_t tNumberOfCommands = aNumberOfRepeats + 1;
    while (tNumberOfCommands > 0) {
        uint32_t tStartOfFrameMillis = SysTimerGetMsValue();

        if (tNumberOfCommands < ((uint8_t) aNumberOfRepeats + 1) && aSpecialSendRepeatFunction != NULL) {
            // send special repeat
            aSpecialSendRepeatFunction();
        } else {
            // Header and regular frame
            mark(aHeaderMarkMicros);
            space(aHeaderSpaceMicros);
            sendPulseDistanceWidthDataV1(aOneMarkMicros, aOneSpaceMicros, aZeroMarkMicros, aZeroSpaceMicros, aData, aNumberOfBits,
                    aFlags);
        }

        tNumberOfCommands--;
        // skip last delay!
        if (tNumberOfCommands > 0) {
            /*
             * Check and fallback for wrong RepeatPeriodMillis parameter. I.e the repeat period must be greater than each frame duration.
             */
            uint32_t tFrameDurationMillis = SysTimerGetMsValue() - tStartOfFrameMillis;
            if (aRepeatPeriodMillis > tFrameDurationMillis)
            {
                nrf_delay_ms(aRepeatPeriodMillis - tFrameDurationMillis);
            }
        }
    }
}

/**
 * Sends PulseDistance data
 * The output always ends with a space
 * Each additional call costs 16 bytes program memory
 */
void sendPulseDistanceWidthDataV3(PulseDistanceWidthProtocolConstants *aProtocolConstants, IRRawDataType aData,
        uint8_t aNumberOfBits)
{
    sendPulseDistanceWidthDataV1(aProtocolConstants->DistanceWidthTimingInfo.OneMarkMicros,
            aProtocolConstants->DistanceWidthTimingInfo.OneSpaceMicros, aProtocolConstants->DistanceWidthTimingInfo.ZeroMarkMicros,
            aProtocolConstants->DistanceWidthTimingInfo.ZeroSpaceMicros, aData, aNumberOfBits, aProtocolConstants->Flags);
}

/**
 * Sends PulseDistance data
 * The output always ends with a space
 */
void sendPulseDistanceWidthDataV2(uint16_t aOneMarkMicros, uint16_t aOneSpaceMicros, uint16_t aZeroMarkMicros,
        uint16_t aZeroSpaceMicros, IRRawDataType aData, uint8_t aNumberOfBits, bool aMSBFirst, bool aSendStopBit)
{
    uint8_t tFlags = 0;
    if (aMSBFirst) {
        tFlags = PROTOCOL_IS_MSB_FIRST;
    }
    (void) aSendStopBit;
    sendPulseDistanceWidthDataV1(aOneMarkMicros, aOneSpaceMicros, aZeroMarkMicros, aZeroSpaceMicros, aData, aNumberOfBits, tFlags);
}
void sendPulseDistanceWidthDataV1(uint16_t aOneMarkMicros, uint16_t aOneSpaceMicros, uint16_t aZeroMarkMicros,
        uint16_t aZeroSpaceMicros, IRRawDataType aData, uint8_t aNumberOfBits, uint8_t aFlags)
{

#if defined(LOCAL_DEBUG)
    Serial.print(aData, HEX);
    Serial.print('|');
    Serial.println(aNumberOfBits);
    Serial.flush();
#endif

    // For MSBFirst, send data from MSB to LSB until mask bit is shifted out
    IRRawDataType tMask = 1ULL << (aNumberOfBits - 1);
    for (uint_fast8_t i = aNumberOfBits; i > 0; i--) {
        if (((aFlags & PROTOCOL_IS_MSB_FIRST) && (aData & tMask)) || (!(aFlags & PROTOCOL_IS_MSB_FIRST) && (aData & 1))) {
#if defined(LOCAL_TRACE)
            Serial.print('1');
#endif
            mark(aOneMarkMicros);
            space(aOneSpaceMicros);
        } else {
#if defined(LOCAL_TRACE)
            Serial.print('0');
#endif
            mark(aZeroMarkMicros);
            space(aZeroSpaceMicros);
        }
        if (aFlags & PROTOCOL_IS_MSB_FIRST) {
            tMask >>= 1;
        } else {
            aData >>= 1;
        }
    }
    /*
     * Stop bit is sent for all pulse distance protocols i.e. aOneMarkMicros == aZeroMarkMicros.
     * Therefore it is not sent for Sony and Magiquest :-)
     */
    if (!(aFlags & SUPPRESS_STOP_BIT_FOR_THIS_DATA) && aOneMarkMicros == aZeroMarkMicros) {
        // Send stop bit here
#if defined(LOCAL_TRACE)
        Serial.print('S');
#endif
        mark(aZeroMarkMicros); // Use aZeroMarkMicros for stop bits. This seems to be correct for all protocols :-)
    }
#if defined(LOCAL_TRACE)
    Serial.println();
#endif
}

void Ir_RemoteInitDecodedIRData(void)
{
    if (irparams.OverflowFlag) {
        decodedIRData.flags = IRDATA_FLAGS_WAS_OVERFLOW;
#if defined(LOCAL_DEBUG)
        Serial.print(F("Overflow happened, try to increase the \"RAW_BUFFER_LENGTH\" value of "));
        Serial.print(RAW_BUFFER_LENGTH);
        Serial.println(F(" with #define RAW_BUFFER_LENGTH=<biggerValue>"));
#endif

    } else {
        decodedIRData.flags = IRDATA_FLAGS_EMPTY;
        // save last protocol, command and address for repeat handling (where they are compared or copied back :-))
        lastDecodedProtocol = decodedIRData.protocol; // repeat patterns can be equal between protocols (e.g. NEC and LG), so we must keep the original one
        lastDecodedCommand = decodedIRData.command;
        lastDecodedAddress = decodedIRData.address;

    }
    decodedIRData.protocol = UNKNOWN;
    decodedIRData.command = 0;
    decodedIRData.address = 0;
    decodedIRData.decodedRawData = 0;
    decodedIRData.numberOfBits = 0;

    decodedIRData.rawDataPtr = &irparams;
}

void Ir_RemoteResume(void)
{
    if (irparams.StateForISR == IR_REC_STATE_STOP)
    {
        irparams.StateForISR = IR_REC_STATE_IDLE;
    }
}

void enableIROut(uint_fast8_t aFrequencyKHz)
{
#if defined(SEND_PWM_BY_TIMER)
    timerConfigForSend(aFrequencyKHz); // must set output pin mode and disable receive interrupt if required, e.g. uses the same resource

#elif defined(USE_NO_SEND_PWM)
    (void) aFrequencyKHz;

#else
    periodTimeMicros = (1000U + (aFrequencyKHz / 2)) / aFrequencyKHz; // rounded value -> 26 for 38.46 kHz, 27 for 37.04 kHz, 25 for 40 kHz.
#  if defined(IR_SEND_PIN)
    periodOnTimeMicros = (((periodTimeMicros * IR_SEND_DUTY_CYCLE_PERCENT) + 50) / 100U); // +50 for rounding -> 830/100 for 30% and 16 MHz
#  else
// Heuristics! We require a nanosecond correction for "slow" digitalWrite() functions
    periodOnTimeMicros = (((periodTimeMicros * IR_SEND_DUTY_CYCLE_PERCENT) + 50 - (PULSE_CORRECTION_NANOS / 10)) / 100U); // +50 for rounding -> 530/100 for 30% and 16 MHz
#  endif
#endif // defined(SEND_PWM_BY_TIMER)

#if defined(USE_OPEN_DRAIN_OUTPUT_FOR_SEND_PIN) && defined(OUTPUT_OPEN_DRAIN) // the mode INPUT for mimicking open drain is set at IRLedOff()
#  if defined(IR_SEND_PIN)
    pinModeFast(IR_SEND_PIN, OUTPUT_OPEN_DRAIN);
#  else
    pinModeFast(sendPin, OUTPUT_OPEN_DRAIN);
#  endif
#else

// For Non AVR platforms pin mode for SEND_PWM_BY_TIMER must be handled by the timerConfigForSend() function
// because ESP 2.0.2 ledcWrite does not work if pin mode is set, and RP2040 requires gpio_set_function(IR_SEND_PIN, GPIO_FUNC_PWM);
#  if defined(__AVR__) || !defined(SEND_PWM_BY_TIMER)
#    if defined(IR_SEND_PIN)
    pinModeFast(IR_SEND_PIN, OUTPUT);
#    else
//    pinModeFast(sendPin, OUTPUT);
#    endif
#  endif
#endif // defined(USE_OPEN_DRAIN_OUTPUT_FOR_SEND_PIN)
}

/**
 * Sends an IR mark for the specified number of microseconds.
 * The mark output is modulated at the PWM frequency if USE_NO_SEND_PWM is not defined.
 * The output is guaranteed to be OFF / inactive after after the call of the function.
 * This function may affect the state of feedback LED.
 * Period time is 26 us for 38.46 kHz, 27 us for 37.04 kHz, 25 us for 40 kHz.
 * On time is 8 us for 30% duty cycle
 */
void mark(uint16_t aMarkMicros)
{

#if defined(SEND_PWM_BY_TIMER) || defined(USE_NO_SEND_PWM)
#  if !defined(NO_LED_FEEDBACK_CODE)
    if (FeedbackLEDControl.LedFeedbackEnabled == LED_FEEDBACK_ENABLED_FOR_SEND) {
        setFeedbackLED(true);
    }
#  endif
#endif

#if defined(SEND_PWM_BY_TIMER)
    /*
     * Generate hardware PWM signal
     */
    enableSendPWMByTimer(); // Enable timer or ledcWrite() generated PWM output
    customDelayMicroseconds(aMarkMicros);
    IRLedOff(); // disables hardware PWM and manages feedback LED
    return;

#elif defined(USE_NO_SEND_PWM)
    /*
     * Here we generate no carrier PWM, just simulate an active low receiver signal.
     */
#  if defined(USE_OPEN_DRAIN_OUTPUT_FOR_SEND_PIN) && !defined(OUTPUT_OPEN_DRAIN)
    pinModeFast(sendPin, OUTPUT); // active state for mimicking open drain
#  else
    digitalWriteFast(sendPin, LOW); // Set output to active low.
#  endif

    customDelayMicroseconds(aMarkMicros);
    IRLedOff();
#  if !defined(NO_LED_FEEDBACK_CODE)
    if (FeedbackLEDControl.LedFeedbackEnabled == LED_FEEDBACK_ENABLED_FOR_SEND) {
        setFeedbackLED(false);
    }
    return;
#  endif

#else // defined(SEND_PWM_BY_TIMER)
    /*
     * Generate PWM by bit banging
     */
    uint32_t tStartMicros = SysTimerGetUsValue();
    uint32_t tNextPeriodEnding = tStartMicros;
    uint32_t tMicros;
#  if !defined(NO_LED_FEEDBACK_CODE)
    bool FeedbackLedIsActive = false;
#  endif

    do {
//        digitalToggleFast(_IR_TIMING_TEST_PIN);
        /*
         * Output the PWM pulse
         */
//        noInterrupts(); // do not let interrupts extend the short on period
#  if defined(USE_OPEN_DRAIN_OUTPUT_FOR_SEND_PIN)
#    if defined(OUTPUT_OPEN_DRAIN)
        digitalWriteFast(sendPin, LOW); // set output with pin mode OUTPUT_OPEN_DRAIN to active low
#    else
        pinModeFast(sendPin, OUTPUT); // active state for mimicking open drain
#    endif
#  else
        // 3.5 us from FeedbackLed on to pin setting. 5.7 us from call of mark() to pin setting incl. setting of feedback pin.
        // 4.3 us from do{ to pin setting if sendPin is no constant
        //digitalWriteFast(sendPin, HIGH);
        nrf_gpio_pin_set(PIN_IR_TRANSMIT);
#  endif
        nrf_delay_us(periodOnTimeMicros); // On time is 8 us for 30% duty cycle. This is normally implemented by a blocking wait.

        /*
         * Output the PWM pause
         */
#  if defined(USE_OPEN_DRAIN_OUTPUT_FOR_SEND_PIN) && !defined(OUTPUT_OPEN_DRAIN)
#    if defined(OUTPUT_OPEN_DRAIN)
        digitalWriteFast(sendPin, HIGH); // Set output with pin mode OUTPUT_OPEN_DRAIN to inactive high.
#    else
        pinModeFast(sendPin, INPUT); // to mimic the open drain inactive state
#    endif

#  else
        //digitalWriteFast(sendPin, LOW);
        nrf_gpio_pin_clear(PIN_IR_TRANSMIT);
#  endif
        /*
         * Enable interrupts at start of the longer off period. Required at least to keep micros correct.
         * If receive interrupt is still active, it takes 3.4 us from now until receive ISR is active (for 7 us + pop's)
         */
//        interrupts();

#  if !defined(NO_LED_FEEDBACK_CODE)
        /*
         * Delayed call of setFeedbackLED() to get better startup timing, especially required for consecutive marks
         */
        if (!FeedbackLedIsActive) {
            FeedbackLedIsActive = true;
            if (FeedbackLEDControl.LedFeedbackEnabled == LED_FEEDBACK_ENABLED_FOR_SEND) {
                setFeedbackLED(true);
            }
        }
#  endif
        /*
         * PWM pause timing
         * Measured delta between pause duration values are 13 us for a 16 MHz Uno (from 13 to 26), if interrupts are disabled below
         * Measured delta between pause duration values are 20 us for a 16 MHz Uno (from 7.8 to 28), if interrupts are not disabled below
         * Minimal pause duration is 5.2 us with NO_LED_FEEDBACK_CODE enabled
         * and 8.1 us with NO_LED_FEEDBACK_CODE disabled.
         */
        tNextPeriodEnding += periodTimeMicros;
#if defined(__AVR__) // micros() for STM sometimes give decreasing values if interrupts are disabled. See https://github.com/stm32duino/Arduino_Core_STM32/issues/1680
        noInterrupts(); // disable interrupts (especially the 20 us receive interrupts) only at start of the PWM pause. Otherwise it may extend the pause too much.
#endif
        do {
#if defined(_IR_MEASURE_TIMING) && defined(_IR_TIMING_TEST_PIN)
            digitalWriteFast(_IR_TIMING_TEST_PIN, HIGH); // 2 clock cycles
#endif
            /*
             * For AVR @16MHz we have only 4 us resolution.
             * The duration of the micros() call itself is 3 us.
             * It takes 0.9 us from signal going low here.
             * The rest of the loop takes 1.2 us with NO_LED_FEEDBACK_CODE enabled
             * and 3 us with NO_LED_FEEDBACK_CODE disabled.
             */
#if defined(_IR_MEASURE_TIMING) && defined(_IR_TIMING_TEST_PIN)
            digitalWriteFast(_IR_TIMING_TEST_PIN, LOW); // 2 clock cycles
#endif
            /*
             * Exit the forever loop if aMarkMicros has reached
             */
            tMicros = SysTimerGetUsValue();
            uint16_t tDeltaMicros = tMicros - tStartMicros;
#if defined(__AVR__)
            // reset feedback led in the last pause before end
//            tDeltaMicros += (160 / CLOCKS_PER_MICRO); // adding this once increases program size, so do it below !
#  if !defined(NO_LED_FEEDBACK_CODE)
            if (tDeltaMicros >= aMarkMicros - (30 + (112 / CLOCKS_PER_MICRO))) { // 30 to be constant. Using periodTimeMicros increases program size too much.
                if (FeedbackLEDControl.LedFeedbackEnabled == LED_FEEDBACK_ENABLED_FOR_SEND) {
                    setFeedbackLED(false);
                }
            }
#  endif
            // Just getting variables and check for end condition takes minimal 3.8 us
            if (tDeltaMicros >= aMarkMicros - (112 / CLOCKS_PER_MICRO)) { // To compensate for call duration - 112 is an empirical value
#else
            if (tDeltaMicros >= aMarkMicros) {
#  if !defined(NO_LED_FEEDBACK_CODE)
                if (FeedbackLEDControl.LedFeedbackEnabled == LED_FEEDBACK_ENABLED_FOR_SEND) {
                    setFeedbackLED(false);
                }
#  endif
#endif
#if defined(__AVR__)
                interrupts();
#endif
                return;
            }
        } while (tMicros < tNextPeriodEnding);
    } while (true);
#  endif
}

void space(uint16_t aSpaceMicros)
{
	customDelayMicroseconds(aSpaceMicros);
}

void customDelayMicroseconds(unsigned long aMicroseconds)
{
#if defined(ESP32) || defined(ESP8266)
    // from https://github.com/crankyoldgit/IRremoteESP8266/blob/00b27cc7ea2e7ac1e48e91740723c805a38728e0/src/IRsend.cpp#L123
    // Invoke a delay(), where possible, to avoid triggering the WDT.
    // see https://github.com/Arduino-IRremote/Arduino-IRremote/issues/1114 for the reason of checking for > 16383)
    // delayMicroseconds() is only accurate to 16383 us. Ref: https://www.arduino.cc/en/Reference/delayMicroseconds
    if (aMicroseconds > 16383) {
        delay(aMicroseconds / 1000UL);  // Delay for as many whole milliseconds as we can.
        // Delay the remaining sub-millisecond.
        delayMicroseconds(static_cast<uint16_t>(aMicroseconds % 1000UL));
    } else {
        delayMicroseconds(aMicroseconds);
    }
#else

#  if defined(__AVR__)
    unsigned long start = micros() - (64 / clockCyclesPerMicrosecond()); // - (64 / clockCyclesPerMicrosecond()) for reduced resolution and additional overhead
#  else
    unsigned long start = SysTimerGetUsValue();
#  endif
// overflow invariant comparison :-)
    while (SysTimerGetUsValue() - start < aMicroseconds) {
    }
#endif
}

bool checkHeader(PulseDistanceWidthProtocolConstants *aProtocolConstants)
{
// Check header "mark" and "space"
    if (!matchMark(decodedIRData.rawDataPtr->rawbuf[1], aProtocolConstants->DistanceWidthTimingInfo.HeaderMarkMicros)) {
#if defined(LOCAL_TRACE)
        Serial.print(::getProtocolString(aProtocolConstants->ProtocolIndex));
        Serial.println(F(": Header mark length is wrong"));
#endif
        return false;
    }
    if (!matchSpace(decodedIRData.rawDataPtr->rawbuf[2], aProtocolConstants->DistanceWidthTimingInfo.HeaderSpaceMicros)) {
#if defined(LOCAL_TRACE)
        Serial.print(::getProtocolString(aProtocolConstants->ProtocolIndex));
        Serial.println(F(": Header space length is wrong"));
#endif
        return false;
    }
    return true;
}

/**
 * Match function without compensating for marks exceeded or spaces shortened by demodulator hardware
 * Currently not used
 */
bool matchTicks(uint16_t aMeasuredTicks, uint16_t aMatchValueMicros) {
#if defined(LOCAL_TRACE)
    Serial.print(F("Testing: "));
    Serial.print(TICKS_LOW(aMatchValueMicros), DEC);
    Serial.print(F(" <= "));
    Serial.print(aMeasuredTicks, DEC);
    Serial.print(F(" <= "));
    Serial.print(TICKS_HIGH(aMatchValueMicros), DEC);
#endif
    bool passed = ((aMeasuredTicks >= TICKS_LOW(aMatchValueMicros)) && (aMeasuredTicks <= TICKS_HIGH(aMatchValueMicros)));
#if defined(LOCAL_TRACE)
    if (passed) {
        Serial.println(F(" => passed"));
    } else {
        Serial.println(F(" => FAILED"));
    }
#endif
    return passed;
}

bool MATCH(uint16_t measured_ticks, uint16_t desired_us) {
    return matchTicks(measured_ticks, desired_us);
}

bool matchMark(uint16_t aMeasuredTicks, uint16_t aMatchValueMicros) {
#if defined(LOCAL_TRACE)
    Serial.print(F("Testing mark (actual vs desired): "));
    Serial.print(aMeasuredTicks * MICROS_PER_TICK, DEC);
    Serial.print(F("us vs "));
    Serial.print(aMatchValueMicros, DEC);
    Serial.print(F("us: "));
    Serial.print(TICKS_LOW(aMatchValueMicros + MARK_EXCESS_MICROS) * MICROS_PER_TICK, DEC);
    Serial.print(F(" <= "));
    Serial.print(aMeasuredTicks * MICROS_PER_TICK, DEC);
    Serial.print(F(" <= "));
    Serial.print(TICKS_HIGH(aMatchValueMicros + MARK_EXCESS_MICROS) * MICROS_PER_TICK, DEC);
#endif
    // compensate for marks exceeded by demodulator hardware
    bool passed = ((aMeasuredTicks >= TICKS_LOW(aMatchValueMicros + MARK_EXCESS_MICROS))
            && (aMeasuredTicks <= TICKS_HIGH(aMatchValueMicros + MARK_EXCESS_MICROS)));
#if defined(LOCAL_TRACE)
    if (passed) {
        Serial.println(F(" => passed"));
    } else {
        Serial.println(F(" => FAILED"));
    }
#endif
    return passed;
}

bool MATCH_MARK(uint16_t measured_ticks, uint16_t desired_us) {
    return matchMark(measured_ticks, desired_us);
}

/**
 * Compensate for spaces shortened by demodulator hardware
 */
bool matchSpace(uint16_t aMeasuredTicks, uint16_t aMatchValueMicros) {
#if defined(LOCAL_TRACE)
    Serial.print(F("Testing space (actual vs desired): "));
    Serial.print(aMeasuredTicks * MICROS_PER_TICK, DEC);
    Serial.print(F("us vs "));
    Serial.print(aMatchValueMicros, DEC);
    Serial.print(F("us: "));
    Serial.print(TICKS_LOW(aMatchValueMicros - MARK_EXCESS_MICROS) * MICROS_PER_TICK, DEC);
    Serial.print(F(" <= "));
    Serial.print(aMeasuredTicks * MICROS_PER_TICK, DEC);
    Serial.print(F(" <= "));
    Serial.print(TICKS_HIGH(aMatchValueMicros - MARK_EXCESS_MICROS) * MICROS_PER_TICK, DEC);
#endif
    // compensate for spaces shortened by demodulator hardware
    bool passed = ((aMeasuredTicks >= TICKS_LOW(aMatchValueMicros - MARK_EXCESS_MICROS))
            && (aMeasuredTicks <= TICKS_HIGH(aMatchValueMicros - MARK_EXCESS_MICROS)));
#if defined(LOCAL_TRACE)
    if (passed) {
        Serial.println(F(" => passed"));
    } else {
        Serial.println(F(" => FAILED"));
    }
#endif
    return passed;
}

bool MATCH_SPACE(uint16_t measured_ticks, uint16_t desired_us) {
    return matchSpace(measured_ticks, desired_us);
}

/**
 * Getter function for MARK_EXCESS_MICROS
 */
int getMarkExcessMicros() {
    return MARK_EXCESS_MICROS;
}

void checkForRepeatSpaceTicksAndSetFlag(uint16_t aMaximumRepeatSpaceTicks)
{
    if (decodedIRData.rawDataPtr->rawbuf[0] < aMaximumRepeatSpaceTicks
#if defined(ENABLE_FULL_REPEAT_CHECK)
            && decodedIRData.address == lastDecodedAddress && decodedIRData.command == lastDecodedCommand /* requires around 85 bytes program space */
#endif
            ) {
        decodedIRData.flags |= IRDATA_FLAGS_IS_REPEAT;
    }
}

bool decodePulseDistanceWidthData(uint8_t aNumberOfBits, uint8_t aStartOffset, uint16_t aOneMarkMicros,
        uint16_t aZeroMarkMicros, uint16_t aOneSpaceMicros, uint16_t aZeroSpaceMicros, bool aMSBfirst)
{

	uint16_t *tRawBufPointer = &decodedIRData.rawDataPtr->rawbuf[aStartOffset];

    bool isPulseDistanceProtocol = (aOneMarkMicros == aZeroMarkMicros); // If true, we have a constant mark -> pulse distance protocol

    IRRawDataType tDecodedData = 0; // For MSB first tDecodedData is shifted left each loop
    IRRawDataType tMask = 1UL; // Mask is only used for LSB first

    for (uint_fast8_t i = aNumberOfBits; i > 0; i--) {
        // get one mark and space pair
        unsigned int tMarkTicks;
        unsigned int tSpaceTicks;
        bool tBitValue;

        if (isPulseDistanceProtocol) {
            /*
             * Pulse distance here, it is not required to check constant mark duration (aOneMarkMicros) and zero space duration.
             */
#if defined DECODE_STRICT_CHECKS
            tMarkTicks = *tRawBufPointer++;
#else
            (void) aZeroSpaceMicros;
            tRawBufPointer++;
#endif
            tSpaceTicks = *tRawBufPointer++; // maybe buffer overflow for last bit, but we do not evaluate this value :-)
            tBitValue = matchSpace(tSpaceTicks, aOneSpaceMicros); // Check for variable length space indicating a 1 or 0

#if defined DECODE_STRICT_CHECKS
            // Check for constant length mark
            if (!matchMark(tMarkTicks, aOneMarkMicros)) {
#  if defined(LOCAL_DEBUG)
                Serial.print(F("Mark="));
                Serial.print(tMarkTicks * MICROS_PER_TICK);
                Serial.print(F(" is not "));
                Serial.print(aOneMarkMicros);
                Serial.print(F(". Index="));
                Serial.print(aNumberOfBits - i);
                Serial.print(' ');
#  endif
                return false;
            }
#endif

        } else {
            /*
             * Pulse width here, it is not required to check (constant) space duration and zero mark duration.
             */
            tMarkTicks = *tRawBufPointer++;
            tBitValue = matchMark(tMarkTicks, aOneMarkMicros); // Check for variable length mark indicating a 1 or 0

#if defined DECODE_STRICT_CHECKS
            tSpaceTicks = *tRawBufPointer++; // maybe buffer overflow for last bit, but we do not evaluate this value :-)
#else
            (void) aZeroMarkMicros;
            (void) aZeroSpaceMicros;
            tRawBufPointer++;
#endif
        }

        if (aMSBfirst) {
            tDecodedData <<= 1;
        }

        if (tBitValue) {
            // It's a 1 -> set the bit
            if (aMSBfirst) {
                tDecodedData |= 1;
            } else {
                tDecodedData |= tMask;
            }
//            IR_TRACE_PRINTLN('1');
        } else {
#if defined DECODE_STRICT_CHECKS
            /*
             * Additionally check length of length parameter which determine a zero
             */
            if (isPulseDistanceProtocol) {
                if (!matchSpace(tSpaceTicks, aZeroSpaceMicros)) {
#  if defined(LOCAL_DEBUG)
                    Serial.print(F("Space="));
                    Serial.print(tSpaceTicks * MICROS_PER_TICK);
                    Serial.print(F(" is not "));
                    Serial.print(aOneSpaceMicros);
                    Serial.print(F(" or "));
                    Serial.print(aZeroSpaceMicros);
                    Serial.print(F(". Index="));
                    Serial.print(aNumberOfBits - i);
                    Serial.print(' ');
#  endif
                    return false;
                }
            } else {
                if (!matchMark(tMarkTicks, aZeroMarkMicros)) {
#  if defined(LOCAL_DEBUG)
                    Serial.print(F("Mark="));
                    Serial.print(tMarkTicks * MICROS_PER_TICK);
                    Serial.print(F(" is not "));
                    Serial.print(aOneMarkMicros);
                    Serial.print(F(" or "));
                    Serial.print(aZeroMarkMicros);
                    Serial.print(F(". Index="));
                    Serial.print(aNumberOfBits - i);
                    Serial.print(' ');
#  endif
                    return false;
                }
            }
#endif
            // do not set the bit
//            IR_TRACE_PRINTLN('0');
        }
#if defined DECODE_STRICT_CHECKS
        // If we have no stop bit, assume that last space, which is not recorded, is correct, since we can not check it
        if (aZeroSpaceMicros == aOneSpaceMicros
                && tRawBufPointer < &decodedIRData.rawDataPtr->rawbuf[decodedIRData.rawDataPtr->rawlen]) {
            // Check for constant length space (of pulse width protocol) here
            if (!matchSpace(tSpaceTicks, aOneSpaceMicros)) {
#  if defined(LOCAL_DEBUG)
                Serial.print(F("Space="));
                Serial.print(tSpaceTicks * MICROS_PER_TICK);
                Serial.print(F(" is not "));
                Serial.print(aOneSpaceMicros);
                Serial.print(F(". Index="));
                Serial.print(aNumberOfBits - i);
                Serial.print(' ');
#  endif
                return false;
            }
        }
#endif
        tMask <<= 1;
    }
    decodedIRData.decodedRawData = tDecodedData;
    return true;
}

bool decodePulseDistanceWidthDatav2(PulseDistanceWidthProtocolConstants *aProtocolConstants, uint8_t aNumberOfBits, uint8_t aStartOffset)
{

    return decodePulseDistanceWidthData(aNumberOfBits, aStartOffset, aProtocolConstants->DistanceWidthTimingInfo.OneMarkMicros,
            aProtocolConstants->DistanceWidthTimingInfo.ZeroMarkMicros, aProtocolConstants->DistanceWidthTimingInfo.OneSpaceMicros,
            aProtocolConstants->DistanceWidthTimingInfo.ZeroSpaceMicros, aProtocolConstants->Flags);
}

bool decodePulseDistanceWidthDatav3(PulseDistanceWidthProtocolConstants *aProtocolConstants, uint8_t aNumberOfBits)
{
	uint8_t aStartOffset = 3;
    return decodePulseDistanceWidthData(aNumberOfBits, aStartOffset, aProtocolConstants->DistanceWidthTimingInfo.OneMarkMicros,
            aProtocolConstants->DistanceWidthTimingInfo.ZeroMarkMicros, aProtocolConstants->DistanceWidthTimingInfo.OneSpaceMicros,
            aProtocolConstants->DistanceWidthTimingInfo.ZeroSpaceMicros, aProtocolConstants->Flags);
}

