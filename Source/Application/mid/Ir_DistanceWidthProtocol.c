#include "Ir_DistanceWidthProtocol.h"

/*
 * @return false if more than 2 distinct duration values found
 */
bool aggregateArrayCounts(uint8_t aArray[], uint8_t aMaxIndex, uint8_t *aShortIndex, uint8_t *aLongIndex)
{
    uint8_t tSum = 0;
    uint16_t tWeightedSum = 0;
    for (uint_fast8_t i = 0; i <= aMaxIndex; i++) {
        uint8_t tCurrentDurations = aArray[i];
        if (tCurrentDurations != 0) {
            // Add it to sum and remove array content
            tSum += tCurrentDurations;
            tWeightedSum += (tCurrentDurations * i);
            aArray[i] = 0;
        }
        if ((tCurrentDurations == 0 || i == aMaxIndex) && tSum != 0) {
            // here we have a sum and a gap after the values
            uint8_t tAggregateIndex = (tWeightedSum + (tSum / 2)) / tSum; // with rounding
            aArray[tAggregateIndex] = tSum; // disabling this line increases code size by 2 - unbelievable!
            // store aggregate for later decoding
            if (*aShortIndex == 0) {
                *aShortIndex = tAggregateIndex;
            } else if (*aLongIndex == 0) {
                *aLongIndex = tAggregateIndex;
            } else {
                // we have 3 bins => this is likely no pulse width or distance protocol. e.g. it can be RC5.
                return false;
            }
            // initialize for next aggregation
            tSum = 0;
            tWeightedSum = 0;
        }
    }
    return true;
}


/*
 * Try to decode a pulse distance or pulse width protocol.
 * 1. Analyze all space and mark length
 * 2. Decide if we have an pulse width or distance protocol
 * 3. Try to decode with the mark and space data found in step 1
 * No data and address decoding, only raw data as result.
 */
bool decodeDistanceWidth()
{
    uint8_t tDurationArray[DURATION_ARRAY_SIZE]; // For up to 49 ticks / 2450 us

    /*
     * Accept only protocols with at least 8 bits
     */
    if (decodedIRData.rawDataPtr->rawlen < (2 * 8) + 4)
    {
#ifdef DEBUG_EXTRA_INFO
    	NRF_LOG_INFO("PULSE_DISTANCE_WIDTH: ")
		NRF_LOG_INFO("Data length = %d", decodedIRData.rawDataPtr->rawlen);
#endif
        return false;
    }

    uint_fast8_t i;

    // Reset duration array
    memset(tDurationArray, 0, DURATION_ARRAY_SIZE);

    uint8_t tIndexOfMaxDuration = 0;
    /*
     * Count number of mark durations up to 49 ticks. Skip leading start and trailing stop bit.
     */
    for (i = 3; i < (uint_fast8_t) decodedIRData.rawDataPtr->rawlen - 2; i += 2)
    {
    	uint16_t tDurationTicks = decodedIRData.rawDataPtr->rawbuf[i];
        if (tDurationTicks < DURATION_ARRAY_SIZE)
        {
            tDurationArray[tDurationTicks]++; // count duration if less than DURATION_ARRAY_SIZE (50)
            if (tIndexOfMaxDuration < tDurationTicks) {
                tIndexOfMaxDuration = tDurationTicks;
            }
        }
        else
        {
#ifdef DEBUG_EXTRA_INFO
        	NRF_LOG_INFO("PULSE_DISTANCE_WIDTH: ");
        	NRF_LOG_INFO("Mark : %d", tDurationTicks * MICROS_PER_TICK);
        	NRF_LOG_INFO(" is longer than maximum ");
        	NRF_LOG_INFO("%d us. Index = %d", DURATION_ARRAY_SIZE * MICROS_PER_TICK, i);
#endif
            return false;

        }
    }

    /*
     * Aggregate mark counts to one duration bin
     */
    uint8_t tMarkTicksShort = 0;
    uint8_t tMarkTicksLong = 0;
    bool tSuccess = aggregateArrayCounts(tDurationArray, tIndexOfMaxDuration, &tMarkTicksShort, &tMarkTicksLong);
#if defined(LOCAL_DEBUG)
    Serial.println(F("Mark:"));
    printDurations(tDurationArray, tIndexOfMaxDuration);
#endif

    if (!tSuccess)
    {
#ifdef DEBUG_EXTRA_INFO
    	NRF_LOG_INFO("PULSE_DISTANCE_WIDTH: ");
    	NRF_LOG_INFO("Mark aggregation failed, more than 2 distinct mark duration values found");
#endif
        return false;
    }

    // Reset duration array
    memset(tDurationArray, 0, DURATION_ARRAY_SIZE);

    /*
     * Count number of space durations. Skip leading start and trailing stop bit.
     */
    tIndexOfMaxDuration = 0;
    for (i = 4; i < (uint_fast8_t) decodedIRData.rawDataPtr->rawlen - 2; i += 2)
    {
    	uint16_t tDurationTicks = decodedIRData.rawDataPtr->rawbuf[i];
        if (tDurationTicks < DURATION_ARRAY_SIZE) {
            tDurationArray[tDurationTicks]++;
            if (tIndexOfMaxDuration < tDurationTicks) {
                tIndexOfMaxDuration = tDurationTicks;
            }
        }
        else
        {
#ifdef DEBUG_EXTRA_INFO
        	NRF_LOG_INFO("PULSE_DISTANCE_WIDTH: ");
        	NRF_LOG_INFO("Space %d is longer than %d  us. Index = %d", tDurationTicks * MICROS_PER_TICK, DURATION_ARRAY_SIZE * MICROS_PER_TICK, i);
#endif
            return false;
        }
    }

    /*
     * Aggregate space counts to one duration bin
     */
    uint8_t tSpaceTicksShort = 0;
    uint8_t tSpaceTicksLong = 0;
    tSuccess = aggregateArrayCounts(tDurationArray, tIndexOfMaxDuration, &tSpaceTicksShort, &tSpaceTicksLong);
#if defined(LOCAL_DEBUG)
    Serial.println(F("Space:"));
    printDurations(tDurationArray, tIndexOfMaxDuration);
#endif

    if (!tSuccess)
    {
#ifdef DEBUG_EXTRA_INFO
    	NRF_LOG_INFO("PULSE_DISTANCE_WIDTH: Space aggregation failed, more than 2 distinct space duration values found");
#endif
    	return false;
    }

    /*
     * Print characteristics of this protocol. Durations are in ticks.
     * Number of bits, start bit, start pause, long mark, long space, short mark, short space
     *
     * NEC:         32, 180, 90,  0, 34, 11, 11
     * Samsung32:   32,  90, 90,  0, 34, 11, 11
     * LG:          28, 180, 84,  0, 32, 10, 11
     * JVC:         16, 168, 84,  0, 32, 10, 10
     * Kaseikyo:    48.  69, 35,  0, 26,  9,  9
     * Sony:  12|15|20,  48, 12, 24,  0, 12, 12 // the only known pulse width protocol
     */
#if defined(LOCAL_DEBUG)
    Serial.print(F("DistanceWidthTimingInfoStruct: "));
    Serial.print(decodedIRData.rawDataPtr->rawbuf[1] * MICROS_PER_TICK);
    Serial.print(F(", "));
    Serial.print(decodedIRData.rawDataPtr->rawbuf[2] * MICROS_PER_TICK);
    Serial.print(F(", "));
    Serial.print(tMarkTicksLong * MICROS_PER_TICK);
    Serial.print(F(", "));
    Serial.print(tSpaceTicksLong * MICROS_PER_TICK);
    Serial.print(F(", "));
    Serial.print(tMarkTicksShort * MICROS_PER_TICK);
    Serial.print(F(", "));
    Serial.println(tSpaceTicksShort * MICROS_PER_TICK);
#endif
    uint8_t tStartIndex = 3;
    // skip leading start bit for decoding.
    uint16_t tNumberOfBits = (decodedIRData.rawDataPtr->rawlen / 2) - 1;
    if (tSpaceTicksLong > 0 && tMarkTicksLong == 0) {
        // For PULSE_DISTANCE a stop bit is mandatory, for PULSE_WIDTH it is not required!
        tNumberOfBits--; // Correct for stop bit
    }
    decodedIRData.numberOfBits = tNumberOfBits;
    uint8_t tNumberOfAdditionalArrayValues = (tNumberOfBits - 1) / BITS_IN_RAW_DATA_TYPE;

    /*
     * We can have the following protocol timings
     * Pulse distance:          Pulses/marks are constant, pause/spaces have different length, like NEC.
     * Pulse width:             Pulses/marks have different length, pause/spaces are constant, like Sony.
     * Pulse distance width:    Pulses/marks and pause/spaces have different length, often the bit length is constant, like MagiQuest.
     * Pulse distance width can be decoded by pulse width decoder, if this decoder does not check the length of pause/spaces.
     */

    if (tMarkTicksLong == 0 && tSpaceTicksLong == 0)
    {
#ifdef DEBUG_EXTRA_INFO
    	NRF_LOG_INFO("PULSE_DISTANCE: Only 1 distinct duration value for each space and mark found");
#endif
        return false;
    }
    unsigned int tSpaceMicrosShort;
#if defined DECODE_STRICT_CHECKS
        if(tMarkTicksLong > 0 && tSpaceTicksLong > 0) {
            // We have different mark and space length here, so signal decodePulseDistanceWidthData() not to check against constant length decodePulseDistanceWidthData
            tSpaceMicrosShort = 0;
        }
#endif
    tSpaceMicrosShort = tSpaceTicksShort * MICROS_PER_TICK;
    unsigned int tMarkMicrosShort = tMarkTicksShort * MICROS_PER_TICK;
    unsigned int tMarkMicrosLong = tMarkTicksLong * MICROS_PER_TICK;
    unsigned int tSpaceMicrosLong = tSpaceTicksLong * MICROS_PER_TICK;

    for (uint_fast8_t i = 0; i <= tNumberOfAdditionalArrayValues; ++i)
    {
        uint8_t tNumberOfBitsForOneDecode = tNumberOfBits;
        /*
         * Decode in 32/64 bit chunks. Only the last chunk can contain less than 32/64 bits
         */
        if (tNumberOfBitsForOneDecode > BITS_IN_RAW_DATA_TYPE) {
            tNumberOfBitsForOneDecode = BITS_IN_RAW_DATA_TYPE;
        }
        bool tResult;
        if (tMarkTicksLong > 0) {
            /*
             * Here short and long mark durations found.
             */
            decodedIRData.protocol = PULSE_WIDTH;
            tResult = decodePulseDistanceWidthData(tNumberOfBitsForOneDecode, tStartIndex, tMarkMicrosLong, tMarkMicrosShort,
                    tSpaceMicrosShort, 0,
#if defined(USE_MSB_DECODING_FOR_DISTANCE_DECODER)
                    true
#else
                    false
#endif
                    );
        }
        else
        {
            /*
             * Here short and long space durations found.
             */
            decodedIRData.protocol = PULSE_DISTANCE;
            tResult = decodePulseDistanceWidthData(tNumberOfBitsForOneDecode, tStartIndex, tMarkMicrosShort, tMarkMicrosShort,
                    tSpaceMicrosLong, tSpaceMicrosShort,
#if defined(USE_MSB_DECODING_FOR_DISTANCE_DECODER)
                    true
#else
                    false
#endif
                    );
        }
        if (!tResult)
        {
#ifdef DEBUG_EXTRA_INFO
        	NRF_LOG_INFO("PULSE_WIDTH: Decode failed");
#endif
            return false;
        }
#if defined(LOCAL_DEBUG)
        Serial.print(F("PULSE_WIDTH: "));
        Serial.print(F("decodedRawData=0x"));
        Serial.println(decodedIRData.decodedRawData, HEX);
#endif
        // fill array with decoded data
        decodedIRData.decodedRawDataArray[i] = decodedIRData.decodedRawData;
        tStartIndex += (2 * BITS_IN_RAW_DATA_TYPE);
        tNumberOfBits -= BITS_IN_RAW_DATA_TYPE;
    }

#if defined(USE_MSB_DECODING_FOR_DISTANCE_DECODER)
    decodedIRData.flags = IRDATA_FLAGS_IS_MSB_FIRST;
#endif

    // Check for repeat
    checkForRepeatSpaceTicksAndSetFlag(DISTANCE_WIDTH_MAXIMUM_REPEAT_DISTANCE_MICROS / MICROS_PER_TICK);

    /*
     * Store timing data to reproduce frame for sending
     */
    decodedIRData.DistanceWidthTimingInfo.HeaderMarkMicros = (decodedIRData.rawDataPtr->rawbuf[1] * MICROS_PER_TICK);
    decodedIRData.DistanceWidthTimingInfo.HeaderSpaceMicros = (decodedIRData.rawDataPtr->rawbuf[2] * MICROS_PER_TICK);
    decodedIRData.DistanceWidthTimingInfo.ZeroMarkMicros = tMarkMicrosShort;
    decodedIRData.DistanceWidthTimingInfo.ZeroSpaceMicros = tSpaceMicrosShort;
    if (tMarkMicrosLong != 0) {
        decodedIRData.DistanceWidthTimingInfo.OneMarkMicros = tMarkMicrosLong;

        decodedIRData.DistanceWidthTimingInfo.OneSpaceMicros = tSpaceMicrosShort;
        if (tSpaceMicrosLong != 0) {
            // Assume long space for zero when we have PulseDistanceWidth -> enables constant bit length
            decodedIRData.DistanceWidthTimingInfo.ZeroSpaceMicros = tSpaceMicrosLong;
        }
    } else {
        decodedIRData.DistanceWidthTimingInfo.OneMarkMicros = tMarkMicrosShort;

        // Here tMarkMicrosLong is 0 => tSpaceMicrosLong != 0
        decodedIRData.DistanceWidthTimingInfo.OneSpaceMicros = tSpaceMicrosLong;
    }

#if defined(LOCAL_DEBUG)
    Serial.print(F("DistanceWidthTimingInfo="));
    IrReceiver.printDistanceWidthTimingInfo(&Serial, &decodedIRData.DistanceWidthTimingInfo);
    Serial.println();
#endif

#ifdef DEBUG_EXTRA_INFO
    NRF_LOG_INFO("Protocol = %d", decodedIRData.protocol);
    NRF_LOG_INFO("Address = %d", decodedIRData.address);
    NRF_LOG_INFO("Command = %d", decodedIRData.command);
    NRF_LOG_INFO("Number of bit = %d", decodedIRData.numberOfBits);
    NRF_LOG_INFO("Raw data = 0x%X", decodedIRData.decodedRawData);
#endif
    return true;
}
