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
#define MAX_IR_CODE							64
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
	uint16_t uwID;
	IRData IRData;
} IRData_t;

#endif // _OP_HEADER_DEFINE_H_
