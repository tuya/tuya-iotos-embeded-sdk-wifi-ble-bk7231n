/***********************************************************
*  File: svc_online_log_seq.h.h
*  Author: nzy
*  Date: 20190422
***********************************************************/
#ifndef __LOG_SEQ_H
#define __LOG_SEQ_H

#include "tuya_cloud_types.h"
#include "tuya_os_adapter.h"
#include "uni_pointer.h"

#ifdef __cplusplus
	extern "C" {
#endif


/***********************************************************
*************************micro define***********************
***********************************************************/
#define STR_DATA_MAX_LENGTH 64

typedef BYTE_T LS_DATA_TP_T; // log sequence data type
#define LDT_NULL 0
#define LDT_DEC 1
#define LDT_HEX 2
#define LDT_TIMESTAMP 3
#define LDT_STRING 4

// 日志序数据存储，注意都为4字节长度
typedef union {
    INT_T   dec_data;   //
    UINT_T  hex_data;
    TIME_T  tm_data;
    CHAR_T  *str_data;
}LOG_DA_TP_U;

// 日志序数据结构
typedef struct {
    BYTE_T          id;                 // 日志序序号
    LS_DATA_TP_T    type;               // 日志序类型
    LOG_DA_TP_U     data;               // 日志序数据

    struct tuya_list_head node;              // 日志序挂接节点
}LS_DATA_S;

typedef BYTE_T LS_STAT_T;
#define LSS_INIT 0
#define LSS_RECORDING 1
#define LSS_LOCKED 2 // log_seq_t set locked status when record the error log sequence

// Calling when record the error log sequence
typedef int (*LOG_SEQ_UPLOAD_CB)(const char *p_log); 

#define LS_DELIMITER " | "

int log_seq_insert_log(const char *ls_name, BYTE_T id, LS_DATA_TP_T type, VOID_T* data);
int log_seq_insert_error_log(const char *ls_name, BYTE_T id, LS_DATA_TP_T type, VOID_T* data);
int log_seq_insert_report_log(const char *ls_name, BYTE_T id, LS_DATA_TP_T type, VOID_T* data);

#define INSERT_LOG_SEQ_NULL(seq)           log_seq_insert_log(LOGSEQ_OBJ, seq, LDT_NULL, NULL)
#define INSERT_LOG_SEQ_DEC(seq,dec)        log_seq_insert_log(LOGSEQ_OBJ, seq, LDT_DEC, (VOID_T*)dec)
#define INSERT_LOG_SEQ_HEX(seq,hex)        log_seq_insert_log(LOGSEQ_OBJ, seq, LDT_HEX, (VOID_T*)hex)
#define INSERT_LOG_SEQ_TM(seq,tm)          log_seq_insert_log(LOGSEQ_OBJ, seq, LDT_TIMESTAMP, (VOID_T*)tm)
//string max length is STR_DATA_MAX_LENGTH
#define INSERT_LOG_SEQ_STR(seq,str)        log_seq_insert_log(LOGSEQ_OBJ, seq, LDT_STRING, (VOID_T*)str)
#define INSERT_ERROR_LOG_SEQ_NULL(seq)     log_seq_insert_error_log(LOGSEQ_OBJ, seq, LDT_NULL, NULL)
#define INSERT_ERROR_LOG_SEQ_DEC(seq,dec)  log_seq_insert_error_log(LOGSEQ_OBJ, seq, LDT_DEC, (VOID_T*)dec)
#define INSERT_ERROR_LOG_SEQ_HEX(seq,hex)  log_seq_insert_error_log(LOGSEQ_OBJ, seq, LDT_HEX, (VOID_T*)hex)
#define INSERT_ERROR_LOG_SEQ_TM(seq,tm)    log_seq_insert_error_log(LOGSEQ_OBJ, seq, LDT_TIMESTAMP, (VOID_T*)tm)
//string max length is STR_DATA_MAX_LENGTH
#define INSERT_ERROR_LOG_SEQ_STR(seq,str)  log_seq_insert_error_log(LOGSEQ_OBJ, seq, LDT_STRING, (VOID_T*)str)
#define INSERT_REPORT_LOG_SEQ_NULL(seq)    log_seq_insert_report_log(LOGSEQ_OBJ, seq, LDT_NULL, NULL)
#define INSERT_REPORT_LOG_SEQ_DEC(seq,dec) log_seq_insert_report_log(LOGSEQ_OBJ, seq, LDT_DEC, (VOID_T*)dec)
#define INSERT_REPORT_LOG_SEQ_HEX(seq,hex) log_seq_insert_report_log(LOGSEQ_OBJ, seq, LDT_HEX, (VOID_T*)hex)
#define INSERT_REPORT_LOG_SEQ_TM(seq,tm)   log_seq_insert_report_log(LOGSEQ_OBJ, seq, LDT_TIMESTAMP, (VOID_T*)tm)
//string max length is STR_DATA_MAX_LENGTH
#define INSERT_REPORT_LOG_SEQ_STR(seq,str) log_seq_insert_report_log(LOGSEQ_OBJ, seq, LDT_STRING, (VOID_T*)str)

/**
 * @brief 日志序初始化接口
 * 
 * @param[in] log_seq_path: 日志序存储路径
 * @param[in] upload_cb：日志序上传cb函数，由online log模块注册提供
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
int log_seq_init(const char *log_seq_path, LOG_SEQ_UPLOAD_CB upload_cb);

/**
 * @brief 设置日志序是否使能
 * 
 * @param[in] enable: 是否使能日志序
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
int log_seq_set_enable(bool_t enable);

/**
 * @brief 设置日志序存储深度
 * 
 * @param[in] depth: 日志序对象最多缓存的日志序个数
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
int log_seq_set_depth(const       unsigned int depth);


/**
 * @brief 日志序重置，清除所有的日志序，在设备重置时候使用
 *
 * @param[in] is_force_cleanis_force_clean: 强制清除全部日志序对象
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
int log_seq_clean(int is_force_clean_all);

/**
 * @brief 强制同步所有日志序
 * 
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
int log_seq_force_sync(void);

/**
 * @brief 上传客户日志序信息接口
 * 
 * @param[in] log: 需要上传的日志字符串
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
int log_seq_upload_custom_log(const char *p_log);

/**
 * @brief 获取日志序内容
 * 
 * @param[out] log_buff: 日志序缓冲区
 * @param[out] log_len: 日志序日志长度
 *
 * @return int: 0成功，非0，请参照tuya error code描述文档
 */
int log_seq_get_netcfg_log(char **log_buff, int *log_len);



#ifdef __cplusplus
}
#endif
#endif

