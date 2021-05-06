/**
 * @file tuya_os_adapt_ota.c
 * @brief ota底层操作接口
 * 
 * @copyright Copyright (c) {2018-2020} 涂鸦科技 www.tuya.com
 * 
 */
#include "tuya_os_adapt_ota.h"
#include "tuya_os_adapter_error_code.h"
#include "tuya_os_adapt_system.h"
#include "tuya_os_adapt_memory.h"
#include "tuya_os_adapt_output.h"
#include "tuya_os_adapt_storge.h"

/***********************************************************
*************************micro define***********************
***********************************************************/

#define UG_PKG_HEAD     0x55aa55aa
#define UG_PKG_TAIL     0xaa55aa55
#define UG_START_ADDR   0x12A000   //664k  
#define RT_IMG_WR_UNIT  512
#define BUF_SIZE        4096
#define OTA_MAX_BIN_SIZE (664 * 1024)

typedef enum {
    UGS_RECV_HEADER = 0,
    UGS_RECV_IMG_DATA,
    UGS_FINISH
}UG_STAT_E;
    
typedef struct
{
    unsigned int header_flag;        //0xaa55aa55
    char sw_version[12];             //sofrware version
    unsigned int bin_len;
    unsigned int bin_sum;
    unsigned int head_sum;           //header_flag + sw_version + bin_len + bin_sum
    unsigned int tail_flag;          //0x55aa55aa
}UPDATE_FILE_HDR_S;

typedef struct {
    UPDATE_FILE_HDR_S file_header;
    unsigned int flash_addr;
    unsigned int start_addr;
    unsigned int recv_data_cnt;
    UG_STAT_E stat;
}UG_PROC_S;

/***********************************************************
*************************variable define********************
***********************************************************/
static const TUYA_OS_OTA_INTF m_tuya_os_ota_intfs = {
    .start      = tuya_os_adapt_ota_start_inform, 
    .process    = tuya_os_adapt_ota_data_process,
    .end        = tuya_os_adapt_ota_end_inform,
};

static UG_PROC_S *ug_proc = NULL;
SET_OTA_FINISH_NOTIFY _ota_finish_notify = NULL;
unsigned char *frist_block_databuf = NULL;
unsigned char first_block = 1;

/***********************************************************
*************************function define********************
***********************************************************/
/**
 * @brief 升级开始通知函数
 * 
 * @param[in] file_size 升级固件大小
 * 
 * @retval  =0      成功
 * @retval  <0      错误码
 */
int tuya_os_adapt_ota_start_inform(unsigned int file_size, OTA_TYPE type)
{
    if(file_size == 0) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    if(ug_proc == NULL) {
        ug_proc = tuya_os_adapt_system_malloc(sizeof(UG_PROC_S));
        if(NULL == ug_proc) {
            return OPRT_OS_ADAPTER_MALLOC_FAILED;
        }
    }

    memset(ug_proc,0,sizeof(UG_PROC_S));
    first_block = 1;
    
    return OPRT_OS_ADAPTER_OK;
}

/**
 * @brief ota数据包处理
 * 
 * @param[in] total_len ota升级包总大小
 * @param[in] offset 当前data在升级包中的偏移
 * @param[in] data ota数据buffer指针
 * @param[in] len ota数据buffer长度
 * @param[out] remain_len 内部已经下发但该函数还未处理的数据长度
 * @param[in] pri_data 保留参数
 *
 * @retval  =0      成功
 * @retval  <0      错误码
 */
int tuya_os_adapt_ota_data_process(const unsigned int total_len, const unsigned int offset,
                              const unsigned char* data, const unsigned int len, unsigned int* remain_len, void* pri_data)
{
    unsigned int sum_tmp = 0, i = 0;
    unsigned int write_len = 0;
    unsigned char *buf = NULL;

    
    if(ug_proc == NULL) {
        LOG_ERR("ota don't start or start err,process error!\r\n");
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    switch(ug_proc->stat) {
        case UGS_RECV_HEADER: {
            if(len < sizeof(UPDATE_FILE_HDR_S)) {
                *remain_len = len;
                break;
            }
            
            ug_proc->file_header.tail_flag = (data[28]<<24)|(data[29]<<16)|(data[30]<<8)|data[31];
            ug_proc->file_header.head_sum = (data[24]<<24)|(data[25]<<16)|(data[26]<<8)|data[27];
            ug_proc->file_header.bin_sum = (data[20]<<24)|(data[21]<<16)|(data[22]<<8)|data[23];
            ug_proc->file_header.bin_len = (data[16]<<24)|(data[17]<<16)|(data[18]<<8)|data[19];
            for(i = 0; i < 12; i++) {
                ug_proc->file_header.sw_version[i] = data[4 + i];
            }
            
            ug_proc->file_header.header_flag = (data[0]<<24)|(data[1]<<16)|(data[2]<<8)|data[3];
            
            for(i = 0;i < sizeof(UPDATE_FILE_HDR_S) - 8;i++) {
                sum_tmp += data[i];
            }
            
            LOG_NOTICE("header_flag(0x%x) tail_flag(0x%x) head_sum(0x%x-0x%x) bin_sum(0x%x)\r\n",ug_proc->file_header.header_flag,ug_proc->file_header.tail_flag,ug_proc->file_header.head_sum,sum_tmp,ug_proc->file_header.bin_sum);

            if((ug_proc->file_header.header_flag !=  UG_PKG_HEAD) || (ug_proc->file_header.tail_flag !=  UG_PKG_TAIL) || (ug_proc->file_header.head_sum != sum_tmp )) {
                memset(&ug_proc->file_header, 0, sizeof(UPDATE_FILE_HDR_S));
                LOG_ERR("bin_file data header err: header_flag(0x%x) tail_flag(0x%x) bin_sum(0x%x) get_sum(0x%x)\r\n",ug_proc->file_header.header_flag,ug_proc->file_header.tail_flag,ug_proc->file_header.head_sum,sum_tmp);
                return OPRT_OS_ADAPTER_OTA_START_INFORM_FAILED;
            }
            
            if(ug_proc->file_header.bin_len >= OTA_MAX_BIN_SIZE) { //ug文件最大为664K
                memset(&ug_proc->file_header, 0, sizeof(UPDATE_FILE_HDR_S));
                LOG_ERR("bin_file too large.... %d\r\n", ug_proc->file_header.bin_len);
                return OPRT_OS_ADAPTER_OTA_PKT_SIZE_FAILED;
            }
            
            LOG_NOTICE("sw_ver:%s\r\n", ug_proc->file_header.sw_version);
            LOG_NOTICE("get right bin_file_header!!!\r\n");
            ug_proc->start_addr = UG_START_ADDR;
            ug_proc->flash_addr = ug_proc->start_addr;
            ug_proc->stat = UGS_RECV_IMG_DATA;
            ug_proc->recv_data_cnt = 0;
            *remain_len = len - sizeof(UPDATE_FILE_HDR_S);
            
            tuya_os_adapt_flash_set_protect(FALSE);
            tuya_os_adapt_flash_erase(ug_proc->start_addr,ug_proc->file_header.bin_len);
            tuya_os_adapt_flash_set_protect(TRUE);
            
        } 
        break;
        
        case UGS_RECV_IMG_DATA: {    //dont have set lock for flash! 
            *remain_len = len;
            if((len < RT_IMG_WR_UNIT) && (ug_proc->recv_data_cnt <= (ug_proc->file_header.bin_len - RT_IMG_WR_UNIT))) {
                break;
            }
            
            write_len = len;
            
            while(write_len >= RT_IMG_WR_UNIT) {
                if(first_block) {
                    if(NULL == frist_block_databuf){
                        frist_block_databuf = tuya_os_adapt_system_malloc(RT_IMG_WR_UNIT);
                        if(NULL == frist_block_databuf) {
                            return OPRT_OS_ADAPTER_OTA_PROCESS_FAILED;
                        }
                    }
                    
                    memcpy(frist_block_databuf, &data[len - write_len], RT_IMG_WR_UNIT);
                    
                    
                    buf = tuya_os_adapt_system_malloc(RT_IMG_WR_UNIT);
                    if(NULL == buf ) {
                        return OPRT_OS_ADAPTER_OTA_PROCESS_FAILED;
                    }

                    memset(buf, 0xFF , RT_IMG_WR_UNIT);  // make dummy data 
                    tuya_os_adapt_flash_set_protect(FALSE);
                    if(tuya_os_adapt_flash_write(ug_proc->flash_addr, buf, RT_IMG_WR_UNIT)) {
                        tuya_os_adapt_flash_set_protect(TRUE);
                        LOG_ERR("Write sector failed\r\n");
                        if(buf){
                            tuya_os_adapt_system_free(buf);
                            buf = NULL;
                        }
                        return OPRT_OS_ADAPTER_OTA_PROCESS_FAILED;
                    }
                    tuya_os_adapt_flash_set_protect(TRUE);
                    first_block = 0;    
                } else {
                    tuya_os_adapt_flash_set_protect(FALSE);
                    if(tuya_os_adapt_flash_write(ug_proc->flash_addr, &data[len - write_len], RT_IMG_WR_UNIT)) {
                        tuya_os_adapt_flash_set_protect(TRUE);
                        LOG_ERR("Write sector failed\r\n");
                        if(buf){
                            tuya_os_adapt_system_free(buf);
                            buf = NULL;
                        }
                        return OPRT_OS_ADAPTER_OTA_PROCESS_FAILED;
                    }
                    tuya_os_adapt_flash_set_protect(TRUE);
                }
                ug_proc->flash_addr += RT_IMG_WR_UNIT;
                ug_proc->recv_data_cnt += RT_IMG_WR_UNIT;
                write_len -= RT_IMG_WR_UNIT; 
                *remain_len = write_len;
            }
            
            if((ug_proc->recv_data_cnt > (ug_proc->file_header.bin_len - RT_IMG_WR_UNIT)) \
                && (write_len >= (ug_proc->file_header.bin_len - ug_proc->recv_data_cnt))) {    //last 512 (write directly when get data )
                
                tuya_os_adapt_flash_set_protect(FALSE);
                
                if(tuya_os_adapt_flash_write(ug_proc->flash_addr, &data[len - write_len], write_len)) {
                    tuya_os_adapt_flash_set_protect(TRUE);
                    LOG_ERR("Write sector failed\r\n");
                    if(buf){
                        tuya_os_adapt_system_free(buf);
                        buf = NULL;
                    }
                    return OPRT_OS_ADAPTER_OTA_PROCESS_FAILED;
                }
                
                tuya_os_adapt_flash_set_protect(TRUE);
                
                ug_proc->flash_addr += write_len;
                ug_proc->recv_data_cnt += write_len;
                write_len = 0;
                *remain_len = 0;
            }            
            if(ug_proc->recv_data_cnt >= ug_proc->file_header.bin_len) {
                ug_proc->stat = UGS_FINISH;
                if(buf){
                    tuya_os_adapt_system_free(buf);
                    buf = NULL;
                }
                break;
            }
        }
        break;

        case UGS_FINISH: {
            *remain_len = 0;
        }
        break;
    }

    return OPRT_OS_ADAPTER_OK;
}


/**
 * @brief 固件ota数据传输完毕通知
 *        用户可以做固件校验以及设备重启
 * param[in]        reset       是否需要重启
 * @retval  =0      成功
 * @retval  <0      错误码
 */
int tuya_os_adapt_ota_end_inform(bool_t reset)
{    
    unsigned int i = 0,k = 0,rlen = 0,addr = 0;
    unsigned int flash_checksum = 0;
    unsigned char *pTempbuf = NULL;

    if(ug_proc == NULL) {
        LOG_ERR("ota don't start or start err, can't end inform!\r\n");
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }
    
    pTempbuf = tuya_os_adapt_system_malloc(BUF_SIZE);
    if(NULL == pTempbuf ) {
        LOG_ERR("Malloc failed!!\r\n");
        return OPRT_OS_ADAPTER_OTA_VERIFY_FAILED;
    }

    
    for(i = 0; i < ug_proc->file_header.bin_len; i += BUF_SIZE) {
        rlen  = ((ug_proc->file_header.bin_len - i) >= BUF_SIZE) ? BUF_SIZE : (ug_proc->file_header.bin_len - i);
        addr = ug_proc->start_addr + i;
        tuya_os_adapt_flash_read(addr, pTempbuf, rlen);
        if(0 == i) {
            memcpy(pTempbuf, frist_block_databuf ,RT_IMG_WR_UNIT); //first 4k block 
        } 
        
        for(k = 0; k < rlen; k++){
            flash_checksum += pTempbuf[k];
        }
     
    }
    
    if(flash_checksum != ug_proc->file_header.bin_sum) {
        LOG_ERR("verify_ota_checksum err  checksum(0x%x)  file_header.bin_sum(0x%x)\r\n",flash_checksum,ug_proc->file_header.bin_sum);
        
        goto OTA_VERIFY_PROC;
    } 
        
    tuya_os_adapt_flash_read(ug_proc->start_addr, pTempbuf, BUF_SIZE);

    tuya_os_adapt_flash_set_protect(FALSE);
    tuya_os_adapt_flash_erase(ug_proc->start_addr, BUF_SIZE);
    tuya_os_adapt_flash_set_protect(TRUE);

    memcpy(pTempbuf, frist_block_databuf, RT_IMG_WR_UNIT); // 还原头部信息的512byte
    tuya_os_adapt_flash_set_protect(FALSE);
    tuya_os_adapt_flash_write(ug_proc->start_addr, pTempbuf, BUF_SIZE);
    tuya_os_adapt_flash_set_protect(TRUE);


    LOG_DEBUG("the gateway upgrade success\r\n");
    
    tuya_os_adapt_system_free(pTempbuf);
    pTempbuf = NULL;
    
    tuya_os_adapt_system_free(ug_proc);
    ug_proc = NULL;

    tuya_os_adapt_system_free(frist_block_databuf);
    frist_block_databuf = NULL;

    if(TRUE == reset) { //verify 
        //OTA完成回调
        if(_ota_finish_notify) {
            _ota_finish_notify();
        }
        
        tuya_os_adapt_system_reset();
    }
    return OPRT_OS_ADAPTER_OK;
    
 OTA_VERIFY_PROC:
    if(pTempbuf != NULL) {
        tuya_os_adapt_system_free(pTempbuf);
        pTempbuf = NULL;
    }

    if(ug_proc != NULL) {
        tuya_os_adapt_system_free(ug_proc);
        ug_proc = NULL;
    }

    if(frist_block_databuf != NULL) {
        tuya_os_adapt_system_free(frist_block_databuf);
        frist_block_databuf = NULL;
    }
    
    return OPRT_OS_ADAPTER_OTA_END_INFORM_FAILED;
}

/**
 * @brief 设置OTA下载校验完成，即将重启之前的callback
 * param[in]  callback
 * @retval  null
 */
void set_ota_finish_notify_cb(SET_OTA_FINISH_NOTIFY callback)
{
    _ota_finish_notify = callback;
}

OPERATE_RET tuya_os_adapt_reg_ota_intf(void)
{
    return tuya_os_adapt_reg_intf(INTF_OTA, (void *)&m_tuya_os_ota_intfs);
}


