/**
 * Copyright (c) 2013 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "sdk_common.h"
#if NRF_MODULE_ENABLED(BLE_LBS)
#include "ble_lbs.h"
#include "ble_srv_common.h"


/**@brief Function for handling the Write event (write command)
 *
 * @param[in] p_lbs      LED Button Service structure.
 * @param[in] p_ble_evt  Event received from the BLE stack.
 */
static void on_write(ble_lbs_t * p_lbs, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    //LED Char
    if (   (p_evt_write->handle == p_lbs->led_char_handles.value_handle)
        && (p_evt_write->len == 1)  //to change
        && (p_lbs->led_write_handler != NULL))
    {
        p_lbs->led_write_handler(p_ble_evt->evt.gap_evt.conn_handle, p_lbs, p_evt_write->data[0]);
    }

    //Our char
    if ((p_evt_write->handle == p_lbs->char_handles_1.value_handle)
        && (p_lbs->characteristic1_value_write_handler != NULL))
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
        characteristic1_value_adr = (uint32_t*) p_evt_write->data;
        characteristic1_value_val = *characteristic1_value_adr;

        // Call the write handler function. Implementation is in the main.
        p_lbs->characteristic1_value_write_handler(characteristic1_value_val);
    }
}

void  ble_lbs_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_lbs_t * p_lbs = (ble_lbs_t *)p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GATTS_EVT_WRITE:
            on_write(p_lbs, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}


uint32_t ble_lbs_init(ble_lbs_t * p_lbs, const ble_lbs_init_t * p_lbs_init)
{
    uint32_t              err_code;
    ble_uuid_t            ble_uuid;
    ble_add_char_params_t add_char_params;

    // Initialize service structure.
    p_lbs->led_write_handler = p_lbs_init->led_write_handler;
    // BLE_WRITE: transfer the pointers from the init instance to the module instance
    p_lbs->characteristic1_value_write_handler = p_lbs_init->characteristic1_value_write_handler;

    // Add service.
    ble_uuid128_t base_uuid = {LBS_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_lbs->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_lbs->uuid_type;
    ble_uuid.uuid = LBS_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_lbs->service_handle);
    VERIFY_SUCCESS(err_code);

    // Add Button characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = LBS_UUID_BUTTON_CHAR;
    add_char_params.uuid_type         = p_lbs->uuid_type;
    add_char_params.init_len          = 2;//sizeof(uint8_t);
    add_char_params.max_len           = 2;//sizeof(uint8_t);
    uint8_t value[2]                  = {0xD2,0x04};
    add_char_params.p_init_value      = value; //init first value
    add_char_params.char_props.read   = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access       = SEC_OPEN; //Access open.
    add_char_params.cccd_write_access = SEC_OPEN; 

    //enable notify?

    err_code = characteristic_add(p_lbs->service_handle,
                                  &add_char_params,
                                  &p_lbs->button_char_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add LED characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid             = LBS_UUID_LED_CHAR;
    add_char_params.uuid_type        = p_lbs->uuid_type;
    add_char_params.max_len          = sizeof(uint8_t);
    add_char_params.init_len         = sizeof(uint8_t);
    add_char_params.char_props.read  = 1;
    add_char_params.char_props.write = 1;

    add_char_params.read_access  = SEC_OPEN;  //Access open.
    add_char_params.write_access = SEC_OPEN;

    characteristic_add(p_lbs->service_handle, &add_char_params, &p_lbs->led_char_handles);

    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add Serial Number characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid             = BLE_UUID_CHARACTERISTIC_1;
    add_char_params.uuid_type        = p_lbs->uuid_type;
    add_char_params.max_len          = 4;
    add_char_params.init_len         = 4;
    uint8_t value2[4]                = {0x12,0x34,0x56,0x78};
    add_char_params.p_init_value     = value2;
    add_char_params.char_props.read  = 1;
    add_char_params.char_props.write = 1;

    add_char_params.read_access  = SEC_OPEN;  //Access open.
    add_char_params.write_access = SEC_OPEN;

    return characteristic_add(p_lbs->service_handle, &add_char_params, &p_lbs->char_handles_1);  // adding characteristic handles to the attribute table
}


uint32_t ble_lbs_on_button_change(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t button_state)
{
    ble_gatts_hvx_params_t params;
    uint16_t len = sizeof(button_state);

    memset(&params, 0, sizeof(params));
    params.type   = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_lbs->button_char_handles.value_handle;
    params.p_data = &button_state;
    params.p_len  = &len;

    return sd_ble_gatts_hvx(conn_handle, &params);
}

static int32_t battery_value_local;
// ALREADY_DONE_FOR_YOU: Function to be called when updating characteristic value
uint32_t ble_lbs_batVolt_characteristic_update(uint16_t conn_handle, ble_lbs_t *p_lbs, uint16_t *battery_value)
{
  // implement the notification on connection
  // OUR_JOB: Step 3.E, Update characteristic value
  if (conn_handle != BLE_CONN_HANDLE_INVALID)//housekeeping allow to check if we are in a valid connection
  {
    if(battery_value_local != *battery_value)  //battery changed
    {
        uint16_t               len = 2;//sizeof(battery_value);
        ble_gatts_hvx_params_t hvx_params;          //Handle Value X(notification or indication)
        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_lbs->button_char_handles.value_handle; //which characteristic value we are working on
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;                //notification
        hvx_params.offset = 0;      //characteristic value might be a sequence of many bytes.
        hvx_params.p_len  = &len;   //number of bytes to transmitt
        hvx_params.p_data = (uint8_t*)battery_value;  //data pointer

        return sd_ble_gatts_hvx(conn_handle, &hvx_params);
      }
       battery_value_local = *battery_value;
    }
  return NRF_SUCCESS; //nothing has be send
}

#endif // NRF_MODULE_ENABLED(BLE_LBS)
