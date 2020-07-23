
#include <stdint.h>
#include <string.h>
#include "nrf_gpio.h"
#include "our_service.h"
#include "ble_srv_common.h"
#include "app_error.h"

// ALREADY_DONE_FOR_YOU: Declaration of a function that will take care of some housekeeping of ble connections related to our service and characteristic
void ble_our_service_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
  	ble_os_t * p_our_service =(ble_os_t *) p_context;  
        // OUR_JOB: Step 3.D Implement switch case handling BLE events related to our service. 
	switch (p_ble_evt->header.evt_id)
        {
            case BLE_GAP_EVT_CONNECTED: //update the connection handle stored in the service structure, with the current connection handle as provided by the BLE stack
                p_our_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
                break;
            case BLE_GAP_EVT_DISCONNECTED://Connection handle set back to invalid
                p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;
                break;
            default:
                // No implementation needed.
                break;
        }
	
}

/**@brief Function for adding our new characterstic to "Our service" that we initiated in the previous tutorial. 
 *
 * @param[in]   p_our_service        Our Service structure.
 *
 */
static uint32_t our_char_add(ble_os_t * p_our_service)
{
    // OUR_JOB: Step 2.A, Add a custom characteristic UUID
    uint32_t            err_code;
    ble_uuid_t          char_uuid;
    ble_uuid128_t       base_uuid = BLE_UUID_OUR_BASE_UUID; //use the same base UUID as the service
    char_uuid.uuid      = BLE_UUID_OUR_CHARACTERISTC_UUID;  //different 16-bit UUID for the characteristic
    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
    APP_ERROR_CHECK(err_code);

    
    // OUR_JOB: Step 2.F Add read/write properties to our characteristic
    /* Characteristic Metadata: 
     * This is a structure holding the value properties of the characteristic value.
     * It also holds metadata of the CCCD and possibly other kinds of descriptors.
     */
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 1;  //Add read/write properties to our characteristic value      
    char_md.char_props.write = 1;


    // OUR_JOB: Step 3.A, Configuring CCCD - Client Characteristic Configuration Descriptor metadata and add to char_md structure
    ble_gatts_attr_md_t cccd_md;                        //metadata structure for the CCCD to hold our configuration.
    memset(&cccd_md, 0, sizeof(cccd_md));               
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm); //permissions defenition
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK; //store descriptor in the SoftDevice/STACK controlled part of memory
    char_md.p_cccd_md           = &cccd_md;             //store the CCCD metadata structure in our characteristic metadata structure
    char_md.char_props.notify   = 1;                    //Enable notification
    
    // OUR_JOB: Step 2.B, Configure the attribute metadata
    /* Attribute MetaData:
     * This is a structure holding permissions and authorization levels required
     * by characteristic value attributes.It also holds information on whether 
     * or not the characteristic value is of variable length and where in memory it is stored.
     */
    ble_gatts_attr_md_t attr_md;                //Structur allow to define the permissions
    memset(&attr_md, 0, sizeof(attr_md));       //store the attribut in the SoftDevice (aka the stack)
    attr_md.vloc        = BLE_GATTS_VLOC_STACK; //Sack part of the memory (else USER)
    
    // OUR_JOB: Step 2.G, Set read/write permissions (security levels) to our characteristic
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    //BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    

    
    // OUR_JOB: Step 2.C, Configure the characteristic value attribute
    /* The Characteristic Value Attribute:
     * This structure holds the actual value of the characteristic (like the temperature value).
     * It also holds the maximum length of the value (it might e.g. be four bytes long) and it's UUID.
     */
    ble_gatts_attr_t    attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value)); //memory allocation
    attr_char_value.p_uuid      = &char_uuid;
    attr_char_value.p_attr_md   = &attr_md;
    
    // OUR_JOB: Step 2.H, Set characteristic length in number of bytes
    attr_char_value.max_len     = 4;  //Max allow length
    attr_char_value.init_len    = 4;  //initial length
    uint8_t value[4]            = {0x12,0x34,0x56,0x78};
    attr_char_value.p_value     = value;

    // OUR_JOB: Step 2.E, Add our new characteristic to the service
    err_code = sd_ble_gatts_characteristic_add(p_our_service->service_handle, //Give to the SoftDevice what service the characteristic belongs to
                                       &char_md,          //Characteristic Metadata
                                       &attr_char_value,  //Characteristic Value Attributes
                                       &p_our_service->char_handles); //stores the handle values of our characteristic into our p_our_service structure
    APP_ERROR_CHECK(err_code);

    return NRF_SUCCESS;
}


/**@brief Function for initiating our new service.
 *
 * @param[in]   p_our_service        Our Service structure.
 *
 */
void our_service_init(ble_os_t * p_our_service)
{
    uint32_t   err_code; // Variable to hold return codes from library and softdevice functions

    // FROM_SERVICE_TUTORIAL: Declare 16-bit service and 128-bit base UUIDs and add them to the BLE stack
    ble_uuid_t        service_uuid;
    ble_uuid128_t     base_uuid = BLE_UUID_OUR_BASE_UUID;
    service_uuid.uuid = BLE_UUID_OUR_SERVICE_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
    APP_ERROR_CHECK(err_code);    
    
    // OUR_JOB: Step 3.B, Set our service connection handle to default value. I.e. an invalid handle since we are not yet in a connection.
    p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;

    // FROM_SERVICE_TUTORIAL: Add our service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                            &service_uuid,
                            &p_our_service->service_handle);
    
    APP_ERROR_CHECK(err_code);
    
    // OUR_JOB: Call the function our_char_add() to add our new characteristic to the service. 
    our_char_add(p_our_service);
}

static int32_t temperature_value_local;
// ALREADY_DONE_FOR_YOU: Function to be called when updating characteristic value
void our_temperature_characteristic_update(ble_os_t *p_our_service, int32_t *temperature_value)
{
    // implement the notification on connection
    // OUR_JOB: Step 3.E, Update characteristic value
    if (p_our_service->conn_handle != BLE_CONN_HANDLE_INVALID)//housekeeping allow to check if we are in a valid connection
    {
      if(temperature_value_local != *temperature_value)  //temperature changed
      {
        uint16_t               len = 4;
        ble_gatts_hvx_params_t hvx_params;          //Handle Value X(notification or indication)
        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_our_service->char_handles.value_handle; //which characteristic value we are working on
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;                //notification
        hvx_params.offset = 0;      //characteristic value might be a sequence of many bytes.
        hvx_params.p_len  = &len;   //number of bytes to transmitt
        hvx_params.p_data = (uint8_t*)temperature_value;  //data pointer

        sd_ble_gatts_hvx(p_our_service->conn_handle, &hvx_params);
      }
       temperature_value_local = *temperature_value;
    }
}
