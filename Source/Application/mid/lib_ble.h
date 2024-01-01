#ifndef LIB_BLE_H
#define LIB_BLE_H

/**
 * @brief BLE Library Initialization Function.
 */
void lib_ble_init(void);

/**
 * @brief BLE Library Function for starting advertising.
 */
void lib_ble_advertising_start(void);

/**
 * @brief BLE Library Function for getting the handler.
 */
uint16_t lib_ble_get_handler(void);

/**
 * @brief BLE stack event processing.
 */
void lib_ble_process(void);

#endif /* LIB_BLE_H */
