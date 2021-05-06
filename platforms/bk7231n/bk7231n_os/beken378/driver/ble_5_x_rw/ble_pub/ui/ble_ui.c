#include "ble_ui.h"
#include "app_ble.h"

ble_event_cb_t ble_event_cb = NULL;
bk_ble_read_cb_t bk_ble_read_cb = NULL;
bk_ble_write_cb_t bk_ble_write_cb = NULL;
ble_recv_adv_cb_t ble_recv_adv_cb = NULL;
extern struct app_env_tag app_ble_ctx;

void ble_set_event_cb(ble_event_cb_t func)
{
    ble_event_cb = func;
}

void ble_event_cb_handler(ble_event_t event, void *param)
{
	ble_event_cb_t func = ble_event_cb;
	
	if(func != NULL)
	{
		func(event,param);
	}
}

void ble_set_write_cb(bk_ble_write_cb_t func)
{
    bk_ble_write_cb = func;
}

void ble_write_cb_handler(write_req_t *write_req)
{
	bk_ble_write_cb_t func = bk_ble_write_cb;
	
	if(func != NULL)
	{
		func(write_req);
	}
}

void ble_set_read_cb(bk_ble_read_cb_t func)
{
    bk_ble_read_cb = func;
}

int ble_read_cb_handler(read_req_t *read_req)
{
	bk_ble_read_cb_t func = bk_ble_read_cb;
	
	if(func != NULL)
	{
		return func(read_req);
	}
	return 0;
}

void ble_set_recv_adv_cb(ble_recv_adv_cb_t func)
{
    ble_recv_adv_cb = func;
}

void ble_recv_adv_cb_handler(recv_adv_t *recv_adv)
{
	ble_recv_adv_cb_t func = ble_recv_adv_cb;
	
	if(func != NULL)
	{
		func(recv_adv);
	}
}

unsigned char ble_appm_get_dev_name(unsigned char* name)
{
    // copy name to provided pointer
    memcpy(name, app_ble_ctx.dev_name, app_ble_ctx.dev_name_len);
    // return name length
    return app_ble_ctx.dev_name_len;
}

unsigned char ble_appm_set_dev_name(unsigned char len,unsigned char* name)
{
    // copy name to provided pointer
    if(len < APP_DEVICE_NAME_MAX_LEN)
    {   
        app_ble_ctx.dev_name_len = len;
        memcpy(app_ble_ctx.dev_name, name, len);
        // return name length
        return app_ble_ctx.dev_name_len;
    }
	
    return 0;   
}



