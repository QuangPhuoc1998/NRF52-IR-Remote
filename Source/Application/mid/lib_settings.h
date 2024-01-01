#ifndef LIB_SETTINGS_H
#define LIB_SETTINGS_H

#define DIS_HW_REV "1.0.0"
#define DIS_FW_REV "1.0.1"

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

#endif /* LIB_SETTINGS_H */
