#ifndef __TUYA_HAL_FS_H__
#define __TUYA_HAL_FS_H__

#include <stdint.h>
#include <stdbool.h>
#include "tuya_os_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************
 * 文件目录管理
 *****************************************************************/

/**
 * @brief 创建目录
 * 
 * @param[in]       path        目录路径
 * @return  0=成功，非0=失败
 */
INT_T tuya_hal_fs_mkdir(CONST CHAR_T* path);

/**
 * @brief 删除目录或文件
 * 
 * @param[in]       path        目录或文件的路径
 * @return  0=成功，非0=失败
 */
INT_T tuya_hal_fs_remove(CONST CHAR_T* path);

/**
 * @brief 获取文件属性
 * 
 * @param[in]       path        目录或文件的路径
 * @param[out]      mode        属性位，同 linux struct stat 的 st_mode 字段
 * @return  0=成功，非0=失败
 */
INT_T tuya_hal_fs_mode(CONST CHAR_T* path, UINT_T* mode);

/**
 * @brief 判断文件或目录是否存在
 * 
 * @param[in]       path        目录或文件的路径
 * @param[out]      is_exist    是否存在
 * @return  0=成功，非0=失败
 */
INT_T tuya_hal_fs_is_exist(CONST CHAR_T* path, BOOL_T* is_exist);

/**
 * @brief 文件重命名
 * 
 * @param[in]       path_old    老的文件名
 * @param[in]       path_new    新的文件名
 * @return  0=成功，非0=失败
 */
INT_T tuya_hal_fs_rename(CONST CHAR_T* path_old, CONST CHAR_T* path_new);


/*****************************************************************
 * 子目录遍历
 *****************************************************************/

/**
 * @brief 打开目录
 * 
 * @param[in]       path        目录的路径
 * @param[out]      dir         返回目录句柄
 * @return  0=成功，非0=失败
 */
INT_T tuya_hal_dir_open(CONST CHAR_T* path, TUYA_DIR* dir);

/**
 * @brief 关闭目录句柄
 * 
 * @param[in]       dir         目录句柄
 * @return  0=成功，非0=失败
 */
INT_T tuya_hal_dir_close(TUYA_DIR dir);

/**
 * @brief 读取当前节点的文件信息，内部指针指向下一个节点
 * 
 * @param[in]       dir         目录句柄
 * @param[out]      info        返回文件信息句柄
 * @return  0=成功，非0=失败
 */
INT_T tuya_hal_dir_read(TUYA_DIR dir, TUYA_FILEINFO* info);

/**
 * @brief 获取节点的名称
 * 
 * @param[in]       info        文件信息句柄
 * @param[out]      name        返回名称字符串地址
 * @return  0=成功，非0=失败
 */
INT_T tuya_hal_dir_name(TUYA_FILEINFO info, CONST CHAR_T** name);

/**
 * @brief 判断节点是否为目录
 * 
 * @param[in]       info        文件信息句柄
 * @param[out]      is_dir      返回是否为目录
 * @return  0=成功，非0=失败
 */
INT_T tuya_hal_dir_is_directory(TUYA_FILEINFO info, BOOL_T* is_dir);

/**
 * @brief 判断节点是否为普通文件
 * 
 * @param[in]       info        文件信息句柄
 * @param[out]      is_regular  返回是否为普通文件
 * @return  0=成功，非0=失败
 */
INT_T tuya_hal_dir_is_regular(TUYA_FILEINFO info, BOOL_T* is_regular);


/*****************************************************************
 * 文件IO
 *****************************************************************/

/**
 * @brief 打开文件
 * 
 * @param[in]       path        文件路径
 * @param[in]       mode        文件打开模式: "r","w"...
 * @return  返回文件句柄
 */
TUYA_FILE tuya_hal_fopen(CONST CHAR_T* path, CONST CHAR_T* mode);

/**
 * @brief 关闭文件句柄
 * 
 * @param[in]       file        文件句柄
 * @return  0=成功，非0=失败
 */
INT_T tuya_hal_fclose(TUYA_FILE file);

/**
 * @brief 读文件
 * 
 * @param[in]       buf         缓冲区地址
 * @param[in]       bytes       缓冲区大小
 * @param[in]       file        文件句柄
 * @return  <0: 失败; >= 实际读取的字节数
 */
INT_T tuya_hal_fread(VOID_T* buf, INT_T bytes, TUYA_FILE file);

/**
 * @brief 写文件
 * 
 * @param[in]       buf         缓冲区地址
 * @param[in]       bytes       缓冲区大小
 * @param[in]       file        文件句柄
 * @return  <0: 失败; >= 实际写入的字节数
 */
INT_T tuya_hal_fwrite(VOID_T* buf, INT_T bytes, TUYA_FILE file);

/**
 * @brief 将缓存写入存储器
 * 
 * @param[in]       file        My Param doc
 * @return  0=成功，非0=失败
 */
INT_T tuya_hal_fsync(TUYA_FILE file);

/**
 * @brief 读取字符串
 * 
 * @param[in]       buf         缓冲区地址
 * @param[in]       len         缓冲区大小
 * @param[in]       file        文件句柄
 * @return  读入的字符串指针
 */
CHAR_T* tuya_hal_fgets(CHAR_T* buf, INT_T len, TUYA_FILE file);

/**
 * @brief 判断是否到达文件尾
 * 
 * @param[in]       file        文件句柄
 * @return  0=未到文件尾，非0=到达文件尾
 */
INT_T tuya_hal_feof(TUYA_FILE file);

/**
 * @brief 文件定位
 * 
 * @param[in]       file        文件句柄
 * @param[in]       offs        偏移量
 * @param[in]       whence      起始点类型
 * @return  0=成功，非0=失败
 */
INT_T tuya_hal_fseek(TUYA_FILE file, INT64_T offs, INT_T whence);

/**
 * @brief 获取文件位置
 * 
 * @param[in]       file        文件句柄
 * @return  返回当前位置
 */
INT64_T tuya_hal_ftell(TUYA_FILE file);

//该函数用于fs组件接口初始化
VOID_T tuya_os_fs_intf_init(VOID_T);



#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif // __TUYA_HAL_FS_H__

