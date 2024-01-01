/*
 * Ir_kaseikyo.h
 *
 *  Created on: Dec 28, 2023
 *      Author: phuoc
 */

#ifndef SOURCE_APPLICATION_MID_IR_KASEIKYO_H_
#define SOURCE_APPLICATION_MID_IR_KASEIKYO_H_

#include "main.h"
#include "Mid_IrRemote.h"
#include "IRProtocol.h"

/** \addtogroup Decoder Decoders and encoders for different protocols
 * @{
 */
//==============================================================================
//       K  K   AA    SSS   EEEE  III  K  K  Y   Y   OOO
//       K K   A  A  S      E      I   K K    Y Y   O   O
//       KK    AAAA   SSS   EEE    I   KK      Y    O   O
//       K K   A  A      S  E      I   K K     Y    O   O
//       K  K  A  A  SSSS   EEEE  III  K  K    Y     OOO
//==============================================================================
//==============================================================================
//       PPPP    AAA   N   N   AAA    SSSS   OOO   N   N  IIIII   CCCC
//       P   P  A   A  NN  N  A   A  S      O   O  NN  N    I    C
//       PPPP   AAAAA  N N N  AAAAA   SSS   O   O  N N N    I    C
//       P      A   A  N  NN  A   A      S  O   O  N  NN    I    C
//       P      A   A  N   N  A   A  SSSS    OOO   N   N  IIIII   CCCC
//==============================================================================
/*
 Protocol=Panasonic Address=0xFF1 Command=0x76 Raw-Data=0x9976FF10 48 bits LSB first
 +3450,-1700
 + 450,- 400 + 500,-1250 + 450,- 400 + 500,- 400
 + 450,- 400 + 400,- 450 + 500,- 350 + 450,- 450
 + 450,- 400 + 450,- 400 + 500,- 400 + 450,- 400
 + 450,- 400 + 500,-1250 + 450,- 400 + 500,- 350
 + 500,- 400 + 450,- 400 + 450,- 450 + 450,- 400
 + 450,-1250 + 500,- 400 + 450,- 400 + 450,- 400
 + 450,-1300 + 450,-1250 + 450,-1300 + 400,-1300
 + 450,-1300 + 450,-1250 + 450,-1250 + 500,-1250
 + 450,- 450 + 450,-1250 + 450,-1250 + 500,- 400
 + 450,-1250 + 450,-1300 + 450,-1250 + 450,- 450
 + 450,-1250 + 450,- 400 + 450,- 400 + 500,-1250
 + 450,-1250 + 450,- 400 + 500,- 400 + 450,-1250
 + 450
 Sum: 64300
 */
// http://www.hifi-remote.com/johnsfine/DecodeIR.html#Panasonic
// http://www.hifi-remote.com/johnsfine/DecodeIR.html#Kaseikyo
// The first two (8-bit) bytes contains the vendor code.
// There are multiple interpretations of the next fields:
// IRP: {37k,432}<1,-1|1,-3>(8,-4,M:8,N:8,X:4,D:4,S:8,F:8,G:8,1,-173)+ {X=M:4:0^M:4:4^N:4:0^N:4:4}
// 1. interpretation: After the vendor code, the next byte is 4 bit VendorID parity and 4 bit Device and Subdevice
//    The 5th byte is the function and the last (6.th) byte is xor of the three bytes before it.
//    0_______ 1_______  2______ 3_______ 4_______ 5
//    01234567 89ABCDEF  01234567 01234567 01234567 01234567
//    01000000 00100100  Dev____  Sub_Dev_ Fun____  XOR( B2, B3, B4) - showing Panasonic vendor code 0x2002
// see: http://www.remotecentral.com/cgi-bin/mboard/rc-pronto/thread.cgi?26152
//
// 2. interpretation: LSB first, start bit + 16 VendorID + 4 VendorID parity + 4 Genre1 + 4 Genre2 + 10 Command + 2 ID + 8 Parity + stop bit
// see: https://www.mikrocontroller.net/articles/IRMP_-_english#KASEIKYO
// 32 bit raw data LSB is VendorID parity.
//
// We reduce it to: IRP: {37k,432}<1,-1|1,-3>(8,-4,V:16,X:4,D:4,S:8,F:8,(X^D^S^F):8,1,-173)+ {X=M:4:0^M:4:4^N:4:0^N:4:4}
// start bit + 16 VendorID + 4 VendorID parity + 12 Address + 8 Command + 8 Parity of VendorID parity, Address and Command + stop bit
//
#include "LongUnion.h"

bool decodeKaseikyo();
void sendKaseikyo(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats, uint16_t aVendorCode);



#endif /* SOURCE_APPLICATION_MID_IR_KASEIKYO_H_ */
