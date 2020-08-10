#include "ble_config.h"

uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                        /**< Handle of the current connection. */
uint8_t m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;                   /**< Advertising handle used to identify an advertising set. */
uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];                    /**< Buffer for storing an encoded advertising set. */
uint8_t m_enc_scan_response_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX];         /**< Buffer for storing an encoded scan data. */

/**@brief Struct that contains pointers to the encoded advertising data. */
ble_gap_adv_data_t m_adv_data =
{
    .adv_data =
    {
        .p_data = m_enc_advdata,
        .len    = BLE_GAP_ADV_SET_DATA_SIZE_MAX
    },
    .scan_rsp_data =
    {
        .p_data = m_enc_scan_response_data,
        .len    = BLE_GAP_ADV_SET_DATA_SIZE_MAX
    }
};

/******************************************************************************/
/* TIMER                                                                      */
/******************************************************************************/
void timers_init(void)
{
    // Initialize timer module.
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    // OUR_JOB: Step 3.H, Initiate our timer
    app_timer_create(&m_ebike_s_timer_id, APP_TIMER_MODE_REPEATED, timer_timeout_handler);
    //call the timeout handler, repeatedly (else APP_TIMER_MODE_SINGLE_SHOT)
}

void timer_timeout_handler(void * p_context)
{
    // OUR_JOB: Step 3.F, Update temperature and characteristic value.
    //spis_send_function_code(0x03);  //
    //ble_ebike_s_batvolt_char_update(m_conn_handle, &m_lbs, &batVolt);  //call the characteristic update function
}

/******************************************************************************/
/* BLE_WRITE: Callback function                                               */
/******************************************************************************/
void unblock_sm_write_handler(uint16_t conn_handle, ble_ebike_s_t * p_ebike_s, uint8_t unblock_sm_state)
{
    NRF_LOG_INFO("<SPI: load tx %x Unblock state machine: %d",FC_UNBLOCK_SM, unblock_sm_state);
    nrf_gpio_pin_set(IRQ_BT_PIN);
    spis_reset_tx_buffer();
    m_tx_buf[0] = FC_UNBLOCK_SM; //function code
    m_tx_buf[1] = SIZE_UNBLOCK_SM; //data size (bytes)
    m_tx_buf[2] = unblock_sm_state;
    nrf_delay_ms(1);  //delay min allow the PIC to detect the pulse
    nrf_gpio_pin_clear(IRQ_BT_PIN);
}
void serial_number_write_handler(uint32_t serial_number_value)
{
    NRF_LOG_INFO("<SPI: load tx %x Serial number: %d",FC_SERIAL_NUMBER, serial_number_value);
    nrf_gpio_pin_set(IRQ_BT_PIN);
    spis_reset_tx_buffer();
    m_tx_buf[0] = FC_SERIAL_NUMBER; //function code
    m_tx_buf[1] = SIZE_SERIAL_NUMBER; //data size (bytes)
    //little to big endian conversion
    m_tx_buf[5] = (uint8_t)  serial_number_value;
    m_tx_buf[4] = (uint8_t) (serial_number_value>>8);
    m_tx_buf[3] = (uint8_t) (serial_number_value>>16);
    m_tx_buf[2] = (uint8_t) (serial_number_value>>24);
    nrf_delay_ms(1);  //delay min allow the PIC to detect the pulse
    nrf_gpio_pin_clear(IRQ_BT_PIN);
}
void charger_current_high_write_handler(uint16_t charger_current_high)
{
    NRF_LOG_INFO("<SPI: load tx %x charger_current_high: %d",FC_CHARGER_CURRENT_HIGH , charger_current_high);
    nrf_gpio_pin_set(IRQ_BT_PIN);
    spis_reset_tx_buffer();
    m_tx_buf[0] = FC_CHARGER_CURRENT_HIGH; //function code
    m_tx_buf[1] = SIZE_CHARGER_CURRENT_HIGH; //data size (bytes)
    //little to big endian conversion --> MSB first
    m_tx_buf[3] = (uint8_t) (charger_current_high);   //LSB
    m_tx_buf[2] = (uint8_t) (charger_current_high>>8);
    nrf_delay_ms(1);  //delay min allow the PIC to detect the pulse
    nrf_gpio_pin_clear(IRQ_BT_PIN);
}
void charger_current_low_write_handler(uint16_t charger_current_low)
{
    NRF_LOG_INFO("<SPI: load tx %x charger_current_low: %d",FC_CHARGER_CURRENT_LOW , charger_current_low);
    nrf_gpio_pin_set(IRQ_BT_PIN);
    spis_reset_tx_buffer();
    m_tx_buf[0] = FC_CHARGER_CURRENT_LOW; //function code
    m_tx_buf[1] = SIZE_CHARGER_CURRENT_LOW; //data size (bytes)
    //little to big endian conversion --> MSB first
    m_tx_buf[3] = (uint8_t) (charger_current_low);   //LSB
    m_tx_buf[2] = (uint8_t) (charger_current_low>>8);
    nrf_delay_ms(1);  //delay min allow the PIC to detect the pulse
    nrf_gpio_pin_clear(IRQ_BT_PIN);
}
// Add other handlers here...


/******************************************************************************/
/* Ble CONFIG                                                                 */
/******************************************************************************/
void gap_params_init(void)
{
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_CYCLING_POWER_SENSOR);
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}

void services_init(void)
{
    ret_code_t         err_code;
    ble_ebike_s_init_t     init = {0};
    nrf_ble_qwr_init_t qwr_init = {0};

    // Initialize Queued Write Module.
    qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    // Initialize EBIKE service.
    // BLE_WRITE: Initialize Our Service module.
    init.unblock_sm_write_handler = unblock_sm_write_handler;
    init.serial_number_write_handler = serial_number_write_handler;
    init.charger_current_high_write_handler = charger_current_high_write_handler;
    init.charger_current_low_write_handler = charger_current_low_write_handler;
    // Add other handlers here...

    // BLE_WRITE: We need to add the init instance pointer to our service instance 
    // Initialize our service
    err_code = ble_ebike_s_init(&m_lbs, &init);
    APP_ERROR_CHECK(err_code);
}

void advertising_init(void)
{
    ret_code_t    err_code;
    ble_advdata_t advdata;
    ble_advdata_t srdata;

    ble_uuid_t adv_uuids[] = {{EBIKE_S_UUID_SERVICE, m_lbs.uuid_type}};

    // Build and set advertising data.
    memset(&advdata, 0, sizeof(advdata));

    advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance = true;
    advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

    memset(&srdata, 0, sizeof(srdata));
    srdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    srdata.uuids_complete.p_uuids  = adv_uuids;

    err_code = ble_advdata_encode(&advdata, m_adv_data.adv_data.p_data, &m_adv_data.adv_data.len);
    APP_ERROR_CHECK(err_code);

    err_code = ble_advdata_encode(&srdata, m_adv_data.scan_rsp_data.p_data, &m_adv_data.scan_rsp_data.len);
    APP_ERROR_CHECK(err_code);

    ble_gap_adv_params_t adv_params;

    // Set advertising parameters.
    memset(&adv_params, 0, sizeof(adv_params));

    adv_params.primary_phy     = BLE_GAP_PHY_1MBPS;
    adv_params.duration        = APP_ADV_DURATION;
    adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
    adv_params.p_peer_addr     = NULL;
    adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY;
    adv_params.interval        = APP_ADV_INTERVAL;

    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_data, &adv_params);
    APP_ERROR_CHECK(err_code);
}

void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);

    
      // BLE_WRITE: Register a handler for BLE our service events.
    // OUR_JOB: Step 3.C Call ble_our_service_on_ble_evt() to do housekeeping of ble connections related to our service and characteristics
    // Needed for associating the observer with the event handler of the service
    //NRF_SDH_BLE_OBSERVER(m_our_service_observer, APP_BLE_OBSERVER_PRIO, ble_our_service_on_ble_evt, (void*) &m_our_service); 
}

void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    ret_code_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}

void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

void conn_params_init(void)
{
    ret_code_t             err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ret_code_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected");
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            app_timer_start(m_ebike_s_timer_id, EBIKE_S_CHAR_TIMER_INTERVAL, NULL);  //start adv timer
            
            //Indicate the smartphone connection to the PIC
            nrf_gpio_pin_set(IRQ_BT_PIN);  
            spis_reset_tx_buffer();
            m_tx_buf[0] = FC_SMARTPHONE_CONNECTED;    //function code
            m_tx_buf[1] = SIZE_SMARTPHONE_CONNECTED;  //data size (bytes)
            m_tx_buf[2] = 0x01;                       //desired function code
            nrf_delay_ms(1);                        //delay min allow the PIC to detect the pulse
            nrf_gpio_pin_clear(IRQ_BT_PIN);
            NRF_LOG_INFO("<SPI: load tx %x Connect: %d",FC_SMARTPHONE_CONNECTED, 0x01);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected");
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            advertising_start();
            app_timer_stop(m_ebike_s_timer_id);  //stop adv timer
            
            //Indicate the samrtphone disconnection to the PIC
            nrf_gpio_pin_set(IRQ_BT_PIN);  
            spis_reset_tx_buffer();
            m_tx_buf[0] = FC_SMARTPHONE_CONNECTED; //function code
            m_tx_buf[1] = SIZE_SMARTPHONE_CONNECTED; //data size (bytes)
            m_tx_buf[2] = 0x00; //desired function code
            nrf_delay_ms(1);                        //delay min allow the PIC to detect the pulse
            nrf_gpio_pin_clear(IRQ_BT_PIN);
            NRF_LOG_INFO("<SPI: load tx %x Connect: %d",FC_SMARTPHONE_CONNECTED, 0x00);
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle,
                                                   BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP,
                                                   NULL,
                                                   NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        default:
            // No implementation needed.
            break;
    }
}

void advertising_start(void)
{
    ret_code_t           err_code;

    err_code = sd_ble_gap_adv_start(m_adv_handle, APP_BLE_CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("Advertise");
}

/******************************************************************************/
/* BLE_READ update ble profil                                                 */
/******************************************************************************/
void update_batVolt(uint16_t* batVolt)
{
    ble_ebike_s_batvolt_char_update(m_conn_handle, &m_lbs, batVolt);  //call the characteristic update function
}
void update_battery_current(uint32_t* battery_current)
{
    ble_ebike_s_battery_current_char_update(m_conn_handle, &m_lbs, battery_current);
}
void update_charger_current(uint16_t* charger_current)
{
    ble_ebike_s_charger_current_char_update(m_conn_handle, &m_lbs, charger_current);
}
void update_curFault(uint8_t* curFault)
{
    ble_ebike_s_curFault_char_update(m_conn_handle, &m_lbs, curFault);
}
void update_balanceInWork(uint8_t* balanceInWork)
{
    ble_ebike_s_balanceInWork_char_update(m_conn_handle, &m_lbs, balanceInWork);
}
void update_smMain(uint8_t* smMain)
{
    ble_ebike_s_smMain_char_update(m_conn_handle, &m_lbs, smMain);
}
//READ & WRITE
void update_pack_serialNumber(uint32_t *serialNumber)
{
    ble_ebike_s_serial_number_char_update(m_conn_handle, &m_lbs, serialNumber);
}
void update_unblock_sm(uint8_t* unblock_sm)
{
    ble_ebike_s_unblock_sm_char_update(m_conn_handle, &m_lbs, unblock_sm);
}
void update_charger_current_high(uint16_t* charger_current_high)
{
    ble_ebike_s_charger_current_high_char_update(m_conn_handle, &m_lbs, charger_current_high);
}
void update_charger_current_low(uint16_t* charger_current_low)
{
    ble_ebike_s_charger_current_low_char_update(m_conn_handle, &m_lbs, charger_current_low);
}

//>>>>>>>>>> Add others update methodes here....