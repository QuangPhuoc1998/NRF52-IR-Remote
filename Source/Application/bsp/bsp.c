#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "hw_tests.h"
#include "bsp.h"

/**
 * @brief System clock initialization
 */
void init_all_clocks(void)
{
	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
	NRF_CLOCK->TASKS_HFCLKSTART = 1;
	while(NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);

//	NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Synth << CLOCK_LFCLKSRC_SRC_Pos); // choose the synthesized 32.768 kHz clock (rather than the RC oscillator)
	NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos); // choose the 32.768 kHz crystal as the source (rather than the RC oscillator)
	NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
	NRF_CLOCK->TASKS_LFCLKSTART = 1; // start the 32.768kHz clock
	while(NRF_CLOCK->EVENTS_LFCLKSTARTED == 0); // wait for the 32.768kHz clock to start. datasheet says it may be 0.25 seconds

	// Enable event routing for the compare event 0
	NRF_RTC0->EVTEN = RTC_EVTEN_COMPARE0_Msk;
	NRF_RTC0->TASKS_START = 1; // start the Real time counter
}

/**
 * @brief Initialize device peripherals
 */
void bsp_init(void)
{
	//Initialize system clocks
    //init_all_clocks();

	//Enable the DC-DC converter to save power
	NRF_POWER->DCDCEN = 1;

	// Configure the GPIOs that we will use for various functions, like controlling the LEDs
//	NRF_P0->OUTSET = (1 << PIN_RED_LED);     // set high to turn off the LED
//	NRF_P0->PIN_CNF[PIN_RED_LED] = 0x3;      // make this GPIO an output
//
//	NRF_P0->OUTSET = (1 << PIN_GREEN_LED);   // set high to turn off the LED
//	NRF_P0->PIN_CNF[PIN_GREEN_LED] = 0x3;    // make this GPIO an output
//
//	NRF_P0->OUTSET = (1 << PIN_BLUE_LED);     // set high to turn off the LED
//	NRF_P0->PIN_CNF[PIN_BLUE_LED] = 0x3;      // make this GPIO an output

//	NRF_P0->OUTSET = (1 << PIN_IR_RECEIVER_POWER_CONTROL);  // set high to turn off the IR receiver
//	NRF_P0->PIN_CNF[PIN_IR_RECEIVER_POWER_CONTROL] = 0x3;   // make this GPIO an output

//	NRF_P0->OUTCLR = (1 << PIN_IR_TRANSMIT); // make sure that the IR LED is off initially
//	NRF_P0->PIN_CNF[PIN_IR_TRANSMIT] = 0x3;  // make this GPIO an output

	//NRF_P0->PIN_CNF[PIN_IR_RECEIVE] = 0x0;   // make this GPIO an input, for receiving the IR communication

	NRF_P0->PIN_CNF[PIN_SWITCH1] = 0x0;      // make this GPIO an input (for the switch 1)
	NRF_P0->PIN_CNF[PIN_SWITCH2] = 0x0;      // make this GPIO an input (for the switch 2)
	NRF_P0->PIN_CNF[PIN_PIR]	 = 0x0;      // make this GPIO an input (for the switch 2)

	init_debug_uart();
}

/**
 * @brief Turn on red LED
 */
void red_led_on(void)
{
	NRF_GPIO->OUTCLR = (1 << PIN_RED_LED);
}

/**
 * @brief Turn off red LED
 */
void red_led_off(void)
{
	NRF_GPIO->OUTSET = (1 << PIN_RED_LED);
}

/**
 * @brief Toggle red LED
 */
void red_led_toggle(void)
{
	NRF_GPIO->OUT ^= (1 << PIN_RED_LED);
}

/**
 * @brief Turn on green LED
 */
void green_led_on(void)
{
	NRF_GPIO->OUTCLR = (1 << PIN_GREEN_LED);
}

/**
 * @brief Turn off green LED
 */
void green_led_off(void)
{
	NRF_GPIO->OUTSET = (1 << PIN_GREEN_LED);
}

/**
 * @brief Toggle Green LED
 */
void green_led_toggle(void)
{
	NRF_GPIO->OUT ^= (1 << PIN_GREEN_LED);
}

/**
 * @brief Turn on blue LED
 */
void blue_led_on(void)
{
	NRF_GPIO->OUTCLR = (1 << PIN_BLUE_LED);
}

/**
 * @brief Turn off blue LED
 */
void blue_led_off(void)
{
	NRF_GPIO->OUTSET = (1 << PIN_BLUE_LED);
}

/**
 * @brief Toggle blue LED
 */
void blue_led_toggle(void)
{
	NRF_GPIO->OUT ^= (1 << PIN_BLUE_LED);
}

/**
 * @brief Toggle IR LED
 */
void IR_led_toggle(void)
{
	NRF_GPIO->OUT ^= (1 << PIN_IR_TRANSMIT);
}

/**
 * @brief Turn on the IR receiver
 */
void IR_receiver_power_on(void)
{
	NRF_P0->OUTCLR = (1 << PIN_IR_RECEIVER_POWER_CONTROL);
}

/**
 * @brief Turn off IR receiver
 */
void IR_receiver_power_off(void)
{
	NRF_P0->OUTSET = (1 << PIN_IR_RECEIVER_POWER_CONTROL);
}

/**
 * @brief Delay function, where one tick is 1/32768 seconds
 * One millisecond will therefore be about 33 ticks
 */
void delay_ticks(uint32_t ticks)
{
	uint32_t delay_finish_time = (NRF_RTC0->COUNTER + ticks) & 0x00FFFFFF; // 24 bit counter
	while (NRF_RTC0->COUNTER != delay_finish_time) {
//		__WFE();
	}
}

uint8_t IR_Receiver_Read(void)
{
	return (uint8_t)(NRF_P0->IN & (1 << PIN_IR_RECEIVE));
}

