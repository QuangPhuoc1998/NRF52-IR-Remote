#include "main.h"

/**@brief Function for handling the Write event.
 *
 * @param[in] p_cus      Custom Service structure.
 * @param[in] p_ble_evt  Event received from the BLE stack.
 */

uint8_t g_ubValueIndex = 0;

static void on_write(ble_cus_t * p_cus, ble_evt_t const * p_ble_evt)
{
	Mid_LedControlSet(LED_TYPE_BLE_RECEIVE);
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if(p_evt_write->handle == g_ubValueHandle[CUS_UUID_RTC_SET_INDEX])
    {
    	NRF_LOG_INFO("*--- RTC set command ---*/");
		NRF_LOG_INFO("Data len: %d", p_evt_write->len);
		Mid_DecodeCommand(CUS_UUID_RTC_SET_INDEX, (uint8_t *)p_evt_write->data, p_evt_write->len);

    }
    else if(p_evt_write->handle == g_ubValueHandle[CUS_UUID_MOT_SENS_INDEX])
    {
    	NRF_LOG_INFO("*--- Motion sens command ---*/");
    	NRF_LOG_INFO("Data len: %d", p_evt_write->len);

    	for(uint8_t i = 0 ; i < p_evt_write->len ; i++)
    	{
    		NRF_LOG_INFO("0x%X - %c", p_evt_write->data[i], p_evt_write->data[i]);
    	}
    	Mid_DecodeCommand(CUS_UUID_MOT_SENS_INDEX, (uint8_t *)p_evt_write->data, p_evt_write->len);
    }
    else if(p_evt_write->handle == g_ubValueHandle[CUS_UUID_MOT_TOUT_INDEX])
	{
    	NRF_LOG_INFO("*--- Motion timeout command ---*/");
    	NRF_LOG_INFO("Data len: %d", p_evt_write->len);

    	for(uint8_t i = 0 ; i < p_evt_write->len ; i++)
    	{
    		NRF_LOG_INFO("0x%X - %c", p_evt_write->data[i], p_evt_write->data[i]);
    	}
    	Mid_DecodeCommand(CUS_UUID_MOT_TOUT_INDEX, (uint8_t *)p_evt_write->data, p_evt_write->len);
	}
    else if(p_evt_write->handle == g_ubValueHandle[CUS_UUID_SCHEDULE_INDEX])
	{
    	NRF_LOG_INFO("*--- Schedule command ---*/");
    	NRF_LOG_INFO("Data len: %d", p_evt_write->len);

    	for(uint8_t i = 0 ; i < p_evt_write->len ; i++)
    	{
    		NRF_LOG_INFO("0x%X - %c", p_evt_write->data[i], p_evt_write->data[i]);
    	}
    	Mid_DecodeCommand(CUS_UUID_SCHEDULE_INDEX, (uint8_t *)p_evt_write->data, p_evt_write->len);
	}
//    else if(p_evt_write->handle == g_ubValueHandle[CUS_UUID_BATT_VOL_INDEX])
//	{
//    	print_debug_message("IR learn command", 0, 0, 1);
//	}
    else if(p_evt_write->handle == g_ubValueHandle[CUS_UUID_IR_LEARN_INDEX])
	{
    	NRF_LOG_INFO("*--- IR learn command ---*/");
    	NRF_LOG_INFO("Len: %d",p_evt_write->len);
    	NRF_LOG_INFO("Raw: 0x%X 0x%X",p_evt_write->data[0], p_evt_write->data[1]);
    	NRF_LOG_INFO("ID: %d",ConvertID((uint8_t *)p_evt_write->data, p_evt_write->len));
    	App_ControlStartLearnIR(ConvertID((uint8_t *)p_evt_write->data, p_evt_write->len));
	}
    else if(p_evt_write->handle == g_ubValueHandle[CUS_UUID_IR_EMMIT_INDEX])
	{
    	NRF_LOG_INFO("/*--- IR emmit command ---*/");

    	NRF_LOG_INFO("Len: %d",p_evt_write->len);
    	NRF_LOG_INFO("Raw: 0x%X 0x%X",p_evt_write->data[0], p_evt_write->data[1]);
    	NRF_LOG_INFO("ID: %d",ConvertID((uint8_t *)p_evt_write->data, p_evt_write->len));
    	app_sched_event_put(p_evt_write->data, p_evt_write->len, App_ControlStartEmitIR);
	}
    else if(p_evt_write->handle == g_ubValueHandle[CUS_UUID_IR_ERASE_INDEX])
	{
    	NRF_LOG_INFO("/*--- IR erase command ---*/");

    	NRF_LOG_INFO("Len: %d",p_evt_write->len);
    	NRF_LOG_INFO("Raw: 0x%X 0x%X",p_evt_write->data[0], p_evt_write->data[1]);
    	NRF_LOG_INFO("ID: %d",ConvertID((uint8_t *)p_evt_write->data, p_evt_write->len));
    	App_ControlEraseIR(ConvertID((uint8_t *)p_evt_write->data, p_evt_write->len));
	}
}


void ble_cus_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_cus_t * p_cus = (ble_cus_t *)p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GATTS_EVT_WRITE:
            on_write(p_cus, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}

uint32_t ble_cus_init(ble_cus_t * p_cus, const ble_cus_init_t * p_cus_init)
{
    uint32_t              err_code;
    ble_uuid_t            ble_uuid;
    ble_add_char_params_t add_char_params;

    // Initialize service structure.
    p_cus->led_write_handler = p_cus_init->led_write_handler;

    // Add service.
    ble_uuid128_t base_uuid = {CUS_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_cus->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_cus->uuid_type;
    ble_uuid.uuid = CUS_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_cus->service_handle);
    VERIFY_SUCCESS(err_code);

    // Add RTC characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = CUS_UUID_RTC_SET;
    add_char_params.uuid_type         = p_cus->uuid_type;
    add_char_params.init_len          = sizeof(uint8_t);
    add_char_params.max_len           = sizeof(uint8_t)*64;
    add_char_params.char_props.read  = 1;
    add_char_params.char_props.write = 1;

    add_char_params.read_access  = SEC_OPEN;
    add_char_params.write_access = SEC_OPEN;

    err_code = characteristic_add(p_cus->service_handle,
                                  &add_char_params,
                                  &p_cus->led_char_handles);

    g_ubValueHandle[g_ubValueIndex++] = p_cus->led_char_handles.value_handle;

    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add Motion sensitivity characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = CUS_UUID_MOT_SENS;
    add_char_params.uuid_type         = p_cus->uuid_type;
    add_char_params.init_len          = sizeof(uint8_t);
    add_char_params.max_len           = sizeof(uint8_t)*64;
    add_char_params.char_props.read  = 1;
    add_char_params.char_props.write = 1;

    add_char_params.read_access  = SEC_OPEN;
    add_char_params.write_access = SEC_OPEN;

    err_code = characteristic_add(p_cus->service_handle,
                                  &add_char_params,
                                  &p_cus->led_char_handles);

    g_ubValueHandle[g_ubValueIndex++] = p_cus->led_char_handles.value_handle;

    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add Motion timeout characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = CUS_UUID_MOT_TOUT;
    add_char_params.uuid_type         = p_cus->uuid_type;
    add_char_params.init_len          = sizeof(uint8_t);
    add_char_params.max_len           = sizeof(uint8_t)*64;
    add_char_params.char_props.read  = 1;
    add_char_params.char_props.write = 1;

    add_char_params.read_access  = SEC_OPEN;
    add_char_params.write_access = SEC_OPEN;

    err_code = characteristic_add(p_cus->service_handle,
                                  &add_char_params,
                                  &p_cus->led_char_handles);

    g_ubValueHandle[g_ubValueIndex++] = p_cus->led_char_handles.value_handle;

    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add schedule characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = CUS_UUID_SCHEDULE;
    add_char_params.uuid_type         = p_cus->uuid_type;
    add_char_params.init_len          = sizeof(uint8_t);
    add_char_params.max_len           = sizeof(uint8_t)*64;
    add_char_params.char_props.read  = 1;
    add_char_params.char_props.write = 1;

    add_char_params.read_access  = SEC_OPEN;
    add_char_params.write_access = SEC_OPEN;

    err_code = characteristic_add(p_cus->service_handle,
                                  &add_char_params,
                                  &p_cus->led_char_handles);

    g_ubValueHandle[g_ubValueIndex++] = p_cus->led_char_handles.value_handle;

    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add battery voltage characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = CUS_UUID_BATT_VOL;
    add_char_params.uuid_type         = p_cus->uuid_type;
    add_char_params.init_len          = sizeof(uint8_t);
    add_char_params.max_len           = sizeof(uint8_t);
    add_char_params.char_props.read   = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access       = SEC_OPEN;
    add_char_params.cccd_write_access = SEC_OPEN;

    err_code = characteristic_add(p_cus->service_handle,
                                  &add_char_params,
                                  &p_cus->led_char_handles);

    g_ubValueHandle[g_ubValueIndex++] = p_cus->led_char_handles.value_handle;

    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add IR learning characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid              = CUS_UUID_IR_LEARN;
    add_char_params.uuid_type         = p_cus->uuid_type;
    add_char_params.init_len          = sizeof(uint8_t);
    add_char_params.max_len           = sizeof(uint8_t)*64;
    add_char_params.char_props.read   = 1;
    add_char_params.char_props.write = 1;

    add_char_params.read_access = SEC_OPEN;
    add_char_params.write_access = SEC_OPEN;

    err_code = characteristic_add(p_cus->service_handle, &add_char_params, &p_cus->led_char_handles);

    g_ubValueHandle[g_ubValueIndex++] = p_cus->led_char_handles.value_handle;

    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add IR emitting characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid             = CUS_UUID_IR_EMMIT;
    add_char_params.uuid_type        = p_cus->uuid_type;
    add_char_params.init_len         = sizeof(uint8_t);
    add_char_params.max_len          = sizeof(uint8_t)*64;
    add_char_params.char_props.read  = 1;
    add_char_params.char_props.write = 1;

    add_char_params.read_access  = SEC_OPEN;
    add_char_params.write_access = SEC_OPEN;

    err_code = characteristic_add(p_cus->service_handle, &add_char_params, &p_cus->led_char_handles);

    g_ubValueHandle[g_ubValueIndex++] = p_cus->led_char_handles.value_handle;

    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add IR erase characteristic.
    memset(&add_char_params, 0, sizeof(add_char_params));
    add_char_params.uuid             = CUS_UUID_IR_ERASE;
    add_char_params.uuid_type        = p_cus->uuid_type;
    add_char_params.init_len         = sizeof(uint8_t);
    add_char_params.max_len          = sizeof(uint8_t)*64;
    add_char_params.char_props.read  = 1;
    add_char_params.char_props.write = 1;

    add_char_params.read_access  = SEC_OPEN;
    add_char_params.write_access = SEC_OPEN;

    err_code = characteristic_add(p_cus->service_handle, &add_char_params, &p_cus->led_char_handles);

    g_ubValueHandle[g_ubValueIndex++] = p_cus->led_char_handles.value_handle;

    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add IR learn done characteristic.
	memset(&add_char_params, 0, sizeof(add_char_params));
	add_char_params.uuid             = CUS_UUID_FEED_BACK;
	add_char_params.uuid_type        = p_cus->uuid_type;
	add_char_params.init_len         = sizeof(uint8_t);
	add_char_params.max_len          = sizeof(uint8_t);
    add_char_params.char_props.read   = 1;
    add_char_params.char_props.notify = 1;

    add_char_params.read_access       = SEC_OPEN;
    add_char_params.cccd_write_access = SEC_OPEN;

	err_code = characteristic_add(p_cus->service_handle, &add_char_params, &p_cus->button_char_handles);

	g_ubValueHandle[g_ubValueIndex++] = p_cus->button_char_handles.value_handle;

    return err_code;
}


uint32_t ble_cus_on_button_change(uint16_t conn_handle, ble_cus_t * p_cus, uint8_t button_state)
{
    ble_gatts_hvx_params_t params;
    uint16_t len = sizeof(button_state);

    memset(&params, 0, sizeof(params));
    params.type   = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_cus->button_char_handles.value_handle;
    params.p_data = &button_state;
    params.p_len  = &len;

    return sd_ble_gatts_hvx(conn_handle, &params);
}

uint32_t ble_cus_noti(uint16_t conn_handle, uint8_t ubValueHandle, uint8_t ubData)
{
    ble_gatts_hvx_params_t params;
    uint16_t len = sizeof(ubData);

    memset(&params, 0, sizeof(params));
    params.type   = BLE_GATT_HVX_NOTIFICATION;
    params.handle = ubValueHandle;
    params.p_data = &ubData;
    params.p_len  = &len;

    return sd_ble_gatts_hvx(conn_handle, &params);
}
