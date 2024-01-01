#include "hw_tests.h"

volatile uint32_t i;

void test_rtc(void)
{
	while(1) {
		if(NRF_RTC0->COUNTER & (1 << 14)) {
			NRF_P0->OUTSET = (1 << PIN_RED_LED);
		}
		else {
			NRF_P0->OUTCLR = (1 << PIN_RED_LED);
		}
	}
}

void test_uart(void)
{
	static char hello_world[] = "Hello World!\n";
	static uint8_t state = 0;
	// Configure the UARTE with no flow control, one parity bit and 230400 baud rate
	NRF_UARTE0->CONFIG = 0; // no hardware flow control, no parity bit, one stop bit
	NRF_UARTE0->BAUDRATE = UARTE_BAUDRATE_BAUDRATE_Baud230400 << UARTE_BAUDRATE_BAUDRATE_Pos;

	// Select TX and RX pins
	NRF_UARTE0->PSEL.TXD = PIN_TXD;
	NRF_UARTE0->PSEL.RXD = PIN_RXD;

	// Enable the UART (starts using the TX/RX pins)
	NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Enabled << UARTE_ENABLE_ENABLE_Pos;

  	while (1) {
		if(state) {
			NRF_P0->OUTCLR = (1 << PIN_GREEN_LED);
		}
		else {
			NRF_P0->OUTSET = (1 << PIN_GREEN_LED);
		}
		state = 1 - state;
		// Configure transmit buffer and start the transfer
		NRF_UARTE0->TXD.MAXCNT = strlen(hello_world);
		NRF_UARTE0->TXD.PTR = (uint32_t)hello_world;
		NRF_UARTE0->TASKS_STARTTX = 1;
		// Wait until the transfer is complete
		NRF_P0->OUTCLR = (1 << PIN_RED_LED);
		while(NRF_UARTE0->EVENTS_ENDTX == 0);
		NRF_P0->OUTSET = (1 << PIN_RED_LED);
				NRF_UARTE0->EVENTS_ENDTX = 0;
		// Stop the UART TX
		//		NRF_UARTE0->TASKS_STOPTX = 1;
		// Wait until we receive the stopped event
		//		while(NRF_UARTE0->EVENTS_TXSTOPPED == 0);
		//                NRF_UARTE0->EVENTS_TXSTOPPED = 0;
		// A 100ms delay
		delay_ticks(10000);

		// Disable the UARTE (pins are now available for other use)
		//		NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Disabled << UARTE_ENABLE_ENABLE_Pos;
	}
}


// an infinint while loop and inside we just make the red light on if the switch1 is on and we make the green led in if the switch2 is on
void test_buttons_and_leds(void)
{
	while(1)
	{
		delay_ticks(1000);

		if(NRF_P0->IN & (1 << PIN_SWITCH1))
		{
			printf("Green On\n");
			NRF_P0->OUTCLR = (1 << PIN_GREEN_LED);
		}
		else {
			printf("Green Off\n");
			NRF_P0->OUTSET = (1 << PIN_GREEN_LED);
		}

		if(NRF_P0->IN & (1 << PIN_SWITCH2))
		{
			printf("Red On\n");
			NRF_P0->OUTCLR = (1 << PIN_RED_LED);
		}
		else {
			printf("Red Off\n");
			NRF_P0->OUTSET = (1 << PIN_RED_LED);
		}
	}
}

void test_ir_LED_toggle_continuously(void)
{
	while(1)
	{
		IR_led_toggle();
		delay_ticks(16000);
	}
}


uint8_t debug_uart_first_time = 0;

// Initialize the debug UART so it can output debug information at a baud rate of 230400
void init_debug_uart(void)
{
	// Configure the UARTE with no flow control, one parity bit and 230400 baud rate
	NRF_UARTE0->CONFIG = 0; // no hardware flow control, no parity bit, one stop bit
	NRF_UARTE0->BAUDRATE = UARTE_BAUDRATE_BAUDRATE_Baud115200 << UARTE_BAUDRATE_BAUDRATE_Pos;

	// Select TX and RX pins
	NRF_UARTE0->PSEL.TXD = PIN_TXD;
	NRF_UARTE0->PSEL.RXD = PIN_RXD;

	// Enable the UART (starts using the TX/RX pins)
	NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Enabled << UARTE_ENABLE_ENABLE_Pos;
	debug_uart_first_time = 1;
}


void disable_debug_uart(void)
{
	// Stop the UART TX
	NRF_UARTE0->EVENTS_TXSTOPPED = 0;
	NRF_UARTE0->TASKS_STOPTX = 1;
	// Wait until we receive the stopped event
	while(NRF_UARTE0->EVENTS_TXSTOPPED == 0);
	// Disable the UARTE (pins are now available for other use)
	NRF_UARTE0->ENABLE = UARTE_ENABLE_ENABLE_Disabled << UARTE_ENABLE_ENABLE_Pos;
}


// Prints out a message to the UART. Optionally, print out an unsigned 16 bit number. Optionally, print out a newline.
void print_debug_message(char *message, uint16_t number, uint8_t print_number, uint8_t newline)
{
	#define MAX_DEBUG_PRINT_BUFFER 200
	static char buf[MAX_DEBUG_PRINT_BUFFER + 1];

	if(debug_uart_first_time) {
		debug_uart_first_time = 0;
	}
	else {
		// If the debug UART is still transmitting a massage, wait until it is done
		while(NRF_UARTE0->EVENTS_ENDTX == 0);
	}

	NRF_UARTE0->EVENTS_ENDTX = 0;

	if(print_number) {
		if(newline) {
			snprintf(buf, MAX_DEBUG_PRINT_BUFFER, "%s%d\n", message, number);
		}
		else {
			snprintf(buf, MAX_DEBUG_PRINT_BUFFER, "%s%d", message, number);
		}
	}
	else {
		if(newline) {
			snprintf(buf, MAX_DEBUG_PRINT_BUFFER, "%s\n", message);
		}
		else {
			snprintf(buf, MAX_DEBUG_PRINT_BUFFER, "%s", message);
		}
	}

	// Configure transmit buffer and start the transfer
	NRF_UARTE0->TXD.MAXCNT = strlen(buf);
	NRF_UARTE0->TXD.PTR = (uint32_t)buf;
	NRF_UARTE0->TASKS_STARTTX = 1;
}


void test_print_debug_message(void)
{
	uint16_t number = 0;
	while(1) {
		print_debug_message("No number here: ", number, 0, 1);
		delay_ticks(1000);
		print_debug_message("Test number: ", number, 1, 1);
		delay_ticks(1000);
		number++;
		red_led_toggle();
	}
}


void enable_reset_pin(void)
{
	if ((NRF_UICR->PSELRESET[0] != 21) || (NRF_UICR->PSELRESET[1] != 21)) {
		uint32_t p = NRF_UICR->PSELRESET[0];
		uint16_t msb = (p >> 16) & 0xFFFF;
		uint16_t lsb = p & 0xFFFF;
		print_debug_message("PSELRESET[0] MSB: ", msb, 1, 1);
		delay_ticks(1600);
		print_debug_message("PSELRESET[0] LSB: ", lsb, 1, 1);
		delay_ticks(1600);
		p = NRF_UICR->PSELRESET[1];
		msb = (p >> 16) & 0xFFFF;
		lsb = p & 0xFFFF;
		print_debug_message("PSELRESET[1] MSB: ", msb, 1, 1);
		delay_ticks(1600);
		print_debug_message("PSELRESET[1] LSB: ", lsb, 1, 1);
		delay_ticks(1600);

		NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
		NRF_UICR->PSELRESET[0] = 21;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
		NRF_UICR->PSELRESET[1] = 21;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
		NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}

		delay_ticks(32000);

		NVIC_SystemReset();
	}
}


// the clock to the PWM is 16MHz and we have a prescalar of 1
#define PWM_PERIOD 421 // 38 kHz
#define PWM_50_PERCENT_DUTY_CYCLE (PWM_PERIOD / 2)
#define PWM_CH0_50_PERCENT_DUTY_CYCLE PWM_50_PERCENT_DUTY_CYCLE
#define POLARITY_NORMAL 0
#define POLARITY_REVERSED (1 << 15)
static uint16_t pwm_seq0[] = {POLARITY_NORMAL | PWM_CH0_50_PERCENT_DUTY_CYCLE, 0, 0, 0};
static uint16_t pwm_seq1[] = {POLARITY_NORMAL | PWM_PERIOD, 0, 0, 0};

void init_pwm(void)
{
//	NRF_PWM0->PSEL.OUT[0] = (PIN_BLUE_LED    << PWM_PSEL_OUT_PIN_Pos) | (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);
	NRF_PWM0->PSEL.OUT[0] = (PIN_IR_TRANSMIT << PWM_PSEL_OUT_PIN_Pos) | (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);
//	NRF_PWM0->PSEL.OUT[1] = (PIN_GREEN_LED << PWM_PSEL_OUT_PIN_Pos) | (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);
	NRF_PWM0->ENABLE = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);
	NRF_PWM0->MODE = (PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos);
	NRF_PWM0->PRESCALER = (PWM_PRESCALER_PRESCALER_DIV_1 << PWM_PRESCALER_PRESCALER_Pos);
	NRF_PWM0->COUNTERTOP  = PWM_PERIOD;
	NRF_PWM0->LOOP = 1; // this means that it will run 2 sequences, which are sequences 0 and 1
	NRF_PWM0->DECODER = (PWM_DECODER_LOAD_Individual << PWM_DECODER_LOAD_Pos) | (PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos); // will load all 4 PWM channel values at each step and the next step will be triggered automatically after the previous step has been completed
	NRF_PWM0->SEQ[0].PTR = (uint32_t)pwm_seq0;
	NRF_PWM0->SEQ[0].CNT = sizeof(pwm_seq0) / sizeof(uint16_t);
//	NRF_PWM0->SEQ[0].REFRESH = 38000;
	NRF_PWM0->SEQ[0].ENDDELAY = 0;
	NRF_PWM0->SEQ[1].PTR = (uint32_t)pwm_seq1;
	NRF_PWM0->SEQ[1].CNT = sizeof(pwm_seq1) / sizeof(uint16_t);
//	NRF_PWM0->SEQ[1].REFRESH = 38000;
	NRF_PWM0->SEQ[1].ENDDELAY = 0;
//	NRF_PWM0->TASKS_SEQSTART[0] = 1;
}


#define LEVEL1 0
#define LEVEL2 PWM_50_PERCENT_DUTY_CYCLE
#define LEVEL_DONT_CARE 0
#define PWM_CH0_DUTY_A LEVEL1
#define PWM_CH1_DUTY_A LEVEL2
#define PWM_CH2_DUTY_A LEVEL_DONT_CARE
#define PWM_CH3_DUTY_A LEVEL_DONT_CARE
#define PWM_CH0_DUTY_B LEVEL2
#define PWM_CH1_DUTY_B LEVEL1
#define PWM_CH2_DUTY_B LEVEL_DONT_CARE
#define PWM_CH3_DUTY_B LEVEL_DONT_CARE
#define LEVEL3 0
#define LEVEL4 PWM_50_PERCENT_DUTY_CYCLE
#define PWM_CH0_DUTY_C LEVEL3
#define PWM_CH1_DUTY_C LEVEL4
#define PWM_CH2_DUTY_C LEVEL_DONT_CARE
#define PWM_CH3_DUTY_C LEVEL_DONT_CARE
#define PWM_CH0_DUTY_D LEVEL4
#define PWM_CH1_DUTY_D LEVEL3
#define PWM_CH2_DUTY_D LEVEL_DONT_CARE
#define PWM_CH3_DUTY_D LEVEL_DONT_CARE

void test_pwm(void)
{
	static uint16_t pwm_seq0[] = {PWM_CH0_DUTY_A, PWM_CH1_DUTY_A, PWM_CH2_DUTY_A, PWM_CH3_DUTY_A, PWM_CH0_DUTY_B, PWM_CH1_DUTY_B, PWM_CH2_DUTY_B, PWM_CH3_DUTY_B};
	static uint16_t pwm_seq1[] = {PWM_CH0_DUTY_C, PWM_CH1_DUTY_C, PWM_CH2_DUTY_C, PWM_CH3_DUTY_C, PWM_CH0_DUTY_D, PWM_CH1_DUTY_D, PWM_CH2_DUTY_D, PWM_CH3_DUTY_D};
	NRF_PWM0->PSEL.OUT[0] = (PIN_RED_LED << PWM_PSEL_OUT_PIN_Pos) | (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);
	NRF_PWM0->PSEL.OUT[1] = (PIN_GREEN_LED << PWM_PSEL_OUT_PIN_Pos) | (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);
	NRF_PWM0->ENABLE = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);
	NRF_PWM0->MODE = (PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos);
	NRF_PWM0->PRESCALER = (PWM_PRESCALER_PRESCALER_DIV_1 << PWM_PRESCALER_PRESCALER_Pos);
	NRF_PWM0->COUNTERTOP  = PWM_PERIOD;
//	NRF_PWM0->COUNTERTOP  = 0x7fff; // 488 Hz
	NRF_PWM0->LOOP = 1; // this means that it will run 2 sequences, which are sequences 0 and 1
	NRF_PWM0->DECODER = (PWM_DECODER_LOAD_Individual << PWM_DECODER_LOAD_Pos) | (PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos); // will load all 4 PWM channel values at each step and the next step will be triggered automatically after the previous step has been completed
	NRF_PWM0->SEQ[0].PTR = (uint32_t)pwm_seq0;
	NRF_PWM0->SEQ[0].CNT = sizeof(pwm_seq0) / sizeof(uint16_t);
	NRF_PWM0->SEQ[0].REFRESH = 38000;
//	NRF_PWM0->SEQ[0].REFRESH = 400;
	NRF_PWM0->SEQ[0].ENDDELAY = 0;
	NRF_PWM0->SEQ[1].PTR = (uint32_t)pwm_seq1;
	NRF_PWM0->SEQ[1].CNT = sizeof(pwm_seq1) / sizeof(uint16_t);
	NRF_PWM0->SEQ[1].REFRESH = 38000;
	NRF_PWM0->SEQ[1].ENDDELAY = 0;
	NRF_PWM0->TASKS_SEQSTART[0] = 1;
	while(1);
}


void test_pwm_pulsing(void)
{
	init_pwm();
	while(1) {
		NRF_PWM0->SEQ[0].REFRESH = 38000;
		NRF_PWM0->SEQ[1].REFRESH = 38000;
		NRF_PWM0->EVENTS_LOOPSDONE = 0;
		NRF_PWM0->TASKS_SEQSTART[0] = 1;
		while(NRF_PWM0->EVENTS_LOOPSDONE == 0);
	}
}

void test_ir_receive(void)
{
	IR_receiver_power_on();
	while(1) {
		if(NRF_P0->IN & (1 << PIN_IR_RECEIVE)) {
			NRF_P0->OUTSET = (1 << PIN_RED_LED);
		}
		else {
			NRF_P0->OUTCLR = (1 << PIN_RED_LED);
		}
	}
}

#define N_IR_SIGNAL_TRANSITION_DATAPOINTS 100 // this needs to be an odd number
#define IR_IDLE_MAX_TIME (32768 / 5) // one fifth of a second
#define IR_ACTIVE_MAX_TIME (32768 / 2)  // one half of a second

static uint16_t captured_ir_signal_transition_time[N_IR_SIGNAL_TRANSITION_DATAPOINTS];
static uint16_t captured_ir_signal_transition_time_index = 0;
#define RECEIVER_LINE_STATE_IDLE 1
#define RECEIVER_LINE_STATE_ACTIVE 0
static uint8_t receiver_line_state = RECEIVER_LINE_STATE_IDLE;


void record_ir_pattern(void)
{
	uint16_t i;
	uint16_t previous_recorded_time = 0;
	uint16_t new_recorded_time = 0;

	NRF_LOG_INFO("Recording IR pattern");

	captured_ir_signal_transition_time_index = 0;
	receiver_line_state = RECEIVER_LINE_STATE_IDLE; // the idle state is high
  	while (1) {
		new_recorded_time = NRF_RTC0->COUNTER;
		if( (receiver_line_state == RECEIVER_LINE_STATE_IDLE) && ((NRF_P0->IN & (1 << PIN_IR_RECEIVE)) == RECEIVER_LINE_STATE_ACTIVE) ) {
			captured_ir_signal_transition_time[captured_ir_signal_transition_time_index++] = new_recorded_time;
			previous_recorded_time = new_recorded_time;
			receiver_line_state = RECEIVER_LINE_STATE_ACTIVE;
			green_led_on();
		}
		else if( (receiver_line_state == RECEIVER_LINE_STATE_ACTIVE) && ((NRF_P0->IN & (1 << PIN_IR_RECEIVE)) != RECEIVER_LINE_STATE_ACTIVE) ) {
			captured_ir_signal_transition_time[captured_ir_signal_transition_time_index++] = new_recorded_time;
			previous_recorded_time = new_recorded_time;
			receiver_line_state = RECEIVER_LINE_STATE_IDLE;
			green_led_off();
		}
		if(captured_ir_signal_transition_time_index >= N_IR_SIGNAL_TRANSITION_DATAPOINTS) {
			break;
		}
		if(captured_ir_signal_transition_time_index > 0) {
			uint16_t time_difference = new_recorded_time - previous_recorded_time;
			if(receiver_line_state == RECEIVER_LINE_STATE_IDLE) {
				if(time_difference > IR_IDLE_MAX_TIME) {
					break;
				}
			}
			else {
				if(time_difference > IR_ACTIVE_MAX_TIME) {
					captured_ir_signal_transition_time[captured_ir_signal_transition_time_index++] = new_recorded_time;
					break;
				}
			}
		}
	}
	green_led_off();

	// calculate the time differences for each transition vs. the previous
	captured_ir_signal_transition_time_index--;
	for(i = 0; i < captured_ir_signal_transition_time_index; i++) {
		uint32_t time_value = captured_ir_signal_transition_time[i + 1] - captured_ir_signal_transition_time[i]; // calculate the time difference
		time_value = time_value * 38000 / 32768; // convert the time into the units of the PWM module
		captured_ir_signal_transition_time[i] = time_value;
	}

	// Now, print out all the collected IR signal timing pattern
	for(i = 0; i < captured_ir_signal_transition_time_index; i++) {
		print_debug_message("Index: ", i, 1, 0);
		if((i & 1) == 0) {
			NRF_LOG_INFO("   IR on time:  %d", captured_ir_signal_transition_time[i]);
		}
		else {
			NRF_LOG_INFO("   IR off time: ", captured_ir_signal_transition_time[i]);
		}
	}
}


void playback_ir_pattern(void)
{
	uint32_t i;

	for(i = 0; i < captured_ir_signal_transition_time_index; i += 2) {
		NRF_PWM0->SEQ[0].REFRESH = captured_ir_signal_transition_time[i]; // set the LED on time
		NRF_PWM0->SEQ[1].REFRESH = captured_ir_signal_transition_time[i + 1]; // set the LED off time
		NRF_PWM0->EVENTS_LOOPSDONE = 0;
		NRF_PWM0->TASKS_SEQSTART[0] = 1;
		while(NRF_PWM0->EVENTS_LOOPSDONE == 0);
	}
}


#ifdef TEST_BLUETOOTH

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

/*
 * Set Advertisement data. Based on the Eddystone specification:
 * https://github.com/google/eddystone/blob/master/protocol-specification.md
 * https://github.com/google/eddystone/tree/master/eddystone-url
 */
static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0xaa, 0xfe),
	BT_DATA_BYTES(BT_DATA_SVC_DATA16,
		      0xaa, 0xfe, /* Eddystone UUID */
		      0x10, /* Eddystone-URL frame type */
		      0x00, /* Calibrated Tx power at 0m */
		      0x00, /* URL Scheme Prefix http://www. */
		      'z', 'e', 'p', 'h', 'y', 'r',
		      'p', 'r', 'o', 'j', 'e', 'c', 't',
		      0x08) /* .org */
};

/* Set Scan Response data */
static const struct bt_data sd[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static void bt_ready(int err)
{
	char addr_s[BT_ADDR_LE_STR_LEN];
	bt_addr_le_t addr = {0};
	size_t count = 1;

	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");

	/* Start advertising */
	err = bt_le_adv_start(BT_LE_ADV_NCONN_IDENTITY, ad, ARRAY_SIZE(ad),
			      sd, ARRAY_SIZE(sd));
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}


	/* For connectable advertising you would use
	 * bt_le_oob_get_local().  For non-connectable non-identity
	 * advertising an non-resolvable private address is used;
	 * there is no API to retrieve that.
	 */

	bt_id_get(&addr, &count);
	bt_addr_le_to_str(&addr, addr_s, sizeof(addr_s));

	printk("Beacon started, advertising as %s\n", addr_s);
}

void test_bluetooth(void)
{
	int err;

	printk("Starting Beacon Demo\n");

	/* Initialize the Bluetooth Subsystem */
	err = bt_enable(bt_ready);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
	}
	while(1);
}


#endif

void test_ble(void)
{
	//print_debug_message("Starting BLE test", 0, 0, 1);

	/* Initialize the Bluetooth Subsystem */
	lib_ble_init();
	lib_ble_advertising_start();
	while(1){
            lib_ble_process();
        }
}

void init_adc(void)
{


}


// This will test the ADC by continuously taking readings from the PIR sensor (connected to pin 5 / AIN3) and print them out to the debug UART 
void test_PIR_motion_sensor(void)
{
	uint16_t adc_sample;
	#define MAX_ADC_SAMPLE_SCALED 132
	char bars[MAX_ADC_SAMPLE_SCALED + 4];
	uint32_t adc_sample_average = 0;
	uint16_t adc_trigger_threshold = 1500;
	uint32_t i;

//	init_debug_uart();
	print_debug_message("Starting ADC test", 0, 0, 1);

	NRF_SAADC->ENABLE = 1; // enable the ADC
	NRF_SAADC->CH[0].PSELP = 4; // choose AIN3 as the input
	NRF_SAADC->CH[0].CONFIG = (SAADC_CH_CONFIG_GAIN_Gain1_6 << SAADC_CH_CONFIG_GAIN_Pos) | (SAADC_CH_CONFIG_REFSEL_Internal << SAADC_CH_CONFIG_REFSEL_Pos) |
	                          (SAADC_CH_CONFIG_TACQ_40us << SAADC_CH_CONFIG_TACQ_Pos); // gain set to 1/6, internal reference (0.6V), 40us aquisition time, single ended conversion, no burst
	NRF_SAADC->RESOLUTION = SAADC_RESOLUTION_VAL_14bit; // set the resolution to 14 bits 
	NRF_SAADC->OVERSAMPLE = SAADC_OVERSAMPLE_OVERSAMPLE_Bypass; // no oversampling
	NRF_SAADC->SAMPLERATE = SAADC_SAMPLERATE_MODE_Task << SAADC_SAMPLERATE_MODE_Pos; // use the task to start sampling

	// calibration the offset
//	NRF_SAADC->EVENTS_CALIBRATEDONE = 0; // clear the event
//	NRF_SAADC->TASKS_CALIBRATEOFFSET = 1; // start the offset calibration
//	while(NRF_SAADC->EVENTS_CALIBRATEDONE == 0); // wait for the offset calibration to finish

	while(1) {
		uint16_t adc_trigger_upper_limit;
		uint16_t adc_trigger_lower_limit;
		adc_trigger_upper_limit = adc_sample_average + adc_trigger_threshold;
		if(adc_sample_average < adc_trigger_threshold) {
			adc_trigger_lower_limit = 0;
		} else {
			adc_trigger_lower_limit = adc_sample_average - adc_trigger_threshold;
		}
		NRF_SAADC->CH[0].LIMIT = (adc_trigger_upper_limit << 16) | adc_trigger_lower_limit; // set the lower and upper limit
		NRF_SAADC->RESULT.MAXCNT = 1; // set the max number of samples to 1
		NRF_SAADC->RESULT.PTR = (uint32_t)&adc_sample; // set the pointer to the sample buffer
		NRF_SAADC->EVENTS_STARTED = 0; // clear the started event
		NRF_SAADC->TASKS_START = 1; // start the ADC
		while(NRF_SAADC->EVENTS_STARTED == 0); // wait for the ADC to start
		NRF_SAADC->EVENTS_END = 0; // clear the event
		NRF_SAADC->TASKS_SAMPLE = 1; // start sampling
		while(NRF_SAADC->EVENTS_END == 0); // wait for the sampling to finish
		NRF_SAADC->TASKS_STOP = 1; // stop the ADC
		if(adc_sample > 32767) {
			adc_sample = 0;
		}
		#define ADC_AVERAGING_SHIFT 6
		adc_sample_average = ((adc_sample_average * ((1 << ADC_AVERAGING_SHIFT) - 1)) + adc_sample + (1 << (ADC_AVERAGING_SHIFT - 1))) >> ADC_AVERAGING_SHIFT;
		#define ADC_SAMPLE_TO_BAR_LENGTH_SCALAR 50
		uint16_t adc_sample_scaled = adc_sample / ADC_SAMPLE_TO_BAR_LENGTH_SCALAR;
		uint16_t adc_sample_average_scaled = adc_sample_average / ADC_SAMPLE_TO_BAR_LENGTH_SCALAR;
		int32_t adc_sample_deviation_from_average = abs(adc_sample_scaled - adc_sample_average_scaled);
		for(i = 0; i < MAX_ADC_SAMPLE_SCALED; i++) {
			if(i == adc_sample_deviation_from_average) {
				bars[i] = '=';
			}
			else if(i == adc_sample_average_scaled) {
				bars[i] = '|';
			}
			else if(i < adc_sample_scaled) {
				bars[i] = '-';
			}
			else {
				bars[i] = ' ';
			}
		}
		bars[MAX_ADC_SAMPLE_SCALED] = ' ';
		if(NRF_SAADC->EVENTS_CH[0].LIMITL) {
			NRF_SAADC->EVENTS_CH[0].LIMITL = 0;
			bars[MAX_ADC_SAMPLE_SCALED + 1] = 'L';
		}
		else if(NRF_SAADC->EVENTS_CH[0].LIMITH) {
			NRF_SAADC->EVENTS_CH[0].LIMITH = 0;
			bars[MAX_ADC_SAMPLE_SCALED + 1] = 'H';
		}
		else {
			bars[MAX_ADC_SAMPLE_SCALED + 1] = ' ';
		}
		bars[MAX_ADC_SAMPLE_SCALED + 2] = ' ';
		bars[MAX_ADC_SAMPLE_SCALED + 3] = '\0';
		print_debug_message(bars, adc_sample, 1, 1);
		delay_ticks(3277);
	}
}


// This function will continuously capture Bluetooth beacon packets from the air (a sniffer basically) and print the information to the debug UART
// We won't use interrupts for this, but instead poll the radio for new packets
void test_rssi(void)
{
	print_debug_message("RSSI test", 0, 0, 1);
	print_debug_message("Ramping up", 0, 0, 1);
	NRF_RADIO->EVENTS_READY = 0;
	NRF_RADIO->TASKS_RXEN = 1; // First, ramp up the receiver
	while(NRF_RADIO->EVENTS_READY == 0); // Wait for the radio to ramp up
	print_debug_message("Starting receiver", 0, 0, 1);
	NRF_RADIO->TASKS_START = 1; // start listening for incoming packets
	
	while(1) {
		NRF_RADIO->EVENTS_RSSIEND = 0;
		NRF_RADIO->TASKS_RSSISTART = 1; // start measuring the RSSI
		while(NRF_RADIO->EVENTS_RSSIEND == 0);
		int8_t rssi = NRF_RADIO->RSSISAMPLE;
		print_debug_message("RSSI: ", (uint16_t)rssi, 1, 1);
		delay_ticks(32000);
	}
/*
	while(1) {
		while(NRF_RADIO->EVENTS_END == 0); // wait for a packet to be received
		NRF_RADIO->EVENTS_END = 0; // clear the end event
		uint16_t packet_length = packet[0] & 0x3F; // get the packet length
		uint16_t packet_type = packet[0] >> 6; // get the packet type
		uint16_t packet_channel = packet[1]; // get the packet channel
		uint16_t packet_rssi = packet[2]; // get the packet RSSI
		uint16_t packet_crc_status = packet[3] & 0x01; // get the packet CRC status
		uint16_t packet_access_address_status = packet[3] >> 1; // get the packet access address status
		uint8_t packet_data[37];
		memcpy(packet_data, &packet[4], packet_length); // copy the packet data
		print_debug_message("Packet type: ", packet_type, 1, 1);
		print_debug_message("Packet channel: ", packet_channel, 1, 1);
		print_debug_message("Packet RSSI: ", packet_rssi, 1, 1);
		print_debug_message("Packet CRC status: ", packet_crc_status, 1, 1);
		print_debug_message("Packet access address status: ", packet_access_address_status, 1, 1);
		print_debug_message("Packet data: ", 0, 0, 0);
		for(i = 0; i < packet_length; i++) {
			print_debug_message(" ", (uint16_t)packet_data[i], 1, 0);
		}
		print_debug_message("", 0, 0, 1);
	}
	*/
}


/** 
* Set up radio for BLE packets. Needs to be done at the beginning of every timeslot.
*/
static inline void radio_init(void) 
{
  /* Set radio configuration parameters */
  NRF_RADIO->TXPOWER = (int8_t)(4);
//  NRF_RADIO->MODE 	 = (RADIO_MODE_MODE_Ble_1Mbit << RADIO_MODE_MODE_Pos);
  NRF_RADIO->MODE 	 = (RADIO_MODE_MODE_Ble_2Mbit << RADIO_MODE_MODE_Pos);

  NRF_RADIO->FREQUENCY 	 = 2;					// Frequency bin 80, 2480MHz, channel 39.
  NRF_RADIO->DATAWHITEIV = 37;					// NOTE: This value needs to correspond to the frequency being used
	
	
	/* Configure Access Address to be the BLE standard */
  NRF_RADIO->PREFIX0	 = 0x8e;
  NRF_RADIO->BASE0 		 = 0x89bed600; 
  NRF_RADIO->TXADDRESS = 0x00;					// Use logical address 0 (prefix0 + base0) = 0x8E89BED6 when transmitting
	NRF_RADIO->RXADDRESSES = 0x01;				// Enable reception on logical address 0 (PREFIX0 + BASE0)
  
	/* PCNF-> Packet Configuration. Now we need to configure the sizes S0, S1 and length field to match the datapacket format of the advertisement packets. */
  NRF_RADIO->PCNF0 =  (
                          (((1UL) << RADIO_PCNF0_S0LEN_Pos) & RADIO_PCNF0_S0LEN_Msk)    // length of S0 field in bytes 0-1.
                        | (((2UL) << RADIO_PCNF0_S1LEN_Pos) & RADIO_PCNF0_S1LEN_Msk)    // length of S1 field in bits 0-8.
                        | (((6UL) << RADIO_PCNF0_LFLEN_Pos) & RADIO_PCNF0_LFLEN_Msk)    // length of length field in bits 0-8.
                      );

	/* Packet configuration */
  NRF_RADIO->PCNF1 =  (
                          (((37UL)                      << RADIO_PCNF1_MAXLEN_Pos)  & RADIO_PCNF1_MAXLEN_Msk)   // maximum length of payload in bytes [0-255]
                        | (((0UL)                       << RADIO_PCNF1_STATLEN_Pos) & RADIO_PCNF1_STATLEN_Msk)	// expand the payload with N bytes in addition to LENGTH [0-255]
                        | (((3UL)                       << RADIO_PCNF1_BALEN_Pos)   & RADIO_PCNF1_BALEN_Msk)    // base address length in number of bytes.
                        | (((RADIO_PCNF1_ENDIAN_Little) << RADIO_PCNF1_ENDIAN_Pos)  & RADIO_PCNF1_ENDIAN_Msk)   // endianess of the S0, LENGTH, S1 and PAYLOAD fields.
                        | (((1UL)                       << RADIO_PCNF1_WHITEEN_Pos) & RADIO_PCNF1_WHITEEN_Msk)	// enable packet whitening
                      );

	/* CRC config */
  NRF_RADIO->CRCCNF  = (RADIO_CRCCNF_LEN_Three << RADIO_CRCCNF_LEN_Pos) | 
                       (RADIO_CRCCNF_SKIPADDR_Skip << RADIO_CRCCNF_SKIPADDR_Pos); // Skip Address when computing crc     
  NRF_RADIO->CRCINIT = 0x555555;    // Initial value of CRC
  NRF_RADIO->CRCPOLY = 0x00065B;    // CRC polynomial function
	
	/* Lock interframe spacing, so that the radio won't send too soon / start RX too early */
	NRF_RADIO->TIFS = 145;
/*
	NRF_RADIO->DACNF = (RADIO_DACNF_ENA7_Enabled << RADIO_DACNF_ENA7_Pos) |
	                   (RADIO_DACNF_ENA6_Enabled << RADIO_DACNF_ENA6_Pos) |
	                   (RADIO_DACNF_ENA5_Enabled << RADIO_DACNF_ENA5_Pos) |
	                   (RADIO_DACNF_ENA4_Enabled << RADIO_DACNF_ENA4_Pos) |
	                   (RADIO_DACNF_ENA3_Enabled << RADIO_DACNF_ENA3_Pos) |
	                   (RADIO_DACNF_ENA2_Enabled << RADIO_DACNF_ENA2_Pos) |
	                   (RADIO_DACNF_ENA1_Enabled << RADIO_DACNF_ENA1_Pos) |
	                   (RADIO_DACNF_ENA0_Enabled << RADIO_DACNF_ENA0_Pos);					   // Enable all 8 logical addresses
*/
}


// This function will continuously capture Bluetooth beacon packets from the air (a sniffer basically) and print the information to the debug UART
// We won't use interrupts for this, but instead poll the radio for new packets
void test_bluetooth_sniffer(void)
{
	uint8_t packet[300];
	char character_plus_terminator[2] = "c";
	uint16_t i;

	print_debug_message("Bluetooth sniffer", 0, 0, 1);

	while(1) {
		NRF_RADIO->POWER = 1;

//		print_debug_message("Initializing radio", 0, 0, 1);

		radio_init();
		NRF_RADIO->SHORTS = (RADIO_SHORTS_DISABLED_RSSISTOP_Enabled << RADIO_SHORTS_DISABLED_RSSISTOP_Pos) | (RADIO_SHORTS_ADDRESS_RSSISTART_Enabled << RADIO_SHORTS_ADDRESS_RSSISTART_Pos);
		memset(packet, 0, sizeof(packet));
		NRF_RADIO->PACKETPTR = (uint32_t)packet;

//		print_debug_message("Ramping up", 0, 0, 1);
		NRF_RADIO->EVENTS_READY = 0;
		NRF_RADIO->TASKS_RXEN = 1; // First, ramp up the receiver
		while(NRF_RADIO->EVENTS_READY == 0); // Wait for the radio to ramp up
//		print_debug_message("Starting receiver", 0, 0, 1);
		NRF_RADIO->TASKS_START = 1; // start listening for incoming packets

		NRF_RADIO->EVENTS_END = 0;
		while(NRF_RADIO->EVENTS_END == 0);
		NRF_RADIO->TASKS_STOP = 1;
		NRF_RADIO->TASKS_DISABLE = 1;

		uint16_t printable_character_streak = 0;
		uint16_t printable_charager_count = 0;
		for(i = 0; i < 50; i++) {
			if((packet[i] >= 32) && (packet[i] <= 126)) {
				printable_charager_count++;
				if(printable_charager_count > printable_character_streak) {
					printable_character_streak = printable_charager_count;
				}
			}
			else {
				printable_charager_count = 0;
			}
		}

		int8_t rssi = NRF_RADIO->RSSISAMPLE;
		if((printable_character_streak >= 1) && (rssi < 100) /* && (packet[3] == 'H') */) {
			print_debug_message("Packet: ", 0, 0, 1);
			print_debug_message("RSSI: ", (uint16_t)rssi, 1, 1);
			for(i = 0; i < packet[1] + 3; i++) {
				if((packet[i] >= 32) && (packet[i] <= 126)) {
					character_plus_terminator[0] = packet[i];
					print_debug_message(character_plus_terminator, 0, 0, 0);
				} else {
					print_debug_message(" 0x", packet[i], 1, 0);
					print_debug_message(" ", 0, 0, 0);
				}
			}
			print_debug_message("", 0, 0, 1);
			delay_ticks(3200);
		}
	}
}


// This function will continuously transmits Bluetooth beacon packets
void test_bluetooth_transmitter(void)
{
	uint8_t packet[300];

	print_debug_message("Bluetooth broadcaster", 0, 0, 1);

	while(1) {
		NRF_RADIO->POWER = 1;

//		print_debug_message("Initializing radio", 0, 0, 1);

		radio_init();
		NRF_RADIO->SHORTS = (RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos) | (RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos);
		memset(packet, 0, sizeof(packet));
		memcpy(packet + 3, "Hello world!", 12);
		packet[1] = 12; // the second byte in the packet is the length of the payload
		NRF_RADIO->PACKETPTR = (uint32_t)packet;

//		print_debug_message("Ramping up", 0, 0, 1);
		NRF_RADIO->EVENTS_READY = 0;
		NRF_RADIO->TASKS_TXEN = 1; // First, ramp up the transmitter

//		while(NRF_RADIO->EVENTS_READY == 0); // Wait for the radio to ramp up
//		print_debug_message("Starting receiver", 0, 0, 1);
//		NRF_RADIO->TASKS_START = 1; // start transmitting the packet

//		NRF_RADIO->EVENTS_END = 0;
//		while(NRF_RADIO->EVENTS_END == 0);
//		NRF_RADIO->TASKS_STOP = 1;
//		NRF_RADIO->TASKS_DISABLE = 1;
		print_debug_message("Packet sent", 0, 0, 1);

		delay_ticks(3200);
	}
}

#define COMMUNICATION_CYCLE_TICKS 3276  // 3276 is the number of ticks in 100 ms
//#define COMMUNICATION_CYCLE_TICKS 6400  // 6400 is the number of ticks in 100 ms
#define MISSED_PACKET_COUNT_UNSYNC_THRESHOLD 20
#define SLAVE_OUT_OF_SYNC_TICKS_SHIFT 33
#define IN_SYNC_RX_WINDOW_TIME_TICKS 33
#define OUT_OF_SYNC_RX_WINDOW_TIME_TICKS 66
#define SLAVE_RX_WINDOW_TIME_TICKS 10
#define PACKET_MAGIC_NUMBER1 12
#define PACKET_MAGIC_NUMBER2 34
#define PACKET_MAGIC_NUMBER3 56
#define PACKET_MAGIC_NUMBER4 78

uint32_t wait_for_time(int32_t offset_ticks, int32_t time_adjust_offset)
{
	static uint32_t next_rx_window_time = 0;
	static uint8_t first_time = 1;

	if(first_time) {
		first_time = 0;
		next_rx_window_time = NRF_RTC0->COUNTER;
	}
//	next_rx_window_time = (next_rx_window_time + COMMUNICATION_CYCLE_TICKS);
	next_rx_window_time = (next_rx_window_time + COMMUNICATION_CYCLE_TICKS + time_adjust_offset);
	uint32_t wait_for_counter_value = (next_rx_window_time + offset_ticks) & 0xFFFFFF; // 0xFFFFFF is the maximum value of the 24-bit counter
//	print_debug_message("Waiting for:", NRF_RTC0->COUNTER & 0xFFFF, 1, 1);
	while(NRF_RTC0->COUNTER != wait_for_counter_value);
//	print_debug_message("Done", 0, 0, 1);
	return wait_for_counter_value;
}


// Interrupt service routing for the RTC0 peripheral
void RTC0_Handler1(void)
{
	red_led_on();
	while(1);
	NVIC_DisableIRQ(RTC0_IRQn);
	if(NRF_RTC0->EVENTS_COMPARE[0] != 0) {
		NRF_RTC0->EVENTS_COMPARE[0] = 0;
//		NRF_RTC0->TASKS_CLEAR = 1;
//		NRF_RTC0->TASKS_START = 1;
	}
//	red_led_toggle();
	NRF_RTC0->EVENTS_COMPARE[0] = 0;
	NRF_RTC0->EVENTS_COMPARE[0] = 0;
	NRF_RTC0->EVENTS_COMPARE[0] = 0;
	NRF_RTC0->EVENTS_COMPARE[0] = 0;
	NRF_RTC0->EVENTS_COMPARE[0] = 0;
	NRF_RTC0->EVENTS_COMPARE[0] = 0;
}

void RTC0_Handler2(void)
{
	red_led_on();
	while(1);
}
void RTC0_Handler3(void)
{
	red_led_on();
	while(1);
}
void RTC0_Handler4(void)
{
	red_led_on();
	while(1);
}
void RTC0_Handler5(void)
{
	red_led_on();
	while(1);
}
void RTC0_Handler6(void)
{
	red_led_on();
	while(1);
}
void RTC0_Handler7(void)
{
	red_led_on();
	while(1);
}
void RTC0_Handler8(void)
{
	red_led_on();
	while(1);
}
void RTC0_Handler9(void)
{
	red_led_on();
	while(1);
}
void RTC0_Handler10(void)
{
	red_led_on();
	while(1);
}
void RTC0_Handler11(void)
{
	red_led_on();
	while(1);
}
void RTC0_Handler13(void)
{
	red_led_on();
	while(1);
}
void RTC0_Handler14(void)
{
	red_led_on();
	while(1);
}
void RTC0_Handler15(void)
{
	red_led_on();
	while(1);
}
void RTC0_Handler16(void)
{
	red_led_on();
	while(1);
}
void RTC0_Handler17(void)
{
	red_led_on();
	while(1);
}
void RTC0_Handler18(void)
{
	red_led_on();
	while(1);
}
void RTC0_Handler19(void)
{
	red_led_on();
	while(1);
}
void RTC0_Handler20(void)
{
	red_led_on();
	while(1);
}
void RTC0_Handler21(void)
{
	red_led_on();
	while(1);
}

void test_sleep_current(void)
{
	// Enable global interrupts
	__enable_irq();
	// Enable the ability to exit from __WFE() when the RTC0 interrupt occurs
	NRF_RTC0->INTENSET = RTC_INTENSET_COMPARE0_Msk;
	NRF_RTC0->EVTEN = RTC_EVTEN_COMPARE0_Msk;
//	NVIC_EnableIRQ(RTC0_IRQn);
	while(1) {
		uint32_t counter = NRF_RTC0->COUNTER;
		// Set the compare register to be half a second later
		NRF_RTC0->CC[0] = (counter + (32768 >> 1)) & 0xFFFFFF;
		NRF_POWER->TASKS_LOWPWR = 1;
		NRF_RTC0->EVENTS_COMPARE[0] = 0;
//		while(NRF_RTC0->EVENTS_COMPARE[0] == 0) {
//			print_debug_message("Sleeping: ", counter & 0xFFFF, 1, 1);
//			print_debug_message("CC                       : ", NRF_RTC0->CC[0] & 0xFFFF, 1, 1);
//		}
//		red_led_on();
		__WFE();
//		red_led_on();
		red_led_toggle();
	}
}

int32_t timing_adjust = 0;

uint8_t master_rx(uint32_t rx_window_time_ticks)
{
	uint8_t packet[30];
	uint32_t rx_window_timout_time = (NRF_RTC0->COUNTER + rx_window_time_ticks) & 0x00FFFFFF; // COUNTER is a 24-bit counter

	NRF_RADIO->POWER = 1;
	radio_init();
	NRF_RADIO->SHORTS = (RADIO_SHORTS_DISABLED_RSSISTOP_Enabled << RADIO_SHORTS_DISABLED_RSSISTOP_Pos) | (RADIO_SHORTS_ADDRESS_RSSISTART_Enabled << RADIO_SHORTS_ADDRESS_RSSISTART_Pos);
	memset(packet, 0, sizeof(packet));
	NRF_RADIO->PACKETPTR = (uint32_t)packet;
	NRF_RADIO->EVENTS_READY = 0;
	NRF_RADIO->TASKS_RXEN = 1; // First, ramp up the receiver
	while(NRF_RADIO->EVENTS_READY == 0); // Wait for the radio to ramp up
	NRF_RADIO->TASKS_START = 1; // start listening for incoming packets
	NRF_RADIO->EVENTS_END = 0;
	while(1) {
		if(NRF_RADIO->EVENTS_END != 0) {
			break;
		}
		if(NRF_RTC0->COUNTER == rx_window_timout_time) {
			break;
		}
	}
	NRF_RADIO->TASKS_STOP = 1;
	NRF_RADIO->TASKS_DISABLE = 1;

	//int8_t rssi = NRF_RADIO->RSSISAMPLE;
	if((packet[1] == 8) && (packet[3] == PACKET_MAGIC_NUMBER1) && (packet[4] == PACKET_MAGIC_NUMBER2) && (packet[5] == PACKET_MAGIC_NUMBER3) && (packet[6] == PACKET_MAGIC_NUMBER4)) {
		#ifdef SLAVE
			int32_t timing_error;
			memcpy(&timing_error, packet + 7, sizeof(timing_error));
			timing_adjust = -timing_error;
			if(timing_error < 0) {
				timing_error = -timing_error;
				print_debug_message("Timing error: -", timing_error & 0xFF, 1, 1);
			}
			else {
				print_debug_message("Timing error: ", timing_error & 0xFF, 1, 1);
			}
		#endif
		return 1;
	}
	else {
		return 0;
	}
}


void master_tx(int32_t timing_error)
{
	uint8_t packet[30];

	NRF_RADIO->POWER = 1;
	radio_init();
	NRF_RADIO->SHORTS = (RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos) | (RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos);
//	memset(packet, 0, sizeof(packet));
	packet[3] = PACKET_MAGIC_NUMBER1;
	packet[4] = PACKET_MAGIC_NUMBER2;
	packet[5] = PACKET_MAGIC_NUMBER3;
	packet[6] = PACKET_MAGIC_NUMBER4;
	packet[7] = timing_error & 0xFF;
	packet[8] = (timing_error >> 8) & 0xFF;
	packet[9] = (timing_error >> 16) & 0xFF;
	packet[10] = (timing_error >> 24) & 0xFF;
	packet[1] = 8; // the second byte in the packet is the length of the payload
	NRF_RADIO->PACKETPTR = (uint32_t)packet;
	NRF_RADIO->EVENTS_READY = 0;
	NRF_RADIO->EVENTS_DISABLED = 0;
	NRF_RADIO->TASKS_TXEN = 1; // First, ramp up the transmitter. The packet will transmit automatically after ramp up because of the shortcut set in the SHORTS register
	while(NRF_RADIO->EVENTS_DISABLED == 0); // Wait for the radio to ramp up
}


void slave_tx(void)
{
	master_tx(0); // for now, we will use the same routine as the master_tx, but this may change in the future
} 


uint8_t slave_rx(uint32_t rx_window_time_ticks)
{
	return master_rx(rx_window_time_ticks); // for now, we will use the same routine as the master_rx, but this may change in the future
}


// The master will receive first, and if it received a packet from the slave, it will transmit a packet to the slave
void master_rx_tx(void)
{
	static uint8_t missed_rx_tx_count = 0;
	static uint32_t rx_window_time_ticks = OUT_OF_SYNC_RX_WINDOW_TIME_TICKS;
	static uint8_t in_sync = 0;

	print_debug_message("Master start", 0, 0, 1);

	while(1) {
		int32_t offset_ticks = -((int32_t)(rx_window_time_ticks / 2));
		uint32_t current_time = wait_for_time(offset_ticks, 0);
		//uint32_t current_time = wait_for_time(-50, 0);
		uint8_t packet_received_flag = master_rx(rx_window_time_ticks);
		if(packet_received_flag == 1) {
			red_led_on();
			int32_t timing_error = (NRF_RTC0->COUNTER - current_time - (rx_window_time_ticks >> 1)) & 0x00FFFFFF;
			if(timing_error & 0x800000) {
				timing_error |= 0xFF000000; // extend the sign bit from 24 bit to 32 bit
			}

			uint32_t c1 = NRF_RTC0->COUNTER; // short pause before transmitting
			while(NRF_RTC0->COUNTER == c1);
			c1 = NRF_RTC0->COUNTER;
			while(NRF_RTC0->COUNTER == c1);

			master_tx(timing_error);

			if(timing_error < 0) {
				timing_error = -timing_error;
				print_debug_message("Timing error: -", timing_error & 0xFFFF, 1, 1);
			}
			else {
				print_debug_message("Timing error: ", timing_error & 0xFFFF, 1, 1);
			}

			if(!in_sync) {
				in_sync = 1;
				print_debug_message("Sent and received a packet. In sync now.", 0, 0, 1);
			}
			missed_rx_tx_count = 0;
			rx_window_time_ticks = IN_SYNC_RX_WINDOW_TIME_TICKS;
			red_led_off();
		}
		else {
			if(missed_rx_tx_count < MISSED_PACKET_COUNT_UNSYNC_THRESHOLD) {
				missed_rx_tx_count++;
			}
			else {
				if(in_sync) {
					in_sync = 0;
					print_debug_message("Missed a bunch of packets in a row. Will make rx window bigger.", 0, 0, 1);
				}
				rx_window_time_ticks = OUT_OF_SYNC_RX_WINDOW_TIME_TICKS;
			}
		}
	}
}


// The slave will transmit first, then receive a response right after
void slave_tx_rx(void)
{
	static uint8_t missed_tx_rx_count = 0;
	static uint8_t in_sync = 0;

	print_debug_message("Slave start", 0, 0, 1);

	while(1) {
		wait_for_time(0, (missed_tx_rx_count < MISSED_PACKET_COUNT_UNSYNC_THRESHOLD) ? timing_adjust : SLAVE_OUT_OF_SYNC_TICKS_SHIFT);
		timing_adjust = 0;
		red_led_on();
		slave_tx();
		red_led_off();
//		green_led_on();
		uint8_t packet_received_flag = slave_rx(SLAVE_RX_WINDOW_TIME_TICKS);
//		green_led_off();
		if(packet_received_flag == 1) {
			red_led_on();
			if(!in_sync) {
				in_sync = 1;
				print_debug_message("Sent and received a packet. In sync now.", 0, 0, 1);
			}
			missed_tx_rx_count = 0;
		}
		else {
			red_led_off();
			if(missed_tx_rx_count < MISSED_PACKET_COUNT_UNSYNC_THRESHOLD) {
				missed_tx_rx_count++;
			}
			else {
				if(in_sync) {
					in_sync = 0;
					print_debug_message("Missed a bunch of packets in a row. Will make rx window bigger.", 0, 0, 1);
				}
			}
		}
	}
}

// Tests the lowest possible current consumption of the system. Expected only 0.3uA consumption from the MCU
void test_system_off(void)
{
    /* Put green LED on to see when the program starts */
    green_led_on();

    /* Enable BLE configuration so that softdevice gets activated */
    lib_ble_init();
    lib_ble_advertising_start();

    /* Put the chip into system off mode. NOTE: System runs together with the Softdevice */
    ret_code_t err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);

    /* Should never reach here */
    red_led_on();
    while (1);
}

/**
 * @brief Run specific hardware tests
 */
void hw_tests(void)
{
	/**
	 * Below are the various tests you can run to test the hardware.
	 * If you want to run a test, uncomment it and leave the others commented out.
	 */
//	test_uart();
//	test_rtc();
//	test_buttons_and_leds();
//	test_ir_LED_toggle_continuously();
//	test_pwm();
//	test_pwm_pulsing();
//	test_ir_receive();
//	test_bluetooth();
//	test_ble();
//	test_print_debug_message();
//	test_PIR_motion_sensor();
//	test_rssi();
//	test_bluetooth_transmitter();
//	test_bluetooth_sniffer();
//	test_sleep_current();
//	test_system_off();

//	print_debug_message("LFCLKSTAT L:", NRF_CLOCK->LFCLKSTAT & 0xFFFF, 1, 1);
//	print_debug_message("LFCLKSTAT H:", (NRF_CLOCK->LFCLKSTAT >> 16) & 0xFFFF, 1, 1);
/*
#ifdef MASTER
	master_rx_tx();
#elif defined SLAVE
	slave_tx_rx();
#else
	#error "Must define either MASTER or SLAVE"
#endif
*/
	init_adc();
	init_pwm();

	enable_reset_pin();

	while(1)
	{
		if(NRF_P0->IN & (1 << PIN_SWITCH1))
		{
			IR_receiver_power_on();
			delay_ticks(66); // a delay of about 2ms to allow for the IR receiver to power up and make a valid signal
			red_led_on();
			record_ir_pattern();
			red_led_off();
			IR_receiver_power_off();
		}

		if(NRF_P0->IN & (1 << PIN_SWITCH2))
		{
			green_led_on();
			playback_ir_pattern();
			green_led_off();
		}
	}
}
