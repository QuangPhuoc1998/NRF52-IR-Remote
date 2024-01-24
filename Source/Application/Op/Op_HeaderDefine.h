#ifndef _OP_HEADER_DEFINE_H_
#define _OP_HEADER_DEFINE_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Define for IR model
//#define DEBUG_EXTRA_INFO
#define DECODE_DISTANCE_WIDTH
#define RAW_BUFFER_LENGTH  					750  // MagiQuest requires 112 bytes.
#define MAX_IR_CODE							10
#define NO_LED_FEEDBACK_CODE
// Define for IR Protocol
typedef uint32_t IRRawDataType;
#define BITS_IN_RAW_DATA_TYPE   32

#define PROCOTL_LEN	(30)

#define PROTOCOL_NAME_UNKNOWN				"UNKNOWN"
#define PROTOCOL_NAME_PULSE_WIDTH			"PulseWidth"
#define PROTOCOL_NAME_PULSE_DISTANCE		"PulseDistance"
#define PROTOCOL_NAME_APPLE					"Apple"
#define PROTOCOL_NAME_DENON					"Denon"
#define PROTOCOL_NAME_JVC					"JVC"
#define PROTOCOL_NAME_LG					"LG"
#define PROTOCOL_NAME_LG2					"LG2"
#define PROTOCOL_NAME_NEC					"NEC"
#define PROTOCOL_NAME_NEC2					"NEC2"
#define PROTOCOL_NAME_ONKYO					"Onkyo"
#define PROTOCOL_NAME_PANASONIC				"Panasonic"
#define PROTOCOL_NAME_KASEIKYO				"Kaseikyo"
#define PROTOCOL_NAME_KASEIKYO_DENON		"Kaseikyo_Denon"
#define PROTOCOL_NAME_KASEIKYO_SHARP		"Kaseikyo_Sharp"
#define PROTOCOL_NAME_KASEIKYO_JVC			"Kaseikyo_JVC"
#define PROTOCOL_NAME_KASEIKYO_MITSUBISHI	"Kaseikyo_Mitsubishi"
#define PROTOCOL_NAME_RC5					"RC5"
#define PROTOCOL_NAME_RC6					"RC6"
#define PROTOCOL_NAME_SAMSUNG				"Samsung"
#define PROTOCOL_NAME_SAMSUNG48				"Samsung48"
#define PROTOCOL_NAME_SAMSUNGLG				"SamsungLG"
#define PROTOCOL_NAME_SHARP					"Sharp"
#define PROTOCOL_NAME_SONY					"Sony"
#define PROTOCOL_NAME_BANG_OLUFSEN			"Bang&Olufsen"
#define PROTOCOL_NAME_BOSEWAVE				"BoseWave"
#define PROTOCOL_NAME_LEGO					"Lego"
#define PROTOCOL_NAME_MAGIQUEST				"MagiQuest"
#define PROTOCOL_NAME_WHYNTER				"Whynter"
#define PROTOCOL_NAME_FAST					"FAST"

typedef enum {
    UNKNOWN = 0,
    PULSE_WIDTH,
    PULSE_DISTANCE,
    APPLE,
    DENON,
    JVC,
    LG,
    LG2,
    NEC,
    NEC2, /* NEC with full frame as repeat */
    ONKYO,
    PANASONIC,
    KASEIKYO,
    KASEIKYO_DENON,
    KASEIKYO_SHARP,
    KASEIKYO_JVC,
    KASEIKYO_MITSUBISHI,
    RC5,
    RC6,
    SAMSUNG,
    SAMSUNG48,
    SAMSUNG_LG,
    SHARP,
    SONY,
    /* Now the exotic protocols */
    BANG_OLUFSEN,
    BOSEWAVE,
    LEGO_PF,
    MAGIQUEST,
    WHYNTER,
    FAST
} decode_type_t;

#define PROTOCOLNAME 					\
{										\
	PROTOCOL_NAME_UNKNOWN			 ,	\
	PROTOCOL_NAME_PULSE_WIDTH		 ,  \
	PROTOCOL_NAME_PULSE_DISTANCE	 ,  \
	PROTOCOL_NAME_APPLE				 ,  \
	PROTOCOL_NAME_DENON				 ,  \
	PROTOCOL_NAME_JVC				 ,  \
	PROTOCOL_NAME_LG				 ,  \
	PROTOCOL_NAME_LG2				 ,  \
	PROTOCOL_NAME_NEC				 ,  \
	PROTOCOL_NAME_NEC2				 ,  \
	PROTOCOL_NAME_ONKYO				 ,  \
	PROTOCOL_NAME_PANASONIC			 ,  \
	PROTOCOL_NAME_KASEIKYO			 ,  \
	PROTOCOL_NAME_KASEIKYO_DENON	 ,  \
	PROTOCOL_NAME_KASEIKYO_SHARP	 ,  \
	PROTOCOL_NAME_KASEIKYO_JVC		 ,  \
	PROTOCOL_NAME_KASEIKYO_MITSUBISHI,  \
	PROTOCOL_NAME_RC5				 ,  \
	PROTOCOL_NAME_RC6				 ,  \
	PROTOCOL_NAME_SAMSUNG			 ,  \
	PROTOCOL_NAME_SAMSUNG48			 ,  \
	PROTOCOL_NAME_SAMSUNGLG			 ,  \
	PROTOCOL_NAME_SHARP				 ,  \
	PROTOCOL_NAME_SONY				 ,  \
	PROTOCOL_NAME_BANG_OLUFSEN		 ,  \
	PROTOCOL_NAME_BOSEWAVE			 ,  \
	PROTOCOL_NAME_LEGO				 ,  \
	PROTOCOL_NAME_MAGIQUEST			 ,  \
	PROTOCOL_NAME_WHYNTER			 ,  \
	PROTOCOL_NAME_FAST				   \
}

#define SIRCS_12_PROTOCOL       12
#define SIRCS_15_PROTOCOL       15
#define SIRCS_20_PROTOCOL       20

struct DistanceWidthTimingInfoStruct {
    uint16_t HeaderMarkMicros;
    uint16_t HeaderSpaceMicros;
    uint16_t OneMarkMicros;
    uint16_t OneSpaceMicros;
    uint16_t ZeroMarkMicros;
    uint16_t ZeroSpaceMicros;
};

/*
 * Definitions for member IRData.flags
 */
#define IRDATA_FLAGS_EMPTY              0x00
#define IRDATA_FLAGS_IS_REPEAT          0x01 ///< The gap between the preceding frame is as smaller than the maximum gap expected for a repeat. !!!We do not check for changed command or address, because it is almost not possible to press 2 different buttons on the remote within around 100 ms!!!
#define IRDATA_FLAGS_IS_AUTO_REPEAT     0x02 ///< The current repeat frame is a repeat, that is always sent after a regular frame and cannot be avoided. Only specified for protocols DENON, and LEGO.
#define IRDATA_FLAGS_PARITY_FAILED      0x04 ///< The current (autorepeat) frame violated parity check.
#define IRDATA_FLAGS_TOGGLE_BIT         0x08 ///< Is set if RC5 or RC6 toggle bit is set.
#define IRDATA_TOGGLE_BIT_MASK          0x08 ///< deprecated -is set if RC5 or RC6 toggle bit is set.
#define IRDATA_FLAGS_EXTRA_INFO         0x10 ///< There is extra info not contained in address and data (e.g. Kaseikyo unknown vendor ID, or in decodedRawDataArray).
#define IRDATA_FLAGS_WAS_OVERFLOW       0x40 ///< irparams.rawlen is set to 0 in this case to avoid endless OverflowFlag.
#define IRDATA_FLAGS_IS_MSB_FIRST       0x80 ///< Value is mainly determined by the (known) protocol.
#define IRDATA_FLAGS_IS_LSB_FIRST       0x00

#define RAW_DATA_ARRAY_SIZE             ((((RAW_BUFFER_LENGTH - 2) - 1) / (2 * BITS_IN_RAW_DATA_TYPE)) + 1) // The -2 is for initial gap + stop bit mark, 128 mark + spaces for 64 bit.
/**
 * Data structure for the user application, available as decodedIRData.
 * Filled by decoders and read by print functions or user application.
 */


typedef struct
{
    decode_type_t ProtocolIndex;
    uint8_t FrequencyKHz;
    struct DistanceWidthTimingInfoStruct DistanceWidthTimingInfo;
    uint8_t Flags;
    unsigned int RepeatPeriodMillis;
    void (*SpecialSendRepeatFunction)(); // using non member functions here saves up to 250 bytes for send demo
//    void (IRsend::*SpecialSendRepeatFunction)();
} PulseDistanceWidthProtocolConstants;
/*
 * Definitions for member PulseDistanceWidthProtocolConstants.Flags
 */
#define SUPPRESS_STOP_BIT_FOR_THIS_DATA 0x20
#define PROTOCOL_IS_MSB_FIRST           IRDATA_FLAGS_IS_MSB_FIRST
#define PROTOCOL_IS_LSB_FIRST           IRDATA_FLAGS_IS_LSB_FIRST
// 2 definitions for deprecated parameter bool aSendStopBit
#define SEND_STOP_BIT true
#define SEND_NO_STOP_BIT false

//#define USE_MSB_DECODING_FOR_DISTANCE_DECODER

/*
 * Carrier frequencies for various protocols
 */
#if !defined(BEO_KHZ) // guard used for unit test, which sends and receive Bang&Olufsen with 38 kHz.
#define BEO_KHZ         455
#endif
#define SONY_KHZ        40
#define BOSEWAVE_KHZ    38
#define DENON_KHZ       38
#define JVC_KHZ         38
#define LG_KHZ          38
#define NEC_KHZ         38
#define SAMSUNG_KHZ     38
#define KASEIKYO_KHZ    37
#define RC5_RC6_KHZ     36
//
#define C_ON (1)
#define C_OFF (0)
#define LOW		(0)
#define HIGH	(1)
#define sbi(str,bit) (str |= (1<<bit)) // set bit in register
#define cbi(str,bit) (str &= ~(1<<bit)) // clear bit in register
#define tbi(str,bit) (str ^= (1<<bit)) // toggle bit in register
#define bit_is_set(str,bit) (str & (1 << bit))
#define UNEQUAL (0)
#define EQUAL   (1)

#define CLEAR (0)
/*--- Define for time ---*/
#define TIME_WAITING_TO_SLEEP TIME_1M30S_BY_1S

#define TIME_2M_BY_1S (120)
#define TIME_1M30S_BY_1S (90)
#define TIME_60S_BY_1S (60)
#define TIME_20S_BY_1S (20)
#define TIME_10S_BY_1S (5)

#define TIME_1MS_BY_1MS (1)

#define TIME_20MS_BY_10MS (2)

#define TIME_10MS_BY_1MS (10)
#define TIME_100MS_BY_1MS (100)
#define TIME_1000MS_BY_1MS (1000)
#define TIME_200MS_BY_1MS (200)

#define TIME_2MIN_BY_100MS  (1200)
#define TIME_1MIN_BY_100MS  (600)
#define TIME_40S_BY_100MS   (400)
#define TIME_30S_BY_100MS  (300)
#define TIME_20S_BY_100MS  (200)
#define TIME_10S_BY_100MS  (100)
#define TIME_5S_BY_100MS  (50)
#define TIME_4S_BY_100MS  (40)
#define TIME_3S_BY_100MS  (30)
#define TIME_1S_BY_100MS  (10)
#define TIME_2S_BY_100MS  (20)
#define TIME_1S9_BY_100MS (19)
#define TIME_1S_BY_100MS (10)
#define TIME_900MS_BY_100MS (9)
#define TIME_500MS_BY_100MS (5)
#define TIME_300MS_BY_100MS (3)
#define TIME_200MS_BY_100MS (2)

#define TIME_20MS_BY_10MS	(2)


#define TIME_2S_BY_10MS  (200)
#define TIME_1S75_BY_10MS  (175)
#define TIME_1S5_BY_10MS  (150)
#define TIME_1S25_BY_10MS  (125)
#define TIME_1S_BY_10MS  (100)
#define TIME_500MS_BY_10MS  (50)
#define TIME_100MS_BY_10MS  (10)
#define TIME_50MS_BY_10MS  (5)

/*--- Define for Key ---*/
#define KEY_1CLICK_COUNT (3)
#define KEY_0_5SEC_COUNT (25)
#define KEY_1SEC_COUNT (50)
#define KEY_1_5SEC_COUNT (75)
#define KEY_2SEC_COUNT (100)
#define KEY_3SEC_COUNT (150)
#define KEY_5SEC_COUNT (250)
#define KEY_7SEC_COUNT (350)
#define KEY_CONTINUS_REFRESH_COUNT (10)
#define KEY_SHORTERROR_COUNT (1500)

#define KEY_OFF_DEBOUCE (1)
#define KEY_ON_DEBOUNCE (2)

#define SPECIAL_CHAR          (0xAA)

#define ADDRESS_ALARM_SIGN   (0x00)
#define ADDRESS_ALARM_1 (0x01)
#define ADDRESS_ALARM_2 (ADDRESS_ALARM_1 + 8)
#define ADDRESS_ALARM_3 (ADDRESS_ALARM_1 + 16)
#define ADDRESS_ALARM_4 (ADDRESS_ALARM_1 + 24)
#define ADDRESS_ALARM_5 (ADDRESS_ALARM_1 + 32)
#define ADDRESS_ALARM_6 (ADDRESS_ALARM_1 + 40)

#define ADDRESS_DOSE_POSITION_SIGN  (ADDRESS_ALARM_6+8+1)
#define ADDRESS_DOSE_VALUE          (ADDRESS_DOSE_POSITION_SIGN+1)

#define NUMBER_OF_ALARM (6)

#define STEP_MOTOR_PIN_1    (4)
#define STEP_MOTOR_PIN_2    (16)
#define STEP_MOTOR_PIN_3    (17)
#define STEP_MOTOR_PIN_4    (15)
#define DS1302_SCLK_PIN     (22)
#define DS1302_IO_PIN       (21)
#define DS1302_RST_PIN      (19)
#define BUZZER_PIN          (5)
#define LED_PIN             (13)

#define BUTTON_HOME_PIN         (34)
#define BUTTON_RESET_PIN        (35)
#define LOW_BATTERY_NOTI        (14)
#define LOW_BATTERY_SENSOR      (36)

#define LOW_BATTERY_VALUE       (4.5f)
#define TIME_ALARM_PADDING      (10)  // seconds

#define KEY_RESET   (1)
#define KEY_SWITCH_1	(1)
#define KEY_SWITCH_2	(2)

#define STEP_MOTOR_LOOP_TIME  (3) // millisecond per step - Change for speed of step motor
#define NUMBER_OF_STEPS (740)                    // number of sterp
// 2038 = 1 circle
#define MAX_STEP_MOTOR       (20000)//(16280*500)

#define TURN_RIGHT  (1)
#define TURN_LEFT   (2)

#define EEPROM_SIZE (256)

#define NUMBER_OF_TRY_CONNECT (3)

#define US_TO_S_FACTOR      (1000000)
#define S_TO_MINUTE_FACTOR  (60)

#define TIME_TO_SLEEP 5
#define MAX_TIME_SLEEP_MINUTES  (30) // minutes

#define MINUTES_OF_ONE_DAY  (1440)
#define MAX_TIME_ALARM  (MINUTES_OF_ONE_DAY * 7)

#define WAKEUP_NOT_BY_SLEEP       (0)
#define WAKEUP_BY_EXT0            (1)
#define WAKEUP_BY_EXT1            (2)
#define WAKEUP_BY_TIMER           (3)
#define WAKEUP_BY_TOUCHPAD        (4)
#define WAKEUP_BY_ULP             (5)
#define USE_AP_MODE           (1)
#define USE_STA_MOE           (2)
#define CURRENT_NETWORK_TYPE  USE_AP_MODE

#define DEV_MONIOTOR

// Define for variable

struct irparams_struct {
    // The fields are ordered to reduce memory over caused by struct-padding
    volatile uint8_t StateForISR;       ///< State Machine state
    uint8_t IRReceivePin;          ///< Pin connected to IR data from detector
#if defined(__AVR__)
    volatile uint8_t *IRReceivePinPortInputRegister;
    uint8_t IRReceivePinMask;
#endif
    volatile uint16_t TickCounterForISR; ///< Counts 50uS ticks. The value is copied into the rawbuf array on every transition.
#if !IR_REMOTE_DISABLE_RECEIVE_COMPLETE_CALLBACK
    void (*ReceiveCompleteCallbackFunction)(void); ///< The function to call if a protocol message has arrived, i.e. StateForISR changed to IR_REC_STATE_STOP
#endif
    bool OverflowFlag;                  ///< Raw buffer OverflowFlag occurred
#if RAW_BUFFER_LENGTH <= 254            // saves around 75 bytes program memory and speeds up ISR
    uint8_t rawlen;                ///< counter of entries in rawbuf
#else
    uint_fast16_t rawlen;               ///< counter of entries in rawbuf
#endif
    uint16_t rawbuf[RAW_BUFFER_LENGTH]; ///< raw data / tick counts per mark/space, first entry is the length of the gap between previous and current command
};

typedef struct{
    decode_type_t protocol; 				///< UNKNOWN, NEC, SONY, RC5, PULSE_DISTANCE, ...
    uint16_t address; 						///< Decoded address, Distance protocol (tMarkTicksLong (if tMarkTicksLong == 0, then tMarkTicksShort) << 8) | tSpaceTicksLong
    uint16_t command;       				///< Decoded command, Distance protocol (tMarkTicksShort << 8) | tSpaceTicksShort
    uint16_t extra; 						///< Contains upper 16 bit of Magiquest WandID, Kaseikyo unknown vendor ID and Distance protocol (HeaderMarkTicks << 8) | HeaderSpaceTicks.
    IRRawDataType decodedRawData; 			///< Up to 32/64 bit decoded raw data, to be used for send functions.
#if defined(DECODE_DISTANCE_WIDTH)
    										// This replaces the address, command, extra and decodedRawData in case of protocol == PULSE_DISTANCE or -rather seldom- protocol == PULSE_WIDTH.
    struct DistanceWidthTimingInfoStruct DistanceWidthTimingInfo; // 12 bytes
    IRRawDataType decodedRawDataArray[RAW_DATA_ARRAY_SIZE]; ///< 32/64 bit decoded raw data, to be used for send function.
#endif
    uint16_t numberOfBits; 					///< Number of bits received for data (address + command + parity) - to determine protocol length if different length are possible.
    uint8_t flags;          				///< IRDATA_FLAGS_IS_REPEAT, IRDATA_FLAGS_WAS_OVERFLOW etc. See IRDATA_FLAGS_* definitions above
    struct irparams_struct *rawDataPtr; 	///< Pointer of the raw timing data to be decoded. Mainly the OverflowFlag and the data buffer filled by receiving ISR.
} IRData;

typedef struct
{
	uint16_t ulStatus;
	uint16_t uwID;
	IRData IRData;
} IRData_t;

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

#define MARK   1
#define SPACE  0

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

/***************************************************************************/
/*					   			DEFINE FOR RTC							   */
/***************************************************************************/
typedef struct
{
    uint8_t second; // 0-59
    uint8_t minute; // 0-59
    uint8_t hour;   // 0-59
    uint8_t day;    // 1-31
    uint8_t month;  // 1-12
    uint16_t year;  // 1970-2038
	uint16_t milliseconds;
	uint8_t dayOfWeek;
} date_time_t;

enum
{
	MON_INDEX,
	TUE_INDEX,
	WED_INDEX,
	THUR_INDEX,
	FRI_INDEX,
	SAT_INDEX,
	SUN_INDEX
};

#define CONVERT_DAYS_TO_INDED(Days)		(7-Days)
/***************************************************************************/
/*					   	   DEFINE FOR SETTING IR						   */
/***************************************************************************/
#define CREATE_COMMAND  'c'
#define DELETE_COMMAND  'd'

enum
{
    NONE,
    MOTION_TRIGGER,
    ONE_TRIGGER,
    WINDOW_TRIGGER,
	TIMEOUT_TRIGGER
};

enum
{
	NONE_TYPE,
	SCHEDULING_TYPE,
	TIMEOUT_TYPE
};

enum
{
	IR_CODE_NONE,
    IR_CODE_DISABLE,
    IR_CODE_ENABLE
};

typedef struct
{
    uint8_t ubHour;
    uint8_t ubMin;
} HourMin_t;

typedef struct
{
    uint8_t ubMode;
    uint8_t uwTrigID;
    uint8_t uwIrCode;
    uint8_t ubDays;
    HourMin_t tTime1;
    HourMin_t tTime2;
    uint32_t ulTimeout;
} IrDataTrigger_t;

typedef enum
{
    COMMAND_INDEX = 0,
    TRIGGER_ID_INDEX,
    IR_CODE_INDEX,
    DAYS_INDEX,
    TIME1_INDEX,
	TIMEOUT_INDEX = TIME1_INDEX,
    TIME2_INDEX
} SchedulingIrTrig_t;

/***************************************************************************/
/*					   		DEFINE FOR ERROR CODE						   */
/***************************************************************************/
enum
{
	NO_ERROR,
	TIMEOUT_COMMAND_INVALID,
	TIMEOUT_TRIGGER_ID_INVALID,
	TIMEOUT_TRIGGER_IR_INVALID,
	TIMEOUT_DAYS_INVALID,
	TIMEOUT_INDEX_INVALID

};
/***************************************************************************/
/*					   	DEFINE FOR MOTION SENSOR				   		   */
/***************************************************************************/
#define MOTION_SENSOR_SENSITIVITY_LEVEL_NONE	(2)
#define MOTION_SENSOR_SENSITIVITY_LEVEL_0		(3)
#define MOTION_SENSOR_SENSITIVITY_LEVEL_1		(6)
#define MOTION_SENSOR_SENSITIVITY_LEVEL_2		(9)
#define MOTION_SENSOR_SENSITIVITY_LEVEL_3		(12)
#define MOTION_SENSOR_SENSITIVITY_LEVEL_4		(15)
#define CONVERT_SENSITIVITY_TO_VALUE(Sensitivity)	(15-Sensitivity+1)

enum
{
	SENSITIVITY_LEVEL_NONE,
	SENSITIVITY_LEVEL_0,
	SENSITIVITY_LEVEL_1,
	SENSITIVITY_LEVEL_2,
	SENSITIVITY_LEVEL_3,
	SENSITIVITY_LEVEL_4
};
/***************************************************************************/
/*					   DEFINE FOR KASEIKYO PROTOCOL						   */
/***************************************************************************/
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
/***************************************************************************/
/*					   DEFINE FOR PULSE DISTANCE PROTOCOL				   */
/***************************************************************************/
#define DURATION_ARRAY_SIZE 50
#if !defined(DISTANCE_WIDTH_MAXIMUM_REPEAT_DISTANCE_MICROS)
#define DISTANCE_WIDTH_MAXIMUM_REPEAT_DISTANCE_MICROS       100000 // 100 ms, bit it is just a guess
#endif
/***************************************************************************/
/*					   			DEFINE FOR NEC				   			   */
/***************************************************************************/
#define NEC_ADDRESS_BITS        16 // 16 bit address or 8 bit address and 8 bit inverted address
#define NEC_COMMAND_BITS        16 // Command and inverted command

#define NEC_BITS                (NEC_ADDRESS_BITS + NEC_COMMAND_BITS)
#define NEC_UNIT                560             // 21.28 periods of 38 kHz, 11.2 ticks TICKS_LOW = 8.358 TICKS_HIGH = 15.0

#define NEC_HEADER_MARK         (16 * NEC_UNIT) // 9000 | 180
#define NEC_HEADER_SPACE        (8 * NEC_UNIT)  // 4500 | 90

#define NEC_BIT_MARK            NEC_UNIT
#define NEC_ONE_SPACE           (3 * NEC_UNIT)  // 1690 | 33.8  TICKS_LOW = 25.07 TICKS_HIGH = 45.0
#define NEC_ZERO_SPACE          NEC_UNIT

#define NEC_REPEAT_HEADER_SPACE (4 * NEC_UNIT)  // 2250

#define NEC_AVERAGE_DURATION    62000 // NEC_HEADER_MARK + NEC_HEADER_SPACE + 32 * 2,5 * NEC_UNIT + NEC_UNIT // 2.5 because we assume more zeros than ones
#define NEC_MINIMAL_DURATION    49900 // NEC_HEADER_MARK + NEC_HEADER_SPACE + 32 * 2 * NEC_UNIT + NEC_UNIT // 2.5 because we assume more zeros than ones
#define NEC_REPEAT_DURATION     (NEC_HEADER_MARK  + NEC_REPEAT_HEADER_SPACE + NEC_BIT_MARK) // 12 ms
#define NEC_REPEAT_PERIOD       110000 // Commands are repeated every 110 ms (measured from start to start) for as long as the key on the remote control is held down.
#define NEC_REPEAT_DISTANCE         (NEC_REPEAT_PERIOD - NEC_AVERAGE_DURATION) // 48 ms
#define NEC_MAXIMUM_REPEAT_DISTANCE (NEC_REPEAT_PERIOD - NEC_MINIMAL_DURATION + 10000) // 70 ms

#define APPLE_ADDRESS           0x87EE
/***************************************************************************/
/*					   		DEFINE FOR DENON				   			   */
/***************************************************************************/
#define DENON_ADDRESS_BITS      5
#define DENON_COMMAND_BITS      8
#define DENON_FRAME_BITS        2 // 00/10 for 1. frame Denon/Sharp, inverted for autorepeat frame

#define DENON_BITS              (DENON_ADDRESS_BITS + DENON_COMMAND_BITS + DENON_FRAME_BITS) // 15 - The number of bits in the command
#define DENON_UNIT              260

#define DENON_BIT_MARK          DENON_UNIT  // The length of a Bit:Mark
#define DENON_ONE_SPACE         (7 * DENON_UNIT) // 1820 // The length of a Bit:Space for 1's
#define DENON_ZERO_SPACE        (3 * DENON_UNIT) // 780 // The length of a Bit:Space for 0's

#define DENON_AUTO_REPEAT_DISTANCE  45000 // Every frame is auto repeated with a space period of 45 ms and the command and frame inverted.
#define DENON_REPEAT_PERIOD        110000 // Commands are repeated every 110 ms (measured from start to start) for as long as the key on the remote control is held down.

// for old decoder
#define DENON_HEADER_MARK       DENON_UNIT // The length of the Header:Mark
#define DENON_HEADER_SPACE      (3 * DENON_UNIT) // 780 // The length of the Header:Space
/***************************************************************************/
/*					   			DEFINE FOR SONY				   			   */
/***************************************************************************/
#define SONY_ADDRESS_BITS       5
#define SONY_COMMAND_BITS       7
#define SONY_EXTRA_BITS         8
#define SONY_BITS_MIN           (SONY_COMMAND_BITS + SONY_ADDRESS_BITS)        // 12 bits
#define SONY_BITS_15            (SONY_COMMAND_BITS + SONY_ADDRESS_BITS + 3)    // 15 bits
#define SONY_BITS_MAX           (SONY_COMMAND_BITS + SONY_ADDRESS_BITS + SONY_EXTRA_BITS)    // 20 bits == SIRCS_20_PROTOCOL
#define SONY_UNIT               600 // 24 periods of 40kHz

#define SONY_HEADER_MARK        (4 * SONY_UNIT) // 2400
#define SONY_ONE_MARK           (2 * SONY_UNIT) // 1200
#define SONY_ZERO_MARK          SONY_UNIT
#define SONY_SPACE              SONY_UNIT

#define SONY_AVERAGE_DURATION_MIN   21000 // SONY_HEADER_MARK + SONY_SPACE  + 12 * 2,5 * SONY_UNIT  // 2.5 because we assume more zeros than ones
#define SONY_AVERAGE_DURATION_MAX   33000 // SONY_HEADER_MARK + SONY_SPACE  + 20 * 2,5 * SONY_UNIT  // 2.5 because we assume more zeros than ones
#define SONY_REPEAT_PERIOD          45000 // Commands are repeated every 45 ms (measured from start to start) for as long as the key on the remote control is held down.
#define SONY_MAXIMUM_REPEAT_DISTANCE    (SONY_REPEAT_PERIOD - SONY_AVERAGE_DURATION_MIN) // 24 ms
/***************************************************************************/
/*					   		DEFINE FOR RC5 RC6				   			   */
/***************************************************************************/
#define RC5_ADDRESS_BITS        5
#define RC5_COMMAND_BITS        6
#define RC5_COMMAND_FIELD_BIT   1
#define RC5_TOGGLE_BIT          1

#define RC5_BITS            (RC5_COMMAND_FIELD_BIT + RC5_TOGGLE_BIT + RC5_ADDRESS_BITS + RC5_COMMAND_BITS) // 13

#define RC5_UNIT            889 // 32 periods of 36 kHz (888.8888)

#define MIN_RC5_MARKS       ((RC5_BITS + 1) / 2) // 7. Divided by 2 to handle the bit sequence of 01010101 which gives one mark and space for each 2 bits

#define RC5_DURATION        (15L * RC5_UNIT) // 13335
#define RC5_REPEAT_PERIOD   (128L * RC5_UNIT) // 113792
#define RC5_REPEAT_DISTANCE (RC5_REPEAT_PERIOD - RC5_DURATION) // 100 ms
#define RC5_MAXIMUM_REPEAT_DISTANCE     (RC5_REPEAT_DISTANCE + (RC5_REPEAT_DISTANCE / 4)) // Just a guess

#define MIN_RC6_SAMPLES         1

#define RC6_RPT_LENGTH      46000

#define RC6_LEADING_BIT         1
#define RC6_MODE_BITS           3 // never seen others than all 0 for Philips TV
#define RC6_TOGGLE_BIT          1 // toggles at every key press. Can be used to distinguish repeats from 2 key presses and has another timing :-(.
#define RC6_TOGGLE_BIT_INDEX    RC6_MODE_BITS //  fourth position, index = 3
#define RC6_ADDRESS_BITS        8
#define RC6_COMMAND_BITS        8

#define RC6_BITS            (RC6_LEADING_BIT + RC6_MODE_BITS + RC6_TOGGLE_BIT + RC6_ADDRESS_BITS + RC6_COMMAND_BITS) // 21

#define RC6_UNIT            444 // 16 periods of 36 kHz (444.4444)

#define RC6_HEADER_MARK     (6 * RC6_UNIT) // 2666
#define RC6_HEADER_SPACE    (2 * RC6_UNIT) // 889

#define RC6_TRAILING_SPACE  (6 * RC6_UNIT) // 2666
#define MIN_RC6_MARKS       4 + ((RC6_ADDRESS_BITS + RC6_COMMAND_BITS) / 2) // 12, 4 are for preamble

#define RC6_REPEAT_DISTANCE 107000 // just a guess but > 2.666ms
#define RC6_MAXIMUM_REPEAT_DISTANCE     (RC6_REPEAT_DISTANCE + (RC6_REPEAT_DISTANCE / 4)) // Just a guess
/***************************************************************************/
/*					   			DEFINE FOR LG				   			   */
/***************************************************************************/
#define LG_ADDRESS_BITS          8
#define LG_COMMAND_BITS         16
#define LG_CHECKSUM_BITS         4
#define LG_BITS                 (LG_ADDRESS_BITS + LG_COMMAND_BITS + LG_CHECKSUM_BITS) // 28

#define LG_UNIT                 500 // 19 periods of 38 kHz

#define LG_HEADER_MARK          (18 * LG_UNIT) // 9000
#define LG_HEADER_SPACE         4200           // 4200 | 84

#define LG2_HEADER_MARK         (19 * LG_UNIT) // 9500
#define LG2_HEADER_SPACE        (6 * LG_UNIT)  // 3000

#define LG_BIT_MARK             LG_UNIT
#define LG_ONE_SPACE            1580  // 60 periods of 38 kHz
#define LG_ZERO_SPACE           550

#define LG_REPEAT_HEADER_SPACE  (4 * LG_UNIT)  // 2250
#define LG_REPEAT_PERIOD        110000 // Commands are repeated every 110 ms (measured from start to start) for as long as the key on the remote control is held down.
//#define LG_AVERAGE_DURATION     58000 // LG_HEADER_MARK + LG_HEADER_SPACE  + 32 * 2,5 * LG_UNIT) + LG_UNIT // 2.5 because we assume more zeros than ones
//#define LG_REPEAT_DURATION      (LG_HEADER_MARK  + LG_REPEAT_HEADER_SPACE + LG_BIT_MARK)
//#define LG_REPEAT_DISTANCE      (LG_REPEAT_PERIOD - LG_AVERAGE_DURATION) // 52 ms
/***************************************************************************/
/*					   			DEFINE FOR JVC				   			   */
/***************************************************************************/
#define JVC_ADDRESS_BITS      8 // 8 bit address
#define JVC_COMMAND_BITS      8 // Command

#define JVC_BITS              (JVC_ADDRESS_BITS + JVC_COMMAND_BITS) // 16 - The number of bits in the protocol
#define JVC_UNIT              526 // 20 periods of 38 kHz (526.315789)

#define JVC_HEADER_MARK       (16 * JVC_UNIT) // 8400
#define JVC_HEADER_SPACE      (8 * JVC_UNIT)  // 4200

#define JVC_BIT_MARK          JVC_UNIT        // The length of a Bit:Mark
#define JVC_ONE_SPACE         (3 * JVC_UNIT)  // 1578 - The length of a Bit:Space for 1's
#define JVC_ZERO_SPACE        JVC_UNIT        // The length of a Bit:Space for 0's

#define JVC_REPEAT_DISTANCE   (uint16_t)(45 * JVC_UNIT)  // 23625 - Commands are repeated with a distance of 23 ms for as long as the key on the remote control is held down.
#define JVC_REPEAT_PERIOD     65000 // assume around 40 ms for a JVC frame. JVC IR Remotes: RM-SA911U, RM-SX463U have 45 ms period
/***************************************************************************/
/*					   		DEFINE FOR SAMSUNG				   			   */
/***************************************************************************/
#define SAMSUNG_ADDRESS_BITS        16
#define SAMSUNG_COMMAND16_BITS      16
#define SAMSUNG_COMMAND32_BITS      32
#define SAMSUNG_BITS                (SAMSUNG_ADDRESS_BITS + SAMSUNG_COMMAND16_BITS)
#define SAMSUNG48_BITS              (SAMSUNG_ADDRESS_BITS + SAMSUNG_COMMAND32_BITS)

// except SAMSUNG_HEADER_MARK, values are like NEC
#define SAMSUNG_UNIT                560             // 21.28 periods of 38 kHz, 11.2 ticks TICKS_LOW = 8.358 TICKS_HIGH = 15.0
#define SAMSUNG_HEADER_MARK         (8 * SAMSUNG_UNIT) // 4500 | 180
#define SAMSUNG_HEADER_SPACE        (8 * SAMSUNG_UNIT) // 4500
#define SAMSUNG_BIT_MARK            SAMSUNG_UNIT
#define SAMSUNG_ONE_SPACE           (3 * SAMSUNG_UNIT) // 1690 | 33.8  TICKS_LOW = 25.07 TICKS_HIGH = 45.0
#define SAMSUNG_ZERO_SPACE          SAMSUNG_UNIT

#define SAMSUNG_AVERAGE_DURATION    55000 // SAMSUNG_HEADER_MARK + SAMSUNG_HEADER_SPACE  + 32 * 2,5 * SAMSUNG_UNIT + SAMSUNG_UNIT // 2.5 because we assume more zeros than ones
#define SAMSUNG_REPEAT_DURATION     (SAMSUNG_HEADER_MARK  + SAMSUNG_HEADER_SPACE + SAMSUNG_BIT_MARK + SAMSUNG_ZERO_SPACE + SAMSUNG_BIT_MARK)
#define SAMSUNG_REPEAT_PERIOD       110000 // Commands are repeated every 110 ms (measured from start to start) for as long as the key on the remote control is held down.
#define SAMSUNG_REPEAT_DISTANCE     (SAMSUNG_REPEAT_PERIOD - SAMSUNG_AVERAGE_DURATION)
#define SAMSUNG_MAXIMUM_REPEAT_DISTANCE     (SAMSUNG_REPEAT_DISTANCE + (SAMSUNG_REPEAT_DISTANCE / 4)) // Just a guess
/***************************************************************************/
/*					   	DEFINE FOR BANGOLUFSEN				   			   */
/***************************************************************************/
#define BEO_DATA_BITS         8                // Command or character

#define BEO_UNIT              3125             // All timings are in microseconds

#define BEO_IR_MARK           200              // The length of a mark in the IR protocol
#define BEO_DATALINK_MARK     (BEO_UNIT / 2)   // The length of a mark in the Datalink protocol

#define BEO_PULSE_LENGTH_ZERO           BEO_UNIT      // The length of a one to zero transition
#define BEO_PULSE_LENGTH_EQUAL          (2 * BEO_UNIT)   // 6250 The length of an equal bit
#define BEO_PULSE_LENGTH_ONE            (3 * BEO_UNIT)   // 9375 The length of a zero to one transition
#define BEO_PULSE_LENGTH_TRAILING_BIT   (4 * BEO_UNIT)   // 12500 The length of the stop bit
#define BEO_PULSE_LENGTH_START_BIT      (5 * BEO_UNIT)   // 15625 The length of the start bit
/***************************************************************************/
/*					   			DEFINE FOR FAST				   			   */
/***************************************************************************/
#define FAST_KHZ                  38
#define FAST_ADDRESS_BITS          0 // No address
#define FAST_COMMAND_BITS         16 // Command and inverted command (parity)
#define FAST_BITS                 (FAST_ADDRESS_BITS + FAST_COMMAND_BITS)

#define FAST_UNIT                 526 // 20 periods of 38 kHz (526.315789)

#define FAST_BIT_MARK             FAST_UNIT
#define FAST_ONE_SPACE            (3 * FAST_UNIT)     // 1578 -> bit period = 2104
#define FAST_ZERO_SPACE           FAST_UNIT           //  526 -> bit period = 1052

#define FAST_HEADER_MARK          (4 * FAST_UNIT)     // 2104
#define FAST_HEADER_SPACE         (2 * FAST_UNIT)     // 1052

#define FAST_REPEAT_PERIOD        50000 // Commands are repeated every 50 ms (measured from start to start) for as long as the key on the remote control is held down.
#define FAST_REPEAT_DISTANCE      (FAST_REPEAT_PERIOD - (55 * FAST_UNIT)) // 19 ms
#define FAST_MAXIMUM_REPEAT_DISTANCE (FAST_REPEAT_DISTANCE + 10000) // 29 ms
/***************************************************************************/
/*					   		DEFINE FOR WHYNTER				   			   */
/***************************************************************************/
#define WHYNTER_BITS            32
#define WHYNTER_HEADER_MARK   2850
#define WHYNTER_HEADER_SPACE  2850
#define WHYNTER_BIT_MARK       750
#define WHYNTER_ONE_SPACE     2150
#define WHYNTER_ZERO_SPACE     750
/***************************************************************************/
/*					   			DEFINE FOR LEGO				   			   */
/***************************************************************************/
#define LEGO_CHANNEL_BITS       4
#define LEGO_MODE_BITS          4
#define LEGO_COMMAND_BITS       4
#define LEGO_PARITY_BITS        4

#define LEGO_BITS               (LEGO_CHANNEL_BITS + LEGO_MODE_BITS + LEGO_COMMAND_BITS + LEGO_PARITY_BITS)

#define LEGO_HEADER_MARK        158    //  6 cycles
#define LEGO_HEADER_SPACE       1026   // 39 cycles

#define LEGO_BIT_MARK           158    //  6 cycles
#define LEGO_ONE_SPACE          553    // 21 cycles
#define LEGO_ZERO_SPACE         263    // 10 cycles

#define LEGO_AVERAGE_DURATION   11000 // LEGO_HEADER_MARK + LEGO_HEADER_SPACE  + 16 * 600 + 158

#define LEGO_AUTO_REPEAT_PERIOD_MIN 110000 // Every frame is auto repeated 5 times.
#define LEGO_AUTO_REPEAT_PERIOD_MAX 230000 // space for channel 3

#define LEGO_MODE_EXTENDED  0
#define LEGO_MODE_COMBO     1
#define LEGO_MODE_SINGLE    0x4 // here the 2 LSB have meanings like Output A / Output B
/***************************************************************************/
/*					   		DEFINE FOR BOSEWAVE				   			   */
/***************************************************************************/
#define BOSEWAVE_BITS             16 // Command and inverted command

#define BOSEWAVE_HEADER_MARK    1014    // 1014 are 39 clock periods (I counted 3 times!)
#define BOSEWAVE_HEADER_SPACE   1468    // 1468(measured), 1456 are 56 clock periods
#define BOSEWAVE_BIT_MARK        520    // 520 are 20 clock periods
#define BOSEWAVE_ZERO_SPACE      468    // 468 are 18 clock periods
#define BOSEWAVE_ONE_SPACE      1468    // 1468(measured), 1456 are 56 clock periods

#define BOSEWAVE_REPEAT_PERIOD              75000
#define BOSEWAVE_REPEAT_DISTANCE            50000
#define BOSEWAVE_MAXIMUM_REPEAT_DISTANCE    62000
/***************************************************************************/
/*					   		DEFINE FOR MAGIQUEST				   		   */
/***************************************************************************/
#define MAGIQUEST_CHECKSUM_BITS     8   // magiquest_t.cmd.checksum
#define MAGIQUEST_MAGNITUDE_BITS    9   // magiquest_t.cmd.magnitude
#define MAGIQUEST_WAND_ID_BITS     31   // magiquest_t.cmd.wand_id -> wand-id is handled as 32 bit and always even
#define MAGIQUEST_START_BITS        8    // magiquest_t.cmd.StartBits

#define MAGIQUEST_PERIOD         1150   // Time for a full MagiQuest "bit" (1100 - 1200 usec)

#define MAGIQUEST_DATA_BITS     (MAGIQUEST_CHECKSUM_BITS + MAGIQUEST_MAGNITUDE_BITS + MAGIQUEST_WAND_ID_BITS) // 48 Size of the command without the start bits
#define MAGIQUEST_BITS          (MAGIQUEST_CHECKSUM_BITS + MAGIQUEST_MAGNITUDE_BITS + MAGIQUEST_WAND_ID_BITS + MAGIQUEST_START_BITS) // 56 Size of the command with the start bits

#define MAGIQUEST_UNIT          (MAGIQUEST_PERIOD / 4) // 287.5

#define MAGIQUEST_ONE_MARK      (2 * MAGIQUEST_UNIT) // 576
#define MAGIQUEST_ONE_SPACE     (2 * MAGIQUEST_UNIT) // 576
#define MAGIQUEST_ZERO_MARK     MAGIQUEST_UNIT       // 287.5
#define MAGIQUEST_ZERO_SPACE    (3 * MAGIQUEST_UNIT) // 864
/***************************************************************************/
/*					   			END OF DEFINE							   */
/***************************************************************************/
#endif // _OP_HEADER_DEFINE_H_
