#ifndef HW_TESTS_H
#define HW_TESTS_H

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "lib_ble.h"
#include "bsp.h"

#include "app_timer.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_delay.h"
#include "nrf_gpio.h"

/**
 * @brief Run specific hardware test
 */
void hw_tests(void);
void test_rtc(void);
void test_uart(void);
void test_buttons_and_leds(void);
void test_ir_LED_toggle_continuously(void);
void init_debug_uart(void);
void test_ir_receive(void);
void record_ir_pattern(void);

#endif /* HW_TESTS_H */
