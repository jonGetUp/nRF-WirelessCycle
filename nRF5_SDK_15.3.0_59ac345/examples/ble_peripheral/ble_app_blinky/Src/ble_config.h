/********************************************************************************/
/**
 * @file ble_config.h
 *
 * @brief Bluetooth profil configuration and usage
 *
 * @author Gaspoz Jonathan
 *
 */
/********************************************************************************/
#ifndef BLE_CONFIG_H__
#define BLE_CONFIG_H__

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_sdh.h"

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"

#include "ble_ebike_service.h"
#include "ble.h"
#include "ble_err.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "boards.h"
#include "app_timer.h"
#include "app_button.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"

#include "nrf_delay.h"

#include "spi.h"


//#ifdef __cplusplus
//extern "C" {
//#endif

#define DEVICE_NAME                     "EBike"                         /**< Name of device. Will be included in the advertising data. */

#define APP_BLE_OBSERVER_PRIO           3                                       /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                       /**< A tag identifying the SoftDevice BLE configuration. */

#define APP_ADV_INTERVAL                64                                      /**< The advertising interval (in units of 0.625 ms; this value corresponds to 40 ms). */
#define APP_ADV_DURATION                BLE_GAP_ADV_TIMEOUT_GENERAL_UNLIMITED   /**< The advertising time-out (in units of seconds). When set to 0, we will never time out. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)        /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS)        /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                   0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)         /**< Connection supervisory time-out (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(20000)                  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (15 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000)                   /**< Time between each call to sd_ble_gap_conn_param_update after the first call (5 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                       /**< Number of attempts before giving up the connection parameter negotiation. */

/*GAP security parameters -> see advertising tuto*/
//#define SEC_PARAM_BOND                  1                                       /**< Perform bonding. */
//#define SEC_PARAM_MITM                  0                                       /**< Man In The Middle protection not required. */
//#define SEC_PARAM_LESC                  0                                       /**< LE Secure Connections not enabled. */
//#define SEC_PARAM_KEYPRESS              0                                       /**< Keypress notifications not enabled. */
//#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                    /**< No I/O capabilities. */
//#define SEC_PARAM_OOB                   0                                       /**< Out Of Band data not available. */
//#define SEC_PARAM_MIN_KEY_SIZE          7                                       /**< Minimum encryption key size. */
//#define SEC_PARAM_MAX_KEY_SIZE          16                                      /**< Maximum encryption key size. */

#define DEAD_BEEF                       0xDEADBEEF                              /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define IRQ_BT_PIN                      NRF_GPIO_PIN_MAP(1,4)                  /**< Interrupt request for SPI slave */

extern uint16_t m_conn_handle;                        /**< Handle of the current connection. */
extern uint8_t m_adv_handle;                   /**< Advertising handle used to identify an advertising set. */
extern uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];                    /**< Buffer for storing an encoded advertising set. */
extern uint8_t m_enc_scan_response_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX];         /**< Buffer for storing an encoded scan data. */

extern ble_gap_adv_data_t m_adv_data;

//! m_ indicate static!!!!!
BLE_LBS_DEF(m_lbs);                                                             /**< data structure used to control LED Button Service instance. */
NRF_BLE_GATT_DEF(m_gatt);                                                       /**< data structure used to control GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                         /**< Context for the Queued Write module.*/
//
//

//Timer
// OUR_JOB: Step 3.G, Declare an app_timer id variable and define our timer interval and define a timer interval
APP_TIMER_DEF(m_lbs_timer_id);
#define LBS_CHAR_TIMER_INTERVAL     APP_TIMER_TICKS(1000) // 1000 ms intervals

void timers_init(void);
void timer_timeout_handler(void * p_context);

void led_write_handler(uint16_t conn_handle, ble_ebike_s_t * p_ebike_s, uint8_t led_state);
void characteristic1_value_write_handler(uint32_t characteristic1_value);

void gap_params_init(void);
void gatt_init(void);
void services_init(void);
void advertising_init(void);
void ble_stack_init(void);
void nrf_qwr_error_handler(uint32_t nrf_error);

void on_conn_params_evt(ble_conn_params_evt_t * p_evt);
void conn_params_error_handler(uint32_t nrf_error);
void conn_params_init(void);
void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);

void advertising_start(void);

void update_batVolt(uint16_t* batVolt);
void update_pack_serialNumber(uint32_t *serialNumber);


///** @} */
//#ifdef __cplusplus
//}
//#endif
#endif /* BLE_CONFIG_H__ */

/**
  @}
  @}
*/
