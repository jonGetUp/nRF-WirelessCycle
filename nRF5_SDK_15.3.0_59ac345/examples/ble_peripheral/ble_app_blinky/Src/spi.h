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
#include "ble_lbs.h"


#ifdef __cplusplus
extern "C" {
#endif

/****SPI****/
#define SPIS_INSTANCE 1 /**< SPIS instance index. */
#define ILLEGAL_FUNCTION 0x01
#define SPI_BUFFER_SIZE 10
#define FC_BATVOLT 0x03
#define FC_SERIAL_NUMBER 0x0A
#define FC_SMARTPHONE_CONNECTED 0x1D
#define SIZE_SMARTPHONE_CONNECTED 0x01

extern uint8_t       m_tx_buf[SPI_BUFFER_SIZE];   /**< TX buffer.*/
extern uint8_t       m_rx_buf[sizeof(m_tx_buf)];

void spi_init(void);
void spis_event_handler(nrf_drv_spis_event_t event);
void spis_handle(void);
void spis_reset_tx_buffer(void);


/** @} */
#ifdef __cplusplus
}
#endif
#endif /* SPI_H__ */

/**
  @}
  @}
*/
