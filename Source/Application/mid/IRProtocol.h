/**
 * @file IRProtocol.h
 * @brief Common declarations for receiving and sending.
 *
 * This file is part of Arduino-IRremote https://github.com/Arduino-IRremote/Arduino-IRremote.
 *
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
#ifndef _IR_PROTOCOL_H
#define _IR_PROTOCOL_H

#include "Mid_IrRemote.h"


#if defined(__AVR__)
const __FlashStringHelper* getProtocolString(decode_type_t aProtocol);
#else
const char* getProtocolString(decode_type_t aProtocol);
#endif
//void printIRResultShort(Print *aSerial, IRData *aIRDataPtr, bool aPrintGap); // A static function to be able to print send or copied received data.

/*
 * Convenience functions to convert MSB to LSB values
 */
uint8_t bitreverseOneByte(uint8_t aValue);
uint32_t bitreverse32Bit(uint32_t aInput);

#endif // _IR_PROTOCOL_H
