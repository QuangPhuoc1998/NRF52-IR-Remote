#include "main.h"

#define DEBUG_MAGIQUEST

// assume 110 as repeat period
PulseDistanceWidthProtocolConstants MagiQuestProtocolConstants = { MAGIQUEST, 38, {MAGIQUEST_ZERO_MARK, MAGIQUEST_ZERO_SPACE,
MAGIQUEST_ONE_MARK, MAGIQUEST_ONE_SPACE, MAGIQUEST_ZERO_MARK, MAGIQUEST_ZERO_SPACE}, PROTOCOL_IS_MSB_FIRST, 110, NULL };

/**
 * @param aWandId       31 bit ID
 * @param aMagnitude    9 bit Magnitude
 */
void sendMagiQuest(uint32_t aWandId, uint16_t aMagnitude)
{
    // Set IR carrier frequency
    enableIROut(38);

    aMagnitude &= 0x1FF; // we have 9 bit
    union LongUnion tWandId;
    tWandId.ULong = aWandId << 1;
    uint8_t tChecksum = (tWandId.Bytes[0]) + tWandId.Bytes[1] + tWandId.Bytes[2] + tWandId.Bytes[3];
    tChecksum += aMagnitude + (aMagnitude >> 8);
    tChecksum = ~tChecksum + 1;

    // 8 start bits
    sendPulseDistanceWidthDataV3(&MagiQuestProtocolConstants, 0, 8);
    // 48 bit data
    sendPulseDistanceWidthDataV3(&MagiQuestProtocolConstants, aWandId, MAGIQUEST_WAND_ID_BITS); // send only 31 bit, do not send MSB here
    sendPulseDistanceWidthDataV3(&MagiQuestProtocolConstants, aMagnitude, MAGIQUEST_MAGNITUDE_BITS);
    sendPulseDistanceWidthDataV3(&MagiQuestProtocolConstants, tChecksum, MAGIQUEST_CHECKSUM_BITS);
#if defined(LOCAL_DEBUG)
    // must be after sending, in order not to destroy the send timing
    Serial.print(F("MagiQuest checksum=0x"));
    Serial.println(tChecksum, HEX);
#endif
}

//+=============================================================================
//
/*
 * decodes a 56 bit result, which is not really compatible with standard decoder layout
 * magnitude is stored in command
 * 31 bit wand_id is stored in decodedRawData
 * lower 16 bit of wand_id is stored in address
 */
bool decodeMagiQuest(void)
{
    // Check we have the right amount of data, magnitude and ID bits and 8 start bits + 0 stop bit
    if (decodedIRData.rawDataPtr->rawlen != (2 * MAGIQUEST_BITS))
    {
#ifdef DEBUG_MAGIQUEST
    	NRF_LOG_INFO("MagiQuest: Data length = %d is not 112", decodedIRData.rawDataPtr->rawlen);
#endif
        return false;
    }

    /*
     * Check for 8 zero header bits
     */
    if (!decodePulseDistanceWidthDatav2(&MagiQuestProtocolConstants, MAGIQUEST_START_BITS, 1)) {
#ifdef DEBUG_MAGIQUEST
    	NRF_LOG_INFO("MagiQuest: Start bit decode failed");
#endif
        return false;
    }
    if (decodedIRData.decodedRawData != 0) {
#ifdef DEBUG_MAGIQUEST
    	NRF_LOG_INFO("MagiQuest: Not 8 leading zero start bits received, RawData=0x%X", decodedIRData.decodedRawData);
#endif
        return false;
    }

    /*
     * Decode the 31 bit ID
     */
    if (!decodePulseDistanceWidthDatav2(&MagiQuestProtocolConstants, MAGIQUEST_WAND_ID_BITS, (MAGIQUEST_START_BITS * 2) + 1)) {
#ifdef DEBUG_MAGIQUEST
    	NRF_LOG_INFO("MagiQuest: ID decode failed");
#endif
        return false;
    }
    union LongUnion tDecodedRawData;
#ifdef DEBUG_MAGIQUEST
    NRF_LOG_INFO("31 bit WandId=0x%X", decodedIRData.decodedRawData);
#endif
    uint32_t tWandId = decodedIRData.decodedRawData; // save tWandId for later use
    tDecodedRawData.ULong = decodedIRData.decodedRawData << 1; // shift for checksum computation
    uint8_t tChecksum = tDecodedRawData.Bytes[0] + tDecodedRawData.Bytes[1] + tDecodedRawData.Bytes[2] + tDecodedRawData.Bytes[3];
#ifdef DEBUG_MAGIQUEST
    NRF_LOG_INFO("31 bit WandId=0x%X shifted=0x%X", decodedIRData.decodedRawData, tDecodedRawData.ULong);
#endif
    /*
     * Decode the 9 bit Magnitude + 8 bit checksum
     */
    if (!decodePulseDistanceWidthDatav2(&MagiQuestProtocolConstants, MAGIQUEST_MAGNITUDE_BITS + MAGIQUEST_CHECKSUM_BITS,
            ((MAGIQUEST_WAND_ID_BITS + MAGIQUEST_START_BITS) * 2) + 1)) {
#ifdef DEBUG_MAGIQUEST
    	NRF_LOG_INFO("MagiQuest: Magnitude + checksum decode failed");
#endif
        return false;
    }

#ifdef DEBUG_MAGIQUEST
    NRF_LOG_INFO("Magnitude + checksum=0x%X", decodedIRData.decodedRawData);
#endif
    tDecodedRawData.ULong = decodedIRData.decodedRawData;
    decodedIRData.command = tDecodedRawData.UBytes[1] | tDecodedRawData.UBytes[2] << 8; // Values observed are 0x102,01,04,37,05,38,2D| 02,06,04|03,103,12,18,0E|09

    tChecksum += tDecodedRawData.UBytes[2] /* only one bit */+ tDecodedRawData.UBytes[1] + tDecodedRawData.UBytes[0];
    if (tChecksum != 0) {
        decodedIRData.flags |= IRDATA_FLAGS_PARITY_FAILED;
#ifdef DEBUG_MAGIQUEST
        NRF_LOG_INFO("Checksum 0x%X is not 0", tChecksum);
#endif
    }

    // Success
    decodedIRData.decodedRawData = tWandId;     // 31 bit wand_id
    decodedIRData.address = tWandId;            // lower 16 bit of wand_id
    decodedIRData.extra = tWandId >> 16;        // upper 15 bit of wand_id

    decodedIRData.protocol = MAGIQUEST;
    decodedIRData.numberOfBits = MAGIQUEST_BITS;
    decodedIRData.flags = IRDATA_FLAGS_IS_MSB_FIRST;

    return true;
}
