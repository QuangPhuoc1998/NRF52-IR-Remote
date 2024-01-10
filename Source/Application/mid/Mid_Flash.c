#include "main.h"

//#define FLASH_SHOW_DEBUG

void print_flash_info(nrf_fstorage_t * p_fstorage);
static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt);
static uint32_t nrf5_flash_end_addr_get();
void wait_for_flash_ready(nrf_fstorage_t const * p_fstorage);
static void power_manage(void);

NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage) =
{
    .evt_handler = fstorage_evt_handler,
    .start_addr = START_ADDRESS_FSTORAGE,
    .end_addr   = END_ADDRESS_FSTORAGE,
};

void Mid_FlashInit(void)
{
	ret_code_t rc;
	nrf_fstorage_api_t * p_fs_api;

	p_fs_api = &nrf_fstorage_sd;

	rc = nrf_fstorage_init(&fstorage, p_fs_api, NULL);
	APP_ERROR_CHECK(rc);

//	Mid_FlashErase(IR_DATA_START_ADDRESS, 1);

//	print_flash_info(&fstorage);

//	uint32_t ulNumber = 0x11223344;
//	Mid_FlashErase(IR_DATA_START_ADDRESS + 4096, 1);
//	Mid_FlashWrite(CONVERT_IR_DATA_TO_ADDRESS(0), &ulNumber, sizeof(ulNumber));
//
//	uint8_t aubReadData[32] = {0};
//	Mid_FlashRead(aubReadData, IR_DATA_START_ADDRESS+4, sizeof(ulNumber));
//
//	for(uint8_t i = 0 ; i < sizeof(ulNumber) ; i++)
//	{
//		NRF_LOG_INFO("Value: 0x%X", aubReadData[i]);
//	}

	Mid_FlashRead(t_IRDataCommom, IR_DATA_START_ADDRESS, SIZE_OF_IR_DATA_VAR);
#ifdef FLASH_SHOW_DEBUG
	NRF_LOG_INFO("IR Code");
	for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
	{
		NRF_LOG_INFO("IR %d : %d", t_IRDataCommom[i].uwID, t_IRDataCommom[i].ulStatus);
	}
#endif //FLASH_SHOW_DEBUG
}

bool Mid_FlashWrite(uint32_t ulAddress, void const * p_src, uint32_t ulLen)
{
	ret_code_t ret;
	ret = nrf_fstorage_write(&fstorage, ulAddress, p_src, ulLen, NULL);
	APP_ERROR_CHECK(ret);
	wait_for_flash_ready(&fstorage);
	if (ret != NRF_SUCCESS)
	{
		NRF_LOG_INFO("nrf_fstorage_write() returned: %s", nrf_strerror_get(ret));
		return false;
	}
	return true;
}

bool Mid_FlashRead(void * pData, uint32_t ulAddress, uint32_t ulLen)
{
	if(ulLen > 1024)
	{
		return false;
	}

	ret_code_t ret;
	uint8_t data[1024] = {0};
	ret = nrf_fstorage_read(&fstorage, ulAddress, data, ulLen);
	if (ret != NRF_SUCCESS)
	{
		NRF_LOG_INFO("nrf_fstorage_read() returned: %s", nrf_strerror_get(ret));
		return false;
	}
	myMemCpy(pData, data, ulLen);
	return true;
}

bool Mid_FlashErase(uint32_t ulAddress, uint32_t ulLen)
{
	ret_code_t ret;
	ret = nrf_fstorage_erase(&fstorage, ulAddress, ulLen, NULL);
    if (ret != NRF_SUCCESS)
    {
        NRF_LOG_INFO("nrf_fstorage_erase() returned: %s", nrf_strerror_get(ret));
        return false;
    }
    return true;
}

void print_flash_info(nrf_fstorage_t * p_fstorage)
{
#ifdef FLASH_SHOW_DEBUG
    NRF_LOG_INFO("========| flash info |========");
    NRF_LOG_INFO("erase unit: \t%d bytes",      p_fstorage->p_flash_info->erase_unit);
    NRF_LOG_INFO("program unit: \t%d bytes",    p_fstorage->p_flash_info->program_unit);
    NRF_LOG_INFO("==============================");

    NRF_LOG_INFO("IR Data - 0x%X - %d - %d", IR_DATA_START_ADDRESS, SIZE_OF_IR_DATA_TYPE, SIZE_OF_IR_DATA_VAR)
    for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
    {
    	NRF_LOG_INFO("Address for IR data %d: 0x%X", i, CONVERT_IR_DATA_TO_ADDRESS(i));
    }

    NRF_LOG_INFO("IR Trig - 0x%X - %d - %d", IR_TRIG_START_ADDRESS, SIZE_OF_IR_TRIG_TYPE, SIZE_OF_IR_TRIG_VAR)
    for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
    {
    	NRF_LOG_INFO("Address for IR Trig %d: 0x%X", i, CONVERT_IR_TRIG_TO_ADDRESS(i));
    }

    NRF_LOG_INFO("IR Timeout - 0x%X - %d - %d", IR_TIMEOUT_START_ADDRESS, SIZE_OF_IR_TIMEOUT_TYPE, SIZE_OF_IR_TIMEOUT_VAR)
    for(uint8_t i = 0 ; i < MAX_IR_CODE ; i++)
    {
    	NRF_LOG_INFO("Address for IR Timeout %d: 0x%X", i, CONVERT_IR_TIMEOUT_TO_ADDRESS(i));
    }

#endif
}

static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt)
{
    if (p_evt->result != NRF_SUCCESS)
    {
        NRF_LOG_INFO("--> Event received: ERROR while executing an fstorage operation.");
        return;
    }

    switch (p_evt->id)
    {
        case NRF_FSTORAGE_EVT_WRITE_RESULT:
        {
            NRF_LOG_INFO("--> Event received: wrote %d bytes at address 0x%x.",
                         p_evt->len, p_evt->addr);
        } break;

        case NRF_FSTORAGE_EVT_ERASE_RESULT:
        {
            NRF_LOG_INFO("--> Event received: erased %d page from address 0x%x.",
                         p_evt->len, p_evt->addr);
        } break;

        default:
            break;
    }
}

static uint32_t nrf5_flash_end_addr_get()
{
    uint32_t const bootloader_addr = BOOTLOADER_ADDRESS;
    uint32_t const page_sz         = NRF_FICR->CODEPAGESIZE;
    uint32_t const code_sz         = NRF_FICR->CODESIZE;

    return (bootloader_addr != 0xFFFFFFFF ?
            bootloader_addr : (code_sz * page_sz));
}

void wait_for_flash_ready(nrf_fstorage_t const * p_fstorage)
{
    /* While fstorage is busy, sleep and wait for an event. */
    while (nrf_fstorage_is_busy(p_fstorage))
    {
        power_manage();
    }
}

static void power_manage(void)
{
#ifdef SOFTDEVICE_PRESENT
    (void) sd_app_evt_wait();
#else
    __WFE();
#endif
}

