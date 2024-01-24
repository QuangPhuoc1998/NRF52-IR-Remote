#include "main.h"

//#define DEBUG_JVC

PulseDistanceWidthProtocolConstants JVCProtocolConstants = { JVC, JVC_KHZ, {JVC_HEADER_MARK, JVC_HEADER_SPACE, JVC_BIT_MARK,
JVC_ONE_SPACE, JVC_BIT_MARK, JVC_ZERO_SPACE}, PROTOCOL_IS_LSB_FIRST, (JVC_REPEAT_PERIOD / MICROS_IN_ONE_MILLI), NULL };

/************************************
 * Start of send and decode functions
 ************************************/

/**
 * The JVC protocol repeats by skipping the header mark and space -> this leads to a poor repeat detection for JVC protocol.
 */
void sendJVC(uint8_t aAddress, uint8_t aCommand, int_fast8_t aNumberOfRepeats)
{
    // Set IR carrier frequency
    enableIROut (JVC_KHZ); // 38 kHz

    if (aNumberOfRepeats < 0) {
        // The JVC protocol repeats by skipping the header.
        aNumberOfRepeats = 0;
    } else {
        mark(JVC_HEADER_MARK);
        space(JVC_HEADER_SPACE);
    }

    uint_fast8_t tNumberOfCommands = aNumberOfRepeats + 1;
    while (tNumberOfCommands > 0) {

        // Address + command
    	sendPulseDistanceWidthDataV3(&JVCProtocolConstants, aAddress | (aCommand << JVC_ADDRESS_BITS), JVC_BITS);

        tNumberOfCommands--;
        // skip last delay!
        if (tNumberOfCommands > 0) {
            // send repeated command in a fixed raster
            nrf_delay_ms(JVC_REPEAT_DISTANCE / MICROS_IN_ONE_MILLI);
        }
    }
}

bool decodeJVC(void)
{

//    uint_fast8_t tRawlen = decodedIRData.rawDataPtr->rawlen; // Using a local variable does not improve code size

    // Check we have the right amount of data (36 or 34). The +4 is for initial gap, start bit mark and space + stop bit mark.
    // +4 is for first frame, +2 is for repeats
    if (decodedIRData.rawDataPtr->rawlen != ((2 * JVC_BITS) + 2) && decodedIRData.rawDataPtr->rawlen != ((2 * JVC_BITS) + 4)) {
#ifdef DEBUG_JVC
        NRF_LOG_INFO("JVC: Data length = %d is not 34 or 36", decodedIRData.rawDataPtr->rawlen);
#endif
        return false;
    }

    if (decodedIRData.rawDataPtr->rawlen == ((2 * JVC_BITS) + 2)) {
        /*
         * Check for repeat
         * Check leading space and first and last mark length
         */
        if (decodedIRData.rawDataPtr->rawbuf[0] < ((JVC_REPEAT_DISTANCE + (JVC_REPEAT_DISTANCE / 4) / MICROS_PER_TICK))
                && matchMark(decodedIRData.rawDataPtr->rawbuf[1], JVC_BIT_MARK)
                && matchMark(decodedIRData.rawDataPtr->rawbuf[decodedIRData.rawDataPtr->rawlen - 1], JVC_BIT_MARK)) {
            /*
             * We have a repeat here, so do not check for start bit
             */
            decodedIRData.flags = IRDATA_FLAGS_IS_REPEAT | IRDATA_FLAGS_IS_LSB_FIRST;
            decodedIRData.address = lastDecodedAddress;
            decodedIRData.command = lastDecodedCommand;
            decodedIRData.protocol = JVC;
        }
    } else {

        if (!checkHeader(&JVCProtocolConstants)) {
            return false;
        }

        if (!decodePulseDistanceWidthDatav3(&JVCProtocolConstants, JVC_BITS)) {
#ifdef DEBUG_JVC
            Serial.print(F("JVC: "));
            Serial.println(F("Decode failed"));
#endif
            return false;
        }

        // Success
//    decodedIRData.flags = IRDATA_FLAGS_IS_LSB_FIRST; // Not required, since this is the start value
        decodedIRData.command = decodedIRData.decodedRawData >> JVC_ADDRESS_BITS;  // upper 8 bits of LSB first value
        decodedIRData.address = decodedIRData.decodedRawData & 0xFF;    // lowest 8 bit of LSB first value
        decodedIRData.numberOfBits = JVC_BITS;
        decodedIRData.protocol = JVC;
    }

    return true;
}

