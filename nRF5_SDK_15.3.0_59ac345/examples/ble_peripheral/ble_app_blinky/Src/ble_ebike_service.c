#include "sdk_common.h"
#if NRF_MODULE_ENABLED(BLE_LBS)
#include "ble_ebike_service.h"
#include "ble_srv_common.h"

static void on_write(ble_ebike_s_t * p_ebike_s, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    //LED Char
    if (   (p_evt_write->handle == p_ebike_s->led_char_handles.value_handle)
        && (p_evt_write->len == 1)  //to change
        && (p_ebike_s->led_write_handler != NULL))
    {
        p_ebike_s->led_write_handler(p_ble_evt->evt.gap_evt.conn_handle, p_ebike_s, p_evt_write->data[0]);
    }

    //Our char
    if ((p_evt_write->handle == p_ebike_s->char_handles_1.value_handle)
        && (p_ebike_s->characteristic1_value_write_handler != NULL))
    {
        // Make sure that the data is 4 bytes (or whatever the size of your characteristic)
        // It has to match the exact byte size of the characteristic to avoid problems
        int8_t len = p_evt_write->len;
        if (len != 4)
        {
            NRF_LOG_INFO("ERROR: incomplete package");
            NRF_LOG_INFO("len: %d", len);
            return;
        }
        
        // Data must be sent from in Little Endian Format and 4 bytes
        // Convert the little endian 4 bytes of data into 32 bit unsigned int
        uint32_t *characteristic1_value_adr;
        uint32_t characteristic1_value_val;
        characteristic1_value_adr = (uint32_t*) p_evt_write->data;  //take the adress contain in the pointer
        characteristic1_value_val = *characteristic1_value_adr;     //take the contain of the pointer

        // Call the write handler function. Implementation is in the main.
        p_ebike_s->characteristic1_value_write_handler(characteristic1_value_val);  //Define const!!!
    }
}

void  ble_lbs_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_ebike_s_t * p_ebike_s = (ble_ebike_s_t *)p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GATTS_EVT_WRITE:
            on_write(p_ebike_s, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}


uint32_t ble_lbs_init(ble_ebike_s_t * p_ebike_s, const ble_ebike_s_init_t * p_ebike_s_init)
{
    uint32_t              err_code;
    ble_uuid_t            ble_uuid;
    ble_add_char_params_t add_char_params;

    // Initialize service structure.
    p_ebike_s->led_write_handler = p_ebike_s_init->led_write_handler;
    // BLE_WRITE: transfer the pointers from the init instance to the module instance
    p_ebike_s->characteristic1_value_write_handler = p_ebike_s_init->characteristic1_value_write_handler;

    // Add service.
    ble_uuid128_t base_uuid = {EBIKE_S_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_ebike_s->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_ebike_s->uuid_type;
    ble_uuid.uuid = EBIKE_S_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_ebike_s->service_handle);
    VERIFY_SUCCESS(err_code);

    // Add Button characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = EBIKE_S_UUID_BATVOLT_CHAR;
    add_char_params.uuid_type         = p_ebike_s->uuid_type;
    add_char_params.init_len          = 2;
    add_char_params.max_len           = 2;
    uint8_t value[2]                  = {0xD2,0x04};  //little endian
    add_char_params.p_init_value      = value; //init first value
    add_char_params.char_props.read   = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access       = SEC_OPEN; //Access open.
    add_char_params.cccd_write_access = SEC_OPEN; 

    //enable notify?

    err_code = characteristic_add(p_ebike_s->service_handle,
                                  &add_char_params,
                                  &p_ebike_s->button_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add UNBLOCK_SM characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid             = EBIKE_S_UUID_UNBLOCK_SM_CHAR;
    add_char_params.uuid_type        = p_ebike_s->uuid_type;
    add_char_params.max_len          = sizeof(uint8_t);
    add_char_params.init_len         = sizeof(uint8_t);
    add_char_params.char_props.read  = 1;
    add_char_params.char_props.write = 1;

    add_char_params.read_access  = SEC_OPEN;  //Access open.
    add_char_params.write_access = SEC_OPEN;

    characteristic_add(p_ebike_s->service_handle, &add_char_params, &p_ebike_s->led_char_handles);

    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add Serial Number characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid             = EBIKE_S_UUID_SERIAL_NUMBER_CHAR;
    add_char_params.uuid_type        = p_ebike_s->uuid_type;
    add_char_params.max_len          = 4;
    add_char_params.init_len         = 4;
    uint8_t value2[4]                = {0x68,0xCE,0x00,0x00}; //little endian
    add_char_params.p_init_value     = value2;
    add_char_params.char_props.read  = 1;
    add_char_params.char_props.write = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access  = SEC_OPEN;  //Access open.
    add_char_params.write_access = SEC_OPEN;
    add_char_params.cccd_write_access = SEC_OPEN;

    characteristic_add(p_ebike_s->service_handle, &add_char_params, &p_ebike_s->char_handles_1);  // adding characteristic handles to the attribute table
    return err_code;
}

// ALREADY_DONE_FOR_YOU: Function to be called when updating characteristic value
uint32_t ble_lbs_batVolt_characteristic_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint16_t *battery_value)
{
  // implement the notification on connection
  // OUR_JOB: Step 3.E, Update characteristic value
  if (conn_handle != BLE_CONN_HANDLE_INVALID)//housekeeping allow to check if we are in a valid connection
  {
        uint16_t               len = 2;
        ble_gatts_hvx_params_t hvx_params;          //Handle Value X(notification or indication)
        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_ebike_s->button_char_handles.value_handle; //which characteristic value we are working on
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;                //notification
        hvx_params.offset = 0;      //characteristic value might be a sequence of many bytes.
        hvx_params.p_len  = &len;   //number of bytes to transmitt
        hvx_params.p_data = (uint8_t*)battery_value;  //data pointer

        return sd_ble_gatts_hvx(conn_handle, &hvx_params);
  }
  return NRF_SUCCESS; //nothing has been send
}

// ALREADY_DONE_FOR_YOU: Function to be called when updating characteristic value
uint32_t ble_lbs_characteristic_1_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint32_t * serial_number)
{
  // implement the notification on connection
  // OUR_JOB: Step 3.E, Update characteristic value
  if (conn_handle != BLE_CONN_HANDLE_INVALID)//housekeeping allow to check if we are in a valid connection
  {
        uint16_t               len = 4;
        ble_gatts_hvx_params_t hvx_params;          //Handle Value X(notification or indication)
        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_ebike_s->char_handles_1.value_handle; //which characteristic value we are working on
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;          //notification
        hvx_params.offset = 0;      //characteristic value might be a sequence of many bytes.
        hvx_params.p_len  = &len;   //number of bytes to transmitt
        hvx_params.p_data = (uint8_t*)serial_number;  //data pointer

        return sd_ble_gatts_hvx(conn_handle, &hvx_params);
      }
  return NRF_SUCCESS; //nothing has been send
}


#endif // NRF_MODULE_ENABLED(BLE_LBS)
