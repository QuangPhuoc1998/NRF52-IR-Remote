/*
 * ir_Kaseikyo.hpp
 *
 *  Contains functions for receiving and sending Kaseikyo/Panasonic IR Protocol in "raw" and standard format with 16 bit address + 8 bit command
 *
 *  This file is part of Arduino-IRremote https://github.com/Arduino-IRremote/Arduino-IRremote.
 *
 ************************************************************************************
 * MIT License
 *
 * Copyright (c) 2020-2023 Armin Joachimsmeyer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************************
 */

#include "Ir_kaseikyo.h"

#define KASEIKYO_VENDOR_ID_BITS     16
#define KASEIKYO_VENDOR_ID_PARITY_BITS   4
#define KASEIKYO_ADDRESS_BITS       12
#define KASEIKYO_COMMAND_BITS       8
#define KASEIKYO_PARITY_BITS        8
#define KASEIKYO_BITS               (KASEIKYO_VENDOR_ID_BITS + KASEIKYO_VENDOR_ID_PARITY_BITS + KASEIKYO_ADDRESS_BITS + KASEIKYO_COMMAND_BITS + KASEIKYO_PARITY_BITS) // 48
#define KASEIKYO_UNIT               432 // 16 pulses of 37 kHz (432,432432)  - Pronto 0x70 | 0x10

#define KASEIKYO_HEADER_MARK        (8 * KASEIKYO_UNIT) // 3456
#define KASEIKYO_HEADER_SPACE       (4 * KASEIKYO_UNIT) // 1728

#define KASEIKYO_BIT_MARK           KASEIKYO_UNIT
#define KASEIKYO_ONE_SPACE          (3 * KASEIKYO_UNIT) // 1296
#define KASEIKYO_ZERO_SPACE         KASEIKYO_UNIT

#define KASEIKYO_AVERAGE_DURATION   56000
#define KASEIKYO_REPEAT_PERIOD      130000
#define KASEIKYO_REPEAT_DISTANCE    (KASEIKYO_REPEAT_PERIOD - KASEIKYO_AVERAGE_DURATION) // 74 ms
#define KASEIKYO_MAXIMUM_REPEAT_DISTANCE    (KASEIKYO_REPEAT_DISTANCE + (KASEIKYO_REPEAT_DISTANCE / 4)) // Just a guess

#define PANASONIC_VENDOR_ID_CODE    0x2002
#define DENON_VENDOR_ID_CODE        0x3254
#define MITSUBISHI_VENDOR_ID_CODE   0xCB23
#define SHARP_VENDOR_ID_CODE        0x5AAA
#define JVC_VENDOR_ID_CODE          0x0103

PulseDistanceWidthProtocolConstants KaseikyoProtocolConstants = {KASEIKYO, KASEIKYO_KHZ, KASEIKYO_HEADER_MARK,
KASEIKYO_HEADER_SPACE, KASEIKYO_BIT_MARK, KASEIKYO_ONE_SPACE, KASEIKYO_BIT_MARK, KASEIKYO_ZERO_SPACE, PROTOCOL_IS_LSB_FIRST
       , (KASEIKYO_REPEAT_PERIOD / MICROS_IN_ONE_MILLI), NULL };

bool decodeKaseikyo()
{
    decode_type_t tProtocol;
    // Check we have enough data (96 + 4) 4 for initial gap, start bit mark and space + stop bit mark
    if (decodedIRData.rawDataPtr->rawlen != ((2 * KASEIKYO_BITS) + 4))
    {
#ifdef DEBUG_EXTRA_INFO
        NRF_LOG_INFO("Kaseikyo: ");
        NRF_LOG_INFO("Data length = %d not 100", decodedIRData.rawDataPtr->rawlen);
#endif
        return false;
    }

    if (!checkHeader(&KaseikyoProtocolConstants)) {
        return false;
    }

    // decode first 16 Vendor ID bits
    if (!decodePulseDistanceWidthDatav2(&KaseikyoProtocolConstants, KASEIKYO_VENDOR_ID_BITS, 3))
    {
#if defined(LOCAL_DEBUG)
        Serial.print(F("Kaseikyo: "));
        Serial.println(F("Vendor ID decode failed"));
#endif
        return false;
    }

    uint16_t tVendorId = decodedIRData.decodedRawData;
    if (tVendorId == PANASONIC_VENDOR_ID_CODE) {
        tProtocol = PANASONIC;
    } else if (tVendorId == SHARP_VENDOR_ID_CODE) {
        tProtocol = KASEIKYO_SHARP;
    } else if (tVendorId == DENON_VENDOR_ID_CODE) {
        tProtocol = KASEIKYO_DENON;
    } else if (tVendorId == JVC_VENDOR_ID_CODE) {
        tProtocol = KASEIKYO_JVC;
    } else if (tVendorId == MITSUBISHI_VENDOR_ID_CODE) {
        tProtocol = KASEIKYO_MITSUBISHI;
    } else {
        tProtocol = KASEIKYO;
    }

    // Vendor Parity
    uint8_t tVendorParity = tVendorId ^ (tVendorId >> 8);
    tVendorParity = (tVendorParity ^ (tVendorParity >> 4)) & 0xF;

    /*
     * Decode next 32 bits, 8 VendorID parity parity + 12 address (device and subdevice) + 8 command + 8 parity
     */
    if (!decodePulseDistanceWidthDatav2(&KaseikyoProtocolConstants,
    KASEIKYO_VENDOR_ID_PARITY_BITS + KASEIKYO_ADDRESS_BITS + KASEIKYO_COMMAND_BITS + KASEIKYO_PARITY_BITS, 3 + (2 * KASEIKYO_VENDOR_ID_BITS))) {
#if defined(LOCAL_DEBUG)
        Serial.print(F("Kaseikyo: "));
        Serial.println(F("VendorID parity, address, command + parity decode failed"));
#endif
        return false;
    }

    // Success
//    decodedIRData.flags = IRDATA_FLAGS_IS_LSB_FIRST; // Not required, since this is the start value
    union LongUnion tValue;

    tValue.ULong = decodedIRData.decodedRawData;
    decodedIRData.address = (tValue.UWord.LowWord >> KASEIKYO_VENDOR_ID_PARITY_BITS); // remove 4 bit vendor parity

    decodedIRData.command = tValue.UByte.MidHighByte;
    uint8_t tParity = tValue.UByte.LowByte ^ tValue.UByte.MidLowByte ^ tValue.UByte.MidHighByte;

    if (tVendorParity != (tValue.UByte.LowByte & 0xF))
    {
        decodedIRData.flags = IRDATA_FLAGS_PARITY_FAILED | IRDATA_FLAGS_IS_LSB_FIRST;
#if defined(LOCAL_DEBUG)
        Serial.print(F("Kaseikyo: "));
        Serial.print(F("4 bit VendorID parity is not correct. Expected=0x"));
        Serial.print(tVendorParity, HEX);
        Serial.print(F(" received=0x"));
        Serial.print(decodedIRData.decodedRawData, HEX);
        Serial.print(F(" VendorID=0x"));
        Serial.println(tVendorId, HEX);
#endif
    }

    if (tProtocol == KASEIKYO) {
        decodedIRData.flags |= IRDATA_FLAGS_EXTRA_INFO;
        decodedIRData.extra = tVendorId; // Store (unknown) vendor ID
    }

    if (tValue.UByte.HighByte != tParity) {
        decodedIRData.flags |= IRDATA_FLAGS_PARITY_FAILED;

#if defined(LOCAL_DEBUG)
        Serial.print(F("Kaseikyo: "));
        Serial.print(F("8 bit parity is not correct. Expected=0x"));
        Serial.print(tParity, HEX);
        Serial.print(F(" received=0x"));
        Serial.print(decodedIRData.decodedRawData >> KASEIKYO_COMMAND_BITS, HEX);
        Serial.print(F(" address=0x"));
        Serial.print(decodedIRData.address, HEX);
        Serial.print(F(" command=0x"));
        Serial.println(decodedIRData.command, HEX);
#endif
    }

    decodedIRData.numberOfBits = KASEIKYO_BITS;
    decodedIRData.protocol = tProtocol;

    // check for repeat
    checkForRepeatSpaceTicksAndSetFlag(KASEIKYO_MAXIMUM_REPEAT_DISTANCE / MICROS_PER_TICK);
#ifdef DEBUG_EXTRA_INFO
    NRF_LOG_INFO("Protocol = %d", decodedIRData.protocol);
    NRF_LOG_INFO("Address = %d", decodedIRData.address);
    NRF_LOG_INFO("Command = %d", decodedIRData.command);
    NRF_LOG_INFO("Number of bit = %d", decodedIRData.numberOfBits);
    NRF_LOG_INFO("Raw data = 0x%X", decodedIRData.decodedRawData);
#endif

    return true;
}

void sendKaseikyo(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats, uint16_t aVendorCode)
{
    // Set IR carrier frequency
    enableIROut (KASEIKYO_KHZ); // 37 kHz

    // Vendor Parity
    uint8_t tVendorParity = aVendorCode ^ (aVendorCode >> 8);
    tVendorParity = (tVendorParity ^ (tVendorParity >> 4)) & 0xF;

    union LongUnion tSendValue;
    // Compute parity
    tSendValue.UWord.LowWord = (aAddress << KASEIKYO_VENDOR_ID_PARITY_BITS) | tVendorParity; // set low nibble to parity
    tSendValue.UBytes[2] = aCommand;
    tSendValue.UBytes[3] = aCommand ^ tSendValue.UBytes[0] ^ tSendValue.UBytes[1]; // Parity
    IRRawDataType tRawKaseikyoData[2];
    tRawKaseikyoData[0] = (uint32_t) tSendValue.UWord.LowWord << 16 | aVendorCode; // LSB of tRawKaseikyoData[0] is sent first
    tRawKaseikyoData[1] = tSendValue.UWord.HighWord;
    sendPulseDistanceWidthFromArray(&KaseikyoProtocolConstants, &tRawKaseikyoData[0], KASEIKYO_BITS, aNumberOfRepeats);
}

void sendPanasonic(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats)
{
    sendKaseikyo(aAddress, aCommand, aNumberOfRepeats, PANASONIC_VENDOR_ID_CODE);
}
