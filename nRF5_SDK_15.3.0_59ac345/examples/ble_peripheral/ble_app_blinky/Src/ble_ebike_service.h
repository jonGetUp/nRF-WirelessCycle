#ifndef BLE_EBIKE_SERVICE_H__
#define BLE_EBIKE_SERVICE_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


//#ifdef __cplusplus
//extern "C" {
//#endif

/**@brief   Macro for defining a ble_lbs instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_LBS_DEF(_name)                                                                          \
static ble_lbs_t _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_LBS_BLE_OBSERVER_PRIO,                                                     \
                     ble_lbs_on_ble_evt, &_name)

#define LBS_UUID_BASE        {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, \
                              0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00} // 128-bit base UUID, 0x00 designed by the service and char UUID
#define LBS_UUID_SERVICE     0x0001//0x1523
#define LBS_UUID_BUTTON_CHAR 0x0002//0x1524
#define LBS_UUID_LED_CHAR    0x0003//0x1525
#define BLE_UUID_SERIAL_NUMBER_CHAR 0x0004

// BLE_WRITE:
/** @brief Our Service init structure. This structure contains all options and data needed for
 *        initialization of the service.*/
 //This is used to pass the write handlers for different characteristics from main.c
 //This is essentially like public constructor. All of the content will be copied to instance.
 //Note that "uint32_t characteristic1_value" part had to match from Step 1
typedef void (*ble_os_characteristic1_value_write_handler_t) (uint32_t characteristic1_value);

//// Add other handlers here...
//typedef struct
//{
//	/**< Event handler to be called when the Characteristic1 is written */
//    ble_os_characteristic1_value_write_handler_t characteristic1_value_write_handler; 
//    // Add other handlers here...
//
//} ble_os_init_t;


// Forward declaration of the ble_lbs_t type.
typedef struct ble_lbs_s ble_lbs_t;

typedef void (*ble_lbs_led_write_handler_t) (uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t new_state); //to change uint32_t

/** @brief LED Button Service init structure. This structure contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_lbs_led_write_handler_t led_write_handler; /**< Event handler to be called when the LED Characteristic is written. */
    /**< Event handler to be called when the Characteristic1 is written */
    ble_os_characteristic1_value_write_handler_t characteristic1_value_write_handler; 
    // Add other handlers here...

} ble_lbs_init_t;

/**@brief LED Button Service structure. This structure contains various status information for the service. */
struct ble_lbs_s
{
    uint16_t                    service_handle;      /**< Handle of LED Button Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    led_char_handles;    /**< Handles related to the LED Characteristic. */
    ble_gatts_char_handles_t    button_char_handles; /**< Handles related to the Button Characteristic. */
    ble_gatts_char_handles_t    char_handles_1;      // Adding handles for the characteristic to our structure

    uint8_t                     uuid_type;           /**< UUID type for the LED Button Service. */
    ble_lbs_led_write_handler_t led_write_handler;   /**< Event handler to be called when the LED Characteristic is written. */
                                                     /**< can hold 16-bit handles for the characteristic value, user descriptor, CCCD and SCCD */
    // BLE_WRITE: Write handlers. Upon BLE write, these handler will be called
    // Their implementation is in the main.c
    ble_os_characteristic1_value_write_handler_t characteristic1_value_write_handler;  /**< Event handler to be called when the Characteristic1 is written. */
    // Add other handlers here...

};                                                  




/**@brief Function for initializing the LED Button Service.
 *
 * @param[out] p_lbs      LED Button Service structure. This structure must be supplied by
 *                        the application. It is initialized by this function and will later
 *                        be used to identify this particular service instance.
 * @param[in] p_lbs_init  Information needed to initialize the service.
 *
 * @retval NRF_SUCCESS If the service was initialized successfully. Otherwise, an error code is returned.
 */
uint32_t ble_lbs_init(ble_lbs_t * p_lbs, const ble_lbs_init_t * p_lbs_init);


/**@brief Function for handling the application's BLE stack events related to our service and characteristic.
 *
 * @details This function handles all events from the BLE stack that are of interest to the LED Button Service.
 *
 * @param[in] p_ble_evt  Event received from the BLE stack.
 * @param[in] p_context  LED Button Service structure.
 */
void ble_lbs_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);


/**@brief Function for sending a button state notification.
 *
 ' @param[in] conn_handle   Handle of the peripheral connection to which the button state notification will be sent.
 * @param[in] p_lbs         LED Button Service structure.
 * @param[in] button_state  New button state.
 *
 * @retval NRF_SUCCESS If the notification was sent successfully. Otherwise, an error code is returned.
 */
//uint32_t ble_lbs_on_button_change(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t button_state);

/**@brief Function for sending a battery voltage notification.
 *
 ' @param[in] conn_handle   Handle of the peripheral connection to which the battery voltage notification will be sent.
 * @param[in] p_lbs         LED Button Service structure.
 * @param[in] button_state  New battery voltage
 *
 * @retval NRF_SUCCESS If the notification was sent successfully. Otherwise, an error code is returned.
 */
uint32_t ble_lbs_batVolt_characteristic_update(uint16_t conn_handle, ble_lbs_t *p_lbs, uint16_t *battery_value);

uint32_t ble_lbs_characteristic_1_update(uint16_t conn_handle, ble_lbs_t *p_lbs, uint32_t * characteristic_1);

//#ifdef __cplusplus
//}
//#endif

#endif // BLE_EBIKE_SERVICE_H__

/** @} */
