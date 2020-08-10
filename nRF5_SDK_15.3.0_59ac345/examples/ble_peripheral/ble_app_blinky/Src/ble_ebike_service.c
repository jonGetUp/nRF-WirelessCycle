#include "sdk_common.h"
#if NRF_MODULE_ENABLED(BLE_LBS)
#include "ble_ebike_service.h"
#include "ble_srv_common.h"

static void on_write(ble_ebike_s_t * p_ebike_s, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    //unblock_sm Char
    if (   (p_evt_write->handle == p_ebike_s->unblock_sm_char_handles.value_handle)
        && (p_evt_write->len == 1)  //to change
        && (p_ebike_s->unblock_sm_write_handler != NULL))
    {
        p_ebike_s->unblock_sm_write_handler(p_ble_evt->evt.gap_evt.conn_handle, p_ebike_s, p_evt_write->data[0]);
    }

    //serial_number char
    if ((p_evt_write->handle == p_ebike_s->serial_number_char_handles.value_handle)
        && (p_ebike_s->serial_number_write_handler != NULL))
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
        uint32_t *serial_number_value_adr;
        uint32_t serial_number_value_val;
        serial_number_value_adr = (uint32_t*) p_evt_write->data;  //take the adress contain in the pointer
        serial_number_value_val = *serial_number_value_adr;     //take the contain of the pointer

        // Call the write handler function. Implementation is in the main.
        p_ebike_s->serial_number_write_handler(serial_number_value_val);  //Define const!!!
    }
    //charger_current_high char
    if ((p_evt_write->handle == p_ebike_s->charger_current_high_char_handles.value_handle)
        && (p_ebike_s->charger_current_high_write_handler != NULL))
    {
        int8_t len = p_evt_write->len;
        if (len != 2)
        {
            NRF_LOG_INFO("ERROR: incomplete package");
            NRF_LOG_INFO("len: %d", len);
            return;
        }        
        // Data must be sent from in Little Endian Format and 4 bytes
        // Convert the little endian 4 bytes of data into 32 bit unsigned int
        uint16_t *charger_current_high_value_adr;
        uint16_t charger_current_high_value_val;
        charger_current_high_value_adr = (uint32_t*) p_evt_write->data;  //take the adress contain in the pointer
        charger_current_high_value_val = *charger_current_high_value_adr;//take the contain of the pointer

        // Call the write handler function. Implementation is in the main.
        p_ebike_s->charger_current_high_write_handler(charger_current_high_value_val);  //Define const!!!
    }
        //charger_current_low char
    if ((p_evt_write->handle == p_ebike_s->charger_current_low_char_handles.value_handle)
        && (p_ebike_s->charger_current_low_write_handler != NULL))
    {
        int8_t len = p_evt_write->len;
        if (len != 2)
        {
            NRF_LOG_INFO("ERROR: incomplete package");
            NRF_LOG_INFO("len: %d", len);
            return;
        }        
        // Data must be sent from in Little Endian Format and 4 bytes
        // Convert the little endian 4 bytes of data into 32 bit unsigned int
        uint16_t *charger_current_low_value_adr;
        uint16_t charger_current_low_value_val;
        charger_current_low_value_adr = (uint32_t*) p_evt_write->data;  //take the adress contain in the pointer
        charger_current_low_value_val = *charger_current_low_value_adr;//take the contain of the pointer

        // Call the write handler function. Implementation is in the main.
        p_ebike_s->charger_current_low_write_handler(charger_current_low_value_val);  //Define const!!!
    }
    //>>>>>>>>>> Add other handlers here....
}

void  ble_ebike_s_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
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


uint32_t ble_ebike_s_init(ble_ebike_s_t * p_ebike_s, const ble_ebike_s_init_t * p_ebike_s_init)
{
    uint32_t              err_code;
    ble_uuid_t            ble_uuid;
    ble_add_char_params_t add_char_params;

    // Initialize service structure.
    // BLE_WRITE: transfer the pointers from the init instance to the module instance
    p_ebike_s->unblock_sm_write_handler = p_ebike_s_init->unblock_sm_write_handler;
    p_ebike_s->serial_number_write_handler = p_ebike_s_init->serial_number_write_handler;
    p_ebike_s->charger_current_high_write_handler = p_ebike_s_init->charger_current_high_write_handler;
    p_ebike_s->charger_current_low_write_handler = p_ebike_s_init->charger_current_low_write_handler;
    //>>>>>>>>>> Add other handlers here....

    // Add EBIKE service
    ble_uuid128_t base_uuid = {EBIKE_S_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_ebike_s->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_ebike_s->uuid_type;
    ble_uuid.uuid = EBIKE_S_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_ebike_s->service_handle);
    VERIFY_SUCCESS(err_code);

    // Add BatVolt characteristic.
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
    add_char_params.cccd_write_access = SEC_OPEN; //enable notify?

    err_code = characteristic_add(p_ebike_s->service_handle,
                                  &add_char_params,
                                  &p_ebike_s->batvolt_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    // Add Battery_Current characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = EBIKE_S_UUID_BATTERY_CURRENT_CHAR;
    add_char_params.uuid_type         = p_ebike_s->uuid_type;
    add_char_params.init_len          = 4;
    add_char_params.max_len           = 4;
    add_char_params.char_props.read   = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access       = SEC_OPEN; //Access open.
    add_char_params.cccd_write_access = SEC_OPEN; //enable notify?

    err_code = characteristic_add(p_ebike_s->service_handle,
                                  &add_char_params,
                                  &p_ebike_s->battery_current_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    // Add Charger Current characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = EBIKE_S_UUID_CHARGER_CURRENT_CHAR;
    add_char_params.uuid_type         = p_ebike_s->uuid_type;
    add_char_params.init_len          = 2;
    add_char_params.max_len           = 2;
    add_char_params.char_props.read   = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access       = SEC_OPEN; //Access open.
    add_char_params.cccd_write_access = SEC_OPEN; //enable notify?

    err_code = characteristic_add(p_ebike_s->service_handle,
                                  &add_char_params,
                                  &p_ebike_s->charger_current_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    // Add CurFault characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = EBIKE_S_UUID_CURFAULT_CHAR;
    add_char_params.uuid_type         = p_ebike_s->uuid_type;
    add_char_params.init_len          = 1;
    add_char_params.max_len           = 1;
    add_char_params.char_props.read   = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access       = SEC_OPEN; //Access open.
    add_char_params.cccd_write_access = SEC_OPEN; //enable notify?

    err_code = characteristic_add(p_ebike_s->service_handle,
                                  &add_char_params,
                                  &p_ebike_s->curFault_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    // Add balanceInWork characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = EBIKE_S_UUID_BALANCEINWORK_CHAR;
    add_char_params.uuid_type         = p_ebike_s->uuid_type;
    add_char_params.init_len          = 1;
    add_char_params.max_len           = 1;
    add_char_params.char_props.read   = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access       = SEC_OPEN; //Access open.
    add_char_params.cccd_write_access = SEC_OPEN; //enable notify?

    err_code = characteristic_add(p_ebike_s->service_handle,
                                  &add_char_params,
                                  &p_ebike_s->balanceInWork_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    // Add smMain characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = EBIKE_S_UUID_SMMAIN_CHAR;
    add_char_params.uuid_type         = p_ebike_s->uuid_type;
    add_char_params.init_len          = 1;
    add_char_params.max_len           = 1;
    add_char_params.char_props.read   = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access       = SEC_OPEN; //Access open.
    add_char_params.cccd_write_access = SEC_OPEN; //enable notify?

    err_code = characteristic_add(p_ebike_s->service_handle,
                                  &add_char_params,
                                  &p_ebike_s->smMain_char_handles);
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

    err_code = characteristic_add(p_ebike_s->service_handle,
                                  &add_char_params,
                                  &p_ebike_s->unblock_sm_char_handles);
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

    err_code = characteristic_add(p_ebike_s->service_handle, &add_char_params, &p_ebike_s->serial_number_char_handles);  // adding characteristic handles to the attribute table
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    // Add Charger_Current_high characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid             = EBIKE_S_UUID_CHARGER_CURRENT_HIGH_CHAR;
    add_char_params.uuid_type        = p_ebike_s->uuid_type;
    add_char_params.max_len          = 2;
    add_char_params.init_len         = 2;
    add_char_params.char_props.read  = 1;
    add_char_params.char_props.write = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access  = SEC_OPEN;  //Access open.
    add_char_params.write_access = SEC_OPEN;
    add_char_params.cccd_write_access = SEC_OPEN;

    err_code = characteristic_add(p_ebike_s->service_handle, &add_char_params, &p_ebike_s->charger_current_high_char_handles);  // adding characteristic handles to the attribute table
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    // Add Charger_Current_low characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid             = EBIKE_S_UUID_CHARGER_CURRENT_LOW_CHAR;
    add_char_params.uuid_type        = p_ebike_s->uuid_type;
    add_char_params.max_len          = 2;
    add_char_params.init_len         = 2;
    add_char_params.char_props.read  = 1;
    add_char_params.char_props.write = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access  = SEC_OPEN;  //Access open.
    add_char_params.write_access = SEC_OPEN;
    add_char_params.cccd_write_access = SEC_OPEN;

    err_code = characteristic_add(p_ebike_s->service_handle, &add_char_params, &p_ebike_s->charger_current_low_char_handles);  // adding characteristic handles to the attribute table
    return err_code;
    //>>>>>>>>>> Add others here...
}

// ALREADY_DONE_FOR_YOU: Function to be called when updating characteristic value
uint32_t ble_ebike_s_batvolt_char_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint16_t *battery_value)
{
  // implement the notification on connection
  // OUR_JOB: Step 3.E, Update characteristic value
  if (conn_handle != BLE_CONN_HANDLE_INVALID)//housekeeping allow to check if we are in a valid connection
  {
      uint16_t               len = 2;
      ble_gatts_hvx_params_t hvx_params;          //Handle Value X(notification or indication)
      memset(&hvx_params, 0, sizeof(hvx_params));

      hvx_params.handle = p_ebike_s->batvolt_char_handles.value_handle; //which characteristic value we are working on
      hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;                //notification
      hvx_params.offset = 0;      //characteristic value might be a sequence of many bytes.
      hvx_params.p_len  = &len;   //number of bytes to transmitt
      hvx_params.p_data = (uint8_t*)battery_value;  //data pointer

      return sd_ble_gatts_hvx(conn_handle, &hvx_params);
  }
  return NRF_SUCCESS; //nothing has been send
}
uint32_t ble_ebike_s_battery_current_char_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint32_t *battery_current)
{
  if (conn_handle != BLE_CONN_HANDLE_INVALID)//housekeeping allow to check if we are in a valid connection
  {
      uint16_t               len = 4;
      ble_gatts_hvx_params_t hvx_params;          //Handle Value X(notification or indication)
      memset(&hvx_params, 0, sizeof(hvx_params));

      hvx_params.handle = p_ebike_s->battery_current_char_handles.value_handle; //which characteristic value we are working on
      hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;                //notification
      hvx_params.offset = 0;      //characteristic value might be a sequence of many bytes.
      hvx_params.p_len  = &len;   //number of bytes to transmitt
      hvx_params.p_data = (uint8_t*)battery_current;  //data pointer

      return sd_ble_gatts_hvx(conn_handle, &hvx_params);
  }
  return NRF_SUCCESS; //nothing has been send
}
uint32_t ble_ebike_s_charger_current_char_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint16_t *charger_current)
{
  if (conn_handle != BLE_CONN_HANDLE_INVALID)//housekeeping allow to check if we are in a valid connection
  {
      uint16_t               len = 2;
      ble_gatts_hvx_params_t hvx_params;          //Handle Value X(notification or indication)
      memset(&hvx_params, 0, sizeof(hvx_params));

      hvx_params.handle = p_ebike_s->charger_current_char_handles.value_handle; //which characteristic value we are working on
      hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;                //notification
      hvx_params.offset = 0;      //characteristic value might be a sequence of many bytes.
      hvx_params.p_len  = &len;   //number of bytes to transmitt
      hvx_params.p_data = (uint8_t*)charger_current;  //data pointer

      return sd_ble_gatts_hvx(conn_handle, &hvx_params);
  }
  return NRF_SUCCESS; //nothing has been send
}
uint32_t ble_ebike_s_curFault_char_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint8_t *curFault)
{
  if (conn_handle != BLE_CONN_HANDLE_INVALID)//housekeeping allow to check if we are in a valid connection
  {
      uint16_t               len = 1;
      ble_gatts_hvx_params_t hvx_params;          //Handle Value X(notification or indication)
      memset(&hvx_params, 0, sizeof(hvx_params));

      hvx_params.handle = p_ebike_s->curFault_char_handles.value_handle; //which characteristic value we are working on
      hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;                //notification
      hvx_params.offset = 0;      //characteristic value might be a sequence of many bytes.
      hvx_params.p_len  = &len;   //number of bytes to transmitt
      hvx_params.p_data = (uint8_t*)curFault;  //data pointer

      return sd_ble_gatts_hvx(conn_handle, &hvx_params);
  }
  return NRF_SUCCESS; //nothing has been send
}
uint32_t ble_ebike_s_balanceInWork_char_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint8_t *balanceInWork)
{
  if (conn_handle != BLE_CONN_HANDLE_INVALID)//housekeeping allow to check if we are in a valid connection
  {
      uint16_t               len = 1;
      ble_gatts_hvx_params_t hvx_params;          //Handle Value X(notification or indication)
      memset(&hvx_params, 0, sizeof(hvx_params));

      hvx_params.handle = p_ebike_s->balanceInWork_char_handles.value_handle; //which characteristic value we are working on
      hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;                //notification
      hvx_params.offset = 0;      //characteristic value might be a sequence of many bytes.
      hvx_params.p_len  = &len;   //number of bytes to transmitt
      hvx_params.p_data = (uint8_t*)balanceInWork;  //data pointer

      return sd_ble_gatts_hvx(conn_handle, &hvx_params);
  }
  return NRF_SUCCESS; //nothing has been send
}
uint32_t ble_ebike_s_smMain_char_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint8_t *smMain)
{
  if (conn_handle != BLE_CONN_HANDLE_INVALID)//housekeeping allow to check if we are in a valid connection
  {
      uint16_t               len = 1;
      ble_gatts_hvx_params_t hvx_params;          //Handle Value X(notification or indication)
      memset(&hvx_params, 0, sizeof(hvx_params));

      hvx_params.handle = p_ebike_s->smMain_char_handles.value_handle; //which characteristic value we are working on
      hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;                //notification
      hvx_params.offset = 0;      //characteristic value might be a sequence of many bytes.
      hvx_params.p_len  = &len;   //number of bytes to transmitt
      hvx_params.p_data = (uint8_t*)smMain;  //data pointer

      return sd_ble_gatts_hvx(conn_handle, &hvx_params);
  }
  return NRF_SUCCESS; //nothing has been send
}




// ALREADY_DONE_FOR_YOU: Function to be called when updating characteristic value
uint32_t ble_ebike_s_serial_number_char_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint32_t * serial_number)
{
  // implement the notification on connection
  // OUR_JOB: Step 3.E, Update characteristic value
  if (conn_handle != BLE_CONN_HANDLE_INVALID)//housekeeping allow to check if we are in a valid connection
  {
      uint16_t               len = 4;
      ble_gatts_hvx_params_t hvx_params;          //Handle Value X(notification or indication)
      memset(&hvx_params, 0, sizeof(hvx_params));

      hvx_params.handle = p_ebike_s->serial_number_char_handles.value_handle; //which characteristic value we are working on
      hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;          //notification
      hvx_params.offset = 0;      //characteristic value might be a sequence of many bytes.
      hvx_params.p_len  = &len;   //number of bytes to transmitt
      hvx_params.p_data = (uint8_t*)serial_number;  //data pointer

      return sd_ble_gatts_hvx(conn_handle, &hvx_params);
  }
  return NRF_SUCCESS; //nothing has been send
}
uint32_t ble_ebike_s_unblock_sm_char_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint8_t * unblock_sm)
{
  // implement the notification on connection
  // OUR_JOB: Step 3.E, Update characteristic value
  if (conn_handle != BLE_CONN_HANDLE_INVALID)     //housekeeping allow to check if we are in a valid connection
  {
      uint16_t               len = 1;
      ble_gatts_hvx_params_t hvx_params;          //Handle Value X(notification or indication)
      memset(&hvx_params, 0, sizeof(hvx_params));

      hvx_params.handle = p_ebike_s->unblock_sm_char_handles.value_handle; //which characteristic value we are working on
      hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;          //notification
      hvx_params.offset = 0;      //characteristic value might be a sequence of many bytes.
      hvx_params.p_len  = &len;   //number of bytes to transmitt
      hvx_params.p_data = (uint8_t*)unblock_sm;  //data pointer

      return sd_ble_gatts_hvx(conn_handle, &hvx_params);
  }
  return NRF_SUCCESS; //nothing has been send
}
uint32_t ble_ebike_s_charger_current_high_char_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint16_t * charger_current_high)
{
  if (conn_handle != BLE_CONN_HANDLE_INVALID)     //housekeeping allow to check if we are in a valid connection
  {
      uint16_t               len = 2;
      ble_gatts_hvx_params_t hvx_params;          //Handle Value X(notification or indication)
      memset(&hvx_params, 0, sizeof(hvx_params));

      hvx_params.handle = p_ebike_s->charger_current_high_char_handles.value_handle; //which characteristic value we are working on
      hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;      //notification
      hvx_params.offset = 0;                              //characteristic value might be a sequence of many bytes.
      hvx_params.p_len  = &len;                           //number of bytes to transmitt
      hvx_params.p_data = (uint8_t*)charger_current_high; //data pointer

      return sd_ble_gatts_hvx(conn_handle, &hvx_params);
  }
  return NRF_SUCCESS; //nothing has been send
}
uint32_t ble_ebike_s_charger_current_low_char_update(uint16_t conn_handle, ble_ebike_s_t *p_ebike_s, uint16_t * charger_current_low)
{
  if (conn_handle != BLE_CONN_HANDLE_INVALID)     //housekeeping allow to check if we are in a valid connection
  {
      uint16_t               len = 2;
      ble_gatts_hvx_params_t hvx_params;          //Handle Value X(notification or indication)
      memset(&hvx_params, 0, sizeof(hvx_params));

      hvx_params.handle = p_ebike_s->charger_current_low_char_handles.value_handle; //which characteristic value we are working on
      hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;      //notification
      hvx_params.offset = 0;                              //characteristic value might be a sequence of many bytes.
      hvx_params.p_len  = &len;                           //number of bytes to transmitt
      hvx_params.p_data = (uint8_t*)charger_current_low; //data pointer

      return sd_ble_gatts_hvx(conn_handle, &hvx_params);
  }
  return NRF_SUCCESS; //nothing has been send
}
//>>>>>>>>>> Add others update methodes here....


#endif // NRF_MODULE_ENABLED(BLE_LBS)
