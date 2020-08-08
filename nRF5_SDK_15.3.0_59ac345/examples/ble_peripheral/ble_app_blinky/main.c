
/**
 * @brief Blinky Sample Application main file.
 *
 * This file contains the source code for a sample server application using the LED Button service.
 */

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "boards.h"

#include "app_error.h"


//#include "ble.h"
//#include "ble_err.h"
//#include "ble_hci.h"
//#include "ble_srv_common.h"
//#include "ble_advdata.h"
//#include "ble_conn_params.h"
//#include "nrf_sdh.h"
//#include "nrf_sdh_ble.h"
//#include "boards.h"
//#include "app_timer.h"
//#include "app_button.h"
//#include "ble_lbs.h"
//#include "nrf_ble_gatt.h"
//#include "nrf_ble_qwr.h"

#include "nrf_pwr_mgmt.h"

#include "ble_ebike_service.h"
#include "ble_config.h"
#include "spi.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

//prototype declaration
//static void led_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t led_state);
//static void characteristic1_value_write_handler(uint32_t characteristic1_value);

//static void timers_init(void);
//static void timer_timeout_handler(void * p_context);

static void log_init(void);
static void power_management_init(void);
static void gpio_output_voltage_setup(void);
static void idle_state_handle(void);
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name);

///******************************************************************************/
///* BLE_WRITE: Callback function                                               */
///******************************************************************************/
//
///**@brief Function for handling write events to the LED characteristic.
// *
// * @param[in] p_lbs     Instance of LED Button Service to which the write applies.
// * @param[in] led_state Written/desired state of the LED.
// */
//static void led_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t led_state)
//{
//    //the led characteristic have been write
//    if (led_state)
//    {
//        NRF_LOG_INFO("Received LED ON!");
//    }
//    else
//    {
//        NRF_LOG_INFO("Received LED OFF!");
//    }
//}
//
///*******************************************************************************
// * @brief  Write a given value in eeprom
// * @param  characteristic1_value containing the value that was received from the phone
// ******************************************************************************/
//static void characteristic1_value_write_handler(uint32_t characteristic1_value)
//{
//    NRF_LOG_INFO("Write command, Serial number:  %x", characteristic1_value);
//
//    nrf_gpio_pin_set(IRQ_BT_PIN);
//    spis_reset_tx_buffer();
//    m_tx_buf[0] = FC_SERIAL_NUMBER; //function code
//    m_tx_buf[1] = 0x04; //data size (bytes)
//    //little to big endian conversion
//    m_tx_buf[5] = (uint8_t)  characteristic1_value;
//    m_tx_buf[4] = (uint8_t) (characteristic1_value>>8);
//    m_tx_buf[3] = (uint8_t) (characteristic1_value>>16);
//    m_tx_buf[2] = (uint8_t) (characteristic1_value>>24);
//    nrf_delay_us(100);  //delay min allow the PIC to detect the pulse
//    nrf_gpio_pin_clear(IRQ_BT_PIN);
//}
//// Add other handlers here...



///******************************************************************************/
///* TIMER                                                                      */
///******************************************************************************/
///**@brief Function for the Timer initialization.
// *
// * @details Initializes the timer module. This creates and starts application timers.
// */
//static void timers_init(void)
//{
//    // Initialize timer module.
//    ret_code_t err_code = app_timer_init();
//    APP_ERROR_CHECK(err_code);
//
//    // OUR_JOB: Step 3.H, Initiate our timer
//    app_timer_create(&m_lbs_timer_id, APP_TIMER_MODE_REPEATED, timer_timeout_handler);
//    //call the timeout handler, repeatedly (else APP_TIMER_MODE_SINGLE_SHOT)
//}
//
//static void timer_timeout_handler(void * p_context)
//{
//    // OUR_JOB: Step 3.F, Update temperature and characteristic value.
//    //spis_send_function_code(0x03);  //
//    //ble_lbs_batVolt_characteristic_update(m_conn_handle, &m_lbs, &batVolt);  //call the characteristic update function
//}



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


///**
// * Function for configuring the spi peripheral as slave
// */
//void spi_init(void)//static 
//{
//    // Enable the constant latency sub power mode to minimize the time it takes
//    // for the SPIS peripheral to become active after the CSN line is asserted
//    // (when the CPU is in sleep mode).
//    NRF_POWER->TASKS_CONSTLAT = 1;
//    
//    //SPI configuration
//    nrf_drv_spis_config_t spis_config = NRF_DRV_SPIS_DEFAULT_CONFIG;
//    spis_config.csn_pin               = APP_SPIS_CS_PIN;
//    spis_config.miso_pin              = APP_SPIS_MISO_PIN;
//    spis_config.mosi_pin              = APP_SPIS_MOSI_PIN;
//    spis_config.sck_pin               = APP_SPIS_SCK_PIN;
//    //SPI init with event handler callback function
//    APP_ERROR_CHECK(nrf_drv_spis_init(&spis, &spis_config, spis_event_handler));
//}


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
        //idle_state_handle();
    }
}


/**
 * @}
 */
