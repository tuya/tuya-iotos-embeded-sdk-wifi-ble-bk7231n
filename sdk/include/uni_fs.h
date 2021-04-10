#ifndef __TUYA_UNI_FS_H__
#define __TUYA_UNI_FS_H__


#include "tuya_hal_fs.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief 创建目录，如果目录存在则忽略错误
 * 
 * @param[in]       path        目录的路径
 * @return  0=成功，非0=失败
 */
int uni_mkdir(const char* path);

/**
 * @brief 创建多级目录
 * 
 * @param[in]       path        目录的路径
 * @return  0=成功，非0=失败
 */
int uni_mkdir_all(const char* path);

/**
 * @brief 删除文件或目录，如果不存在则忽略错误
 * 
 * @param[in]       path        文件或目录的路径
 * @return  0=成功，非0=失败
 */
int uni_remove(const char* path);

/**
 * @brief 判断文件或目录是否存在
 * 
 * @param[in]       path        文件或目录的路径
 * @return  是否存在
 */
bool uni_is_exist(const char* path);

/**
 * @brief 判断文件是否可读
 * 
 * @param[in]       path        文件的路径
 * @return  是否可读
 */
bool uni_is_readable(const char* path);

/**
 * @brief 判断文件是否可写
 * 
 * @param[in]       path        文件的路径
 * @return  是否可写
 */
bool uni_is_writable(const char* path);

/**
 * @brief 判断文件是否可执行
 * 
 * @param[in]       path        文件的路径
 * @return  是否可执行
 */
bool uni_is_executable(const char* path);


#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif // __TUYA_UNI_FS_H__

