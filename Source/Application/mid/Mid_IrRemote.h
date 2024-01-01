#ifndef IR_REMOTE_H_
#define IR_REMOTE_H_

#include "main.h"
#include "Op_HeaderVariable.h"
#include "IRProtocol.h"
#include "Ir_kaseikyo.h"
#include "Ir_DistanceWidthProtocol.h"

#if ! defined(IR_SEND_DUTY_CYCLE_PERCENT)
#define IR_SEND_DUTY_CYCLE_PERCENT 30 // 30 saves power and is compatible to the old existing code
#endif

#if !defined(PULSE_CORRECTION_NANOS)
#  if defined(F_CPU)
// To change this value, you simply can add a line #define "PULSE_CORRECTION_NANOS <My_new_value>" in your ino file before the line "#include <IRremote.hpp>"
#define PULSE_CORRECTION_NANOS (48000L / (F_CPU/MICROS_IN_ONE_SECOND)) // 3000 @16MHz, 666 @72MHz
#  else
#define PULSE_CORRECTION_NANOS 600
#  endif
#endif

#if !defined(TOLERANCE_FOR_DECODERS_MARK_OR_SPACE_MATCHING)
#define TOLERANCE_FOR_DECODERS_MARK_OR_SPACE_MATCHING    25 // Relative tolerance (in percent) for matchTicks(), matchMark() and matchSpace() functions used for protocol decoding.
#endif

#define UINT16_MAX_CUSTOM 65535

/** Lower tolerance for comparison of measured data */
//#define LTOL            (1.0 - (TOLERANCE/100.))
#define LTOL            (100 - TOLERANCE_FOR_DECODERS_MARK_OR_SPACE_MATCHING)
/** Upper tolerance for comparison of measured data */
//#define UTOL            (1.0 + (TOLERANCE/100.))
#define UTOL            (100 + TOLERANCE_FOR_DECODERS_MARK_OR_SPACE_MATCHING)

//#define TICKS_LOW(us)   ((int)(((us)*LTOL/MICROS_PER_TICK)))
//#define TICKS_HIGH(us)  ((int)(((us)*UTOL/MICROS_PER_TICK + 1)))
#if MICROS_PER_TICK == 50 && TOLERANCE_FOR_DECODERS_MARK_OR_SPACE_MATCHING == 25           // Defaults
#define TICKS_LOW(us)   ((us)/67 )     // (us) / ((MICROS_PER_TICK:50 / LTOL:75 ) * 100)
#define TICKS_HIGH(us)  (((us)/40) + 1)  // (us) / ((MICROS_PER_TICK:50 / UTOL:125) * 100) + 1
#else
#define TICKS_LOW(us)   ((uint16_t ) ((long) (us) * LTOL / (MICROS_PER_TICK * 100) ))
#define TICKS_HIGH(us)  ((uint16_t ) ((long) (us) * UTOL / (MICROS_PER_TICK * 100) + 1))
#endif

#define MARK_EXCESS_MICROS    20
// ISR State-Machine : Receiver States
#define IR_REC_STATE_IDLE      0 // Counting the gap time and waiting for the start bit to arrive
#define IR_REC_STATE_MARK      1 // A mark was received and we are counting the duration of it.
#define IR_REC_STATE_SPACE     2 // A space was received and we are counting the duration of it. If space is too long, we assume end of frame.
#define IR_REC_STATE_STOP      3 // Stopped until set to IR_REC_STATE_IDLE which can only be done by resume()

#define INPUT_MARK   0 ///< Sensor output for a mark ("flash")

#define RECORD_GAP_MICROS   5000
#define MICROS_PER_TICK    	50L
#define RECORD_GAP_TICKS    (RECORD_GAP_MICROS / MICROS_PER_TICK) // 100

#define MILLIS_IN_ONE_SECOND 1000L
#define MICROS_IN_ONE_SECOND 1000000L
#define MICROS_IN_ONE_MILLI 1000L

void enableIROut(uint_fast8_t aFrequencyKHz);
void mark(uint16_t aMarkMicros);
void space(uint16_t aSpaceMicros);
void customDelayMicroseconds(unsigned long aMicroseconds);
bool checkHeader(PulseDistanceWidthProtocolConstants *aProtocolConstants);

void sendPulseDistanceWidthData2(PulseDistanceWidthProtocolConstants *aProtocolConstants, IRRawDataType aData,
        uint8_t aNumberOfBits);
void sendPulseDistanceWidthData3(uint16_t aOneMarkMicros, uint16_t aOneSpaceMicros, uint16_t aZeroMarkMicros,
        uint16_t aZeroSpaceMicros, IRRawDataType aData, uint8_t aNumberOfBits, bool aMSBFirst, bool aSendStopBit);
void sendPulseDistanceWidthData1(uint16_t aOneMarkMicros, uint16_t aOneSpaceMicros, uint16_t aZeroMarkMicros,
        uint16_t aZeroSpaceMicros, IRRawDataType aData, uint8_t aNumberOfBits, uint8_t aFlags);

bool matchTicks(uint16_t aMeasuredTicks, uint16_t aMatchValueMicros);
bool MATCH(uint16_t measured_ticks, uint16_t desired_us);
bool matchMark(uint16_t aMeasuredTicks, uint16_t aMatchValueMicros);
bool MATCH_MARK(uint16_t measured_ticks, uint16_t desired_us);
bool matchSpace(uint16_t aMeasuredTicks, uint16_t aMatchValueMicros);
bool MATCH_SPACE(uint16_t measured_ticks, uint16_t desired_us);
int getMarkExcessMicros();

void checkForRepeatSpaceTicksAndSetFlag(uint16_t aMaximumRepeatSpaceTicks) ;
bool decodePulseDistanceWidthData(uint8_t aNumberOfBits, uint8_t aStartOffset, uint16_t aOneMarkMicros,
        uint16_t aZeroMarkMicros, uint16_t aOneSpaceMicros, uint16_t aZeroSpaceMicros, bool aMSBfirst) ;
bool decodePulseDistanceWidthDatav2(PulseDistanceWidthProtocolConstants *aProtocolConstants, uint8_t aNumberOfBits, uint8_t aStartOffset);
#endif // IR_REMOTE_H_
