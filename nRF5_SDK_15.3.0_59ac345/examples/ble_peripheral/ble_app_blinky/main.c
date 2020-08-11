/********************************************************************************/
/**
 * @file ble_config.h
 *
 * @brief Connected Ebike Application main file.
 *        This file contains the source code for a server application using the EBIke service.
 *
 * @author Gaspoz Jonathan
 *
 */
/********************************************************************************/

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "boards.h"
#include "app_error.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "ble_ebike_service.h"
#include "ble_config.h"
#include "spi.h"

/******************************************************************************/
/* nRF Init                                                                   */
/******************************************************************************/
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

/**
 * Function for configuring UICR_REGOUT0 register
 * to set GPIO output voltage to 3.3V.
 * PIC18f can only detect voltage above 3V
 */
static void gpio_output_voltage_setup(void)
{
    if((NRF_UICR->REGOUT0 & (uint32_t)UICR_REGOUT0_VOUT_Msk) != (UICR_REGOUT0_VOUT_3V3 << UICR_REGOUT0_VOUT_Pos))
    {   //check if the register has already the correct settings
        // Set regulator settings in UICR.
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen; /*!< Write enabled */
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}

        NRF_UICR->REGOUT0 = (NRF_UICR->REGOUT0 & ~((uint32_t)UICR_REGOUT0_VOUT_Msk)) |
                            (UICR_REGOUT0_VOUT_3V3 << UICR_REGOUT0_VOUT_Pos);

        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren; /*!< Read only access */
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}

        // System reset is needed to update UICR registers.
        NVIC_SystemReset();
    }
}

/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
static void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}

/**@brief Function for assert macro callback.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for application main entry.
 */
int main(void)
{
    //--------Initialisation--------
    //nRF
    log_init();
    power_management_init();
    gpio_output_voltage_setup();
    nrf_gpio_cfg_output(IRQ_BT_PIN);

    //SPI slave
    spi_init();

    //bluetooth profil
    timers_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    advertising_init();
    conn_params_init();

    // Start execution.
    NRF_LOG_INFO("Connected Ebike program started.");
    advertising_start();
   

    // Enter main loop.
    while(1)
    {
        spis_handle();
    }
}
