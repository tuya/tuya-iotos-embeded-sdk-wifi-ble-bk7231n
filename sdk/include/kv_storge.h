#ifndef _KV_STORGE_H
#define _KV_STORGE_H

#ifdef __cplusplus
	extern "C" {
#endif

#include "tuya_cloud_types.h"


/**
 * @brief kvs_init 初始化key-value存储模块
 * @param p_attr   可读写文件路径
 * @param p_encrypt_key     加密key
 * @param encrypt_key_len   加密key的长度
 * @return
 */
OPERATE_RET kvs_init(IN CONST CHAR_T *p_attr, IN CONST BYTE_T *p_encrypt_key, IN CONST UINT_T encrypt_key_len);

/**
 * @brief kvs_uninit 去初始化存储模块
 */
VOID kvs_uninit(VOID);

/**
 * @brief kvs_write 新建/更新key-value
 * @param name 要新建/更新的key值
 * @param data 要写入的value
 * @param len  value的长度
 * @return
 */
OPERATE_RET kvs_write(IN CONST CHAR_T *name,IN CONST BYTE_T *data,IN CONST UINT_T len);

/**
 * @brief kvs_read 读取key-value, key要完全匹配
 * @param name 要读取的key
 * @param data 要读取的value
 * @param len  value的长度
 * @return
 */
OPERATE_RET kvs_read(IN CONST CHAR_T *name,OUT BYTE_T **data,OUT UINT_T *len);

/**
 * @brief kvs_fuzzy_read 读取key-value, key要前端匹配
 * 比如存储了x1,x2,x3三个key，那么使用fuzzy_name=“x”进行读取时，均可以匹配到这三个key。
 * @param fuzzy_name 要前端匹配的key
 * @param index 要作为基准查询的索引，和查询返回的新的索引，以0开始。
 * @param data 要读取的value
 * @param len  value的长度
 * @return
 */
OPERATE_RET kvs_fuzzy_read(IN CONST CHAR_T *fuzzy_name,INOUT UINT_T *index,\
                           OUT BYTE_T **data,OUT UINT_T *len);

/**
 * @brief kvs_free_data 释放kvs_read/kvs_fuzzy_read 读取的内存
 * @param name 要删除的key
 * @return
 */
OPERATE_RET kvs_free_data(IN BYTE_T *p_data);

/**
 * @brief kvs_delete 删除一个key-value, key要完全匹配
 * @param name 要删除的key
 * @return
 */
OPERATE_RET kvs_delete(IN CONST CHAR_T *name);

/**
 * @brief kvs_fuzzy_delete 删除若干个key-value, key要前端匹配
 * @param fuzzy_name 要删除的前端匹配的key
 * @return
 */
OPERATE_RET kvs_fuzzy_delete(IN CONST CHAR_T *fuzzy_name);

/**
 * @brief kvs_format 格式化key-value存储
 * @return
 */
OPERATE_RET kvs_format(VOID);


#ifdef __cplusplus
}
#endif
#endif

