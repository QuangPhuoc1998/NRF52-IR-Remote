
#ifndef BLE_CUS_H__
#define BLE_CUS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BLE_CUS_BLE_OBSERVER_PRIO
#define BLE_CUS_BLE_OBSERVER_PRIO 2
#endif

/**@brief   Macro for defining a ble_cus instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_CUS_DEF(_name)                                                                          \
static ble_cus_t _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_CUS_BLE_OBSERVER_PRIO,                                                     \
                     ble_cus_on_ble_evt, &_name)

#define CUS_UUID_BASE        {0xd6, 0x1d, 0x2a, 0xfd, 0x83, 0xa4, 0x3a, 0x9d, \
			      0x91, 0x4a, 0x41, 0x69, 0x88, 0x88, 0x39, 0x09}
#define CUS_UUID_SERVICE     0x1523
#define CUS_UUID_RTC_SET     0x1524
#define CUS_UUID_MOT_SENS    0x1525
#define CUS_UUID_MOT_TOUT    0x1526
#define CUS_UUID_SCHEDULE    0x1527
#define CUS_UUID_BATT_VOL    0x1529
#define CUS_UUID_IR_LEARN    0x1532
#define CUS_UUID_IR_EMMIT    0x1533

enum
{
	CUS_UUID_RTC_SET_INDEX,
	CUS_UUID_MOT_SENS_INDEX,
	CUS_UUID_MOT_TOUT_INDEX,
	CUS_UUID_SCHEDULE_INDEX,
	CUS_UUID_BATT_VOL_INDEX,
	CUS_UUID_IR_LEARN_INDEX,
	CUS_UUID_IR_EMMIT_INDEX,
};



// Forward declaration of the ble_cus_t type.
typedef struct ble_cus_s ble_cus_t;

typedef void (*ble_cus_led_write_handler_t) (uint16_t conn_handle, ble_cus_t * p_cus, uint8_t new_state);

/** @brief Custom Service init structure. This structure contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_cus_led_write_handler_t led_write_handler; /**< Event handler to be called when the LED Characteristic is written. */
} ble_cus_init_t;

/**@brief Custom Service structure. This structure contains various status information for the service. */
struct ble_cus_s
{
    uint16_t                    service_handle;      /**< Handle of Custom Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    led_char_handles;    /**< Handles related to the LED Characteristic. */
    ble_gatts_char_handles_t    button_char_handles; /**< Handles related to the Button Characteristic. */
    uint8_t                     uuid_type;           /**< UUID type for the Custom Service. */
    ble_cus_led_write_handler_t led_write_handler;   /**< Event handler to be called when the LED Characteristic is written. */
};


/**@brief Function for initializing the Custom Service.
 *
 * @param[out] p_cus      Custom Service structure. This structure must be supplied by
 *                        the application. It is initialized by this function and will later
 *                        be used to identify this particular service instance.
 * @param[in] p_cus_init  Information needed to initialize the service.
 *
 * @retval NRF_SUCCESS If the service was initialized successfully. Otherwise, an error code is returned.
 */
uint32_t ble_cus_init(ble_cus_t * p_cus, const ble_cus_init_t * p_cus_init);


/**@brief Function for handling the application's BLE stack events.
 *
 * @details This function handles all events from the BLE stack that are of interest to the Custom Service.
 *
 * @param[in] p_ble_evt  Event received from the BLE stack.
 * @param[in] p_context  Custom Service structure.
 */
void ble_cus_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);


/**@brief Function for sending a button state notification.
 *
 ' @param[in] conn_handle   Handle of the peripheral connection to which the button state notification will be sent.
 * @param[in] p_cus         Custom Service structure.
 * @param[in] button_state  New button state.
 *
 * @retval NRF_SUCCESS If the notification was sent successfully. Otherwise, an error code is returned.
 */
uint32_t ble_cus_on_button_change(uint16_t conn_handle, ble_cus_t * p_cus, uint8_t button_state);


#ifdef __cplusplus
}
#endif

#endif // BLE_CUS_H__

/** @} */
