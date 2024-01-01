
#include "lib_settings.h"

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "app_timer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


/**
 * @brief Settings Library Initialization Function.
 */
ret_code_t lib_settings_init(void);

/**
 * @brief Settings Library Function for getting the most recent configuration.
 */
ret_code_t lib_settings_get(void);

/**
 * @brief Settings Library Function for Saving the most recent configuration.
 */
ret_code_t lib_settings_write(void);

/**
 * @}
 */
