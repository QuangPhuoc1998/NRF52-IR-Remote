/*
 * Ir_DistanceWidthProtocol.h
 *
 *  Created on: Dec 30, 2023
 *      Author: phuoc
 */

#ifndef SOURCE_APPLICATION_MID_IR_DISTANCEWIDTHPROTOCOL_H_
#define SOURCE_APPLICATION_MID_IR_DISTANCEWIDTHPROTOCOL_H_

#include "main.h"
#include "Mid_IrRemote.h"
#include "IRProtocol.h"

#if !defined(DISTANCE_WIDTH_MAXIMUM_REPEAT_DISTANCE_MICROS)
#define DISTANCE_WIDTH_MAXIMUM_REPEAT_DISTANCE_MICROS       100000 // 100 ms, bit it is just a guess
#endif

#define DURATION_ARRAY_SIZE 50

bool aggregateArrayCounts(uint8_t aArray[], uint8_t aMaxIndex, uint8_t *aShortIndex, uint8_t *aLongIndex);
bool decodeDistanceWidth();

#endif /* SOURCE_APPLICATION_MID_IR_DISTANCEWIDTHPROTOCOL_H_ */
