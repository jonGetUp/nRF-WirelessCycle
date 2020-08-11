/********************************************************************************/
/**
 * @file ble_ebike_service.h
 *
 * @brief Bluetooth profil service and characteristics structure initialization
 *
 * @author Gaspoz Jonathan
 *
 */
/********************************************************************************/
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

/**@brief   Macro for defining a ble_ebike instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_LBS_DEF(_name)                                                       \
static ble_ebike_s_t _name;                                                      \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                              \
                     BLE_LBS_BLE_OBSERVER_PRIO,                                  \
                     ble_ebike_s_on_ble_evt, &_name)

#define EBIKE_S_UUID_BASE     {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15,   \
                               0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00} // 128-bit base UUID, 0x00 designed by the service and char UUID
#define EBIKE_S_UUID_SERVICE              0x0001
#define EBIKE_S_UUID_BATVOLT_CHAR         0x0002
#define EBIKE_S_UUID_BATTERY_CURRENT_CHAR 0x0010
#define EBIKE_S_UUID_CHARGER_CURRENT_CHAR 0x0011
#define EBIKE_S_UUID_CURFAULT_CHAR        0x0014
#define EBIKE_S_UUID_BALANCEINWORK_CHAR   0x0015
#define EBIKE_S_UUID_SMMAIN_CHAR          0x0016

#define EBIKE_S_UUID_UNBLOCK_SM_CHAR      0x0017
#define EBIKE_S_UUID_SERIAL_NUMBER_CHAR   0x0018
#define EBIKE_S_UUID_CHARGER_CURRENT_HIGH_CHAR 0x0012
#define EBIKE_S_UUID_CHARGER_CURRENT_LOW_CHAR  0x0013
//>>>>>>>>>> Add other UUIDs here....

// Forward declaration of the ble_ebike_s_t type.
typedef struct ble_ebike_s ble_ebike_s_t;

/******************************************************************************/
/* BLE_WRITE                                                                  */
/******************************************************************************/

/** @brief Our Service init structure. This structure contains all options and data needed for
 *        initialization of the service.
 *        This is used to pass the write handlers for different characteristics from main.c
 *        This is essentially like public constructor. All of the content will be copied to instance.
 *        Note that "uint32_t serial_number_value" part had to match from Step 1 */
typedef void (*ble_ebike_s_unblock_sm_write_handler_t) (uint16_t conn_handle, ble_ebike_s_t * p_ebike_s, uint8_t new_state); //to change uint32_t
typedef void (*ble_ebike_s_serial_number_write_handler_t) (uint32_t serial_number_value);
typedef void (*ble_ebike_s_charger_current_high_write_handler_t) (uint16_t charger_current_high_value);
typedef void (*ble_ebike_s_charger_current_low_write_handler_t) (uint16_t charger_current_low_value);
//>>>>>>>>>> Add other handlers here...



/** @brief Ebike Service init structure. This structure contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_ebike_s_unblock_sm_write_handler_t unblock_sm_write_handler; /**< Event handler to be called when the LED Characteristic is written. */
    /**< Event handler to be called when the Characteristic1 is written */
    ble_ebike_s_serial_number_write_handler_t serial_number_write_handler;
    ble_ebike_s_charger_current_high_write_handler_t charger_current_high_write_handler; 
    ble_ebike_s_charger_current_low_write_handler_t charger_current_low_write_handler; 
    //>>>>>>>>>> Add other handlers here....
} ble_ebike_s_init_t;

/**@brief LED Button Service structure. This structure contains various status information for the service. */
struct ble_ebike_s
{
    uint16_t                    service_handle;      /**< Handle of EBIKE Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    batvolt_char_handles; /**< Handles related to the BatVolt Characteristic. */
    ble_gatts_char_handles_t    battery_current_char_handles;
    ble_gatts_char_handles_t    charger_current_char_handles;
    ble_gatts_char_handles_t    curFault_char_handles;
    ble_gatts_char_handles_t    balanceInWork_char_handles;
    ble_gatts_char_handles_t    smMain_char_handles;

    ble_gatts_char_handles_t    unblock_sm_char_handles;      /**< Handles related to the unblock_sm Characteristic. */
    ble_gatts_char_handles_t    serial_number_char_handles;   /**< Adding handles for the characteristic to our structure */
    ble_gatts_char_handles_t    charger_current_high_char_handles;
    ble_gatts_char_handles_t    charger_current_low_char_handles;
    //>>>>>>>>>> Add other handlers here....

    // BLE_WRITE: Write handlers. Upon BLE write, these handler will be called
    // Their implementation is in the ble_config.c
    // can hold 16-bit handles for the characteristic value, user descriptor, CCCD and SCCD */
    uint8_t                     uuid_type;                             /**< UUID type for the LED Button Service. */
    ble_ebike_s_unblock_sm_write_handler_t unblock_sm_write_handler;   /**< Event handler to be called when the sm Main Characteristic is written. */
    ble_ebike_s_serial_number_write_handler_t serial_number_write_handler;
    ble_ebike_s_charger_current_high_write_handler_t charger_current_high_write_handler;
    ble_ebike_s_charger_current_low_write_handler_t charger_current_low_write_handler;
    //>>>>>>>>>> Add other write handlers here...
};                                                  

/**@brief Function for handling the Write event (write command)
 *
 * @param[in] p_ebike_s  LED Button Service structure.
 * @param[in] p_ble_evt  Event received from the BLE stack.
 */
static void on_write(ble_ebike_s_t * p_ebike_s, ble_evt_t const * p_ble_evt);

/**@brief Function for initializing the EBike Service.
 *
 * @param[out] p_ebike_s  EBike Service structure. This structure must be supplied by
 *                        the application. It is initialized by this function and will later
 *                        be used to identify this particular service instance.
 * @param[in] p_ebike_s_init  Information needed to initialize the service.
 *
 * @retval NRF_SUCCESS If the service was initialized successfully. Otherwise, an error code is returned.
 */
uint32_t ble_ebike_s_init(ble_ebike_s_t * p_ebike_s, const ble_ebike_s_init_t * p_ebike_s_init);


/**@brief Function for handling the application's BLE stack events related to our service and characteristic.
 *
 * @details This function handles all events from the BLE stack that are of interest to the EBike Service.
 *
 * @param[in] p_ble_evt  Event received from the BLE stack.
 * @param[in] p_context  EBike Service structure.
 */
void ble_ebike_s_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

/**@brief Function for sending a battery voltage notification.
 *
 ' @param[in] conn_handle   Handle of the peripheral connection to which the battery voltage notification will be sent.
 * @param[in] p_ebike_s     EBike Service structure.
 * @param[in] battery_value New battery voltage
 *
 * @retval NRF_SUCCESS If the notification was sent successfully. Otherwise, an error code is returned.
 */
uint32_t ble_ebike_s_batvolt_char_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint16_t *battery_value);
uint32_t ble_ebike_s_battery_current_char_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint32_t *battery_current);
uint32_t ble_ebike_s_charger_current_char_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint16_t *charger_current);
uint32_t ble_ebike_s_curFault_char_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint8_t *curFault);
uint32_t ble_ebike_s_balanceInWork_char_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint8_t *balanceInWork);
uint32_t ble_ebike_s_smMain_char_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint8_t *smMain);

/**@brief Function for sending a serial_number notification.
 *
 ' @param[in] conn_handle   Handle of the peripheral connection to which the battery voltage notification will be sent.
 * @param[in] p_ebike_s     EBike Service structure.
 * @param[in] serial_number New serial_number
 *
 * @retval NRF_SUCCESS If the notification was sent successfully. Otherwise, an error code is returned.
 */
uint32_t ble_ebike_s_serial_number_char_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint32_t* serial_number);
uint32_t ble_ebike_s_unblock_sm_char_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint8_t* unblock_sm);
uint32_t ble_ebike_s_charger_current_high_char_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint16_t* charger_current_high);
uint32_t ble_ebike_s_charger_current_low_char_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint16_t* charger_current_low);
//>>>>>>>>>> Add other update methodes here....

#endif // BLE_EBIKE_SERVICE_H__

/** @} */
