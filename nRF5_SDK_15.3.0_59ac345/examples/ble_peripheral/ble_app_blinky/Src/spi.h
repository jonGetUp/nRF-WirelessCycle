/********************************************************************************/
/**
 * @file spi.h
 *
 * @brief SPI slave peripheral configuration and usage
 *
 * @author Gaspoz Jonathan
 *
 */
/********************************************************************************/
#ifndef SPI_H__
#define SPI_H__

#include "nrf_drv_spis.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_sdh.h"

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "boards.h"
#include "ble_ebike_service.h"
#include "ble_config.h"


/****SPI****/
#define SPIS_INSTANCE 1 /**< SPIS instance index. */
#define SPI_BUFFER_SIZE 10

//Data packets size
#define SIZE_BATVOLT 0x02
#define SIZE_SERIAL_NUMBER 0x04
#define SIZE_SMARTPHONE_CONNECTED 0x01
#define SIZE_UNBLOCK_SM 0x01
    
//Function Code
typedef enum
{
    FC_BATVOLT = 0x02,
    FC_SERIAL_NUMBER = 0x05,
    FC_UNBLOCK_SM = 0x0C,
    FC_SMARTPHONE_CONNECTED = 0x0D
}FC_ENUM;

struct BMS_STATE
{
    uint16_t            batVolt;                // battery voltage in mV
    //--------------------------------------------------------------------------
    uint32_t            pack_serialNumber;      // Smartphone admin password
    //--------------------------------------------------------------------------
    uint8_t             unblock_sm;             // Allow to unblock the state machine when in SM_BATTERY_DEAD
};

extern struct BMS_STATE bmsState; 

extern uint8_t       m_tx_buf[SPI_BUFFER_SIZE];   /**< TX buffer.*/
extern uint8_t       m_rx_buf[sizeof(m_tx_buf)];

/** @Brief Function for configuring the spi peripheral as slave
 */
void spi_init(void);

/**
 * @brief SPIS user event handler.
 *
 * @param event
 */
void spis_event_handler(nrf_drv_spis_event_t event);

/** @Brief Function call in the while(1), handle the SPI communications
 */
void spis_handle(void);

/** @brief Reset the tx_Buffer to 0x00, to override all tx bytes
 */
void spis_reset_tx_buffer(void);



///** @} */
#endif /* SPI_H__ */

/**
  @}
  @}
*/
