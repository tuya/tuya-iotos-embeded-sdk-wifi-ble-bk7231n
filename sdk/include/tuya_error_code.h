/*******************************************************************
*  File: tuya_error_code.h
*  Author: auto generate by tuya code gen system
*  Date: 2021-04-22
*  Description:this file defined the error code of tuya IOT 
*  you can change it manully if needed
*  Copyright(C),2018-2020, tuya inc, www.tuya.comm
*******************************************************************/

#ifndef TUYA_ERROR_CODE_H
#define TUYA_ERROR_CODE_H

#ifdef __cplusplus
extern "C" {
#endif


typedef int OPERATE_RET;


/****************************************************************************
            the error code marco define for module GLOBAL 
****************************************************************************/
#define OPRT_OK                                            (-0x0000)  //执行成功
#define OPRT_COM_ERROR                                     (-0x0001)  //通用错误
#define OPRT_INVALID_PARM                                  (-0x0002)  //无效的入参
#define OPRT_MALLOC_FAILED                                 (-0x0003)  //内存分配失败
#define OPRT_NOT_SUPPORTED                                 (-0x0004)  //不支持
#define OPRT_NETWORK_ERROR                                 (-0x0005)  //网络错误
#define OPRT_NOT_FOUND                                     (-0x0006)  //没有找到对象
#define OPRT_CR_CJSON_ERR                                  (-0x0007)  //创建json对象失败
#define OPRT_CJSON_PARSE_ERR                               (-0x0008)  //json解析失败
#define OPRT_CJSON_GET_ERR                                 (-0x0009)  //获取json对象失败
#define OPRT_CR_MUTEX_ERR                                  (-0x000a)  //创建信号量失败
#define OPRT_SOCK_ERR                                      (-0x000b)  //创建socket失败
#define OPRT_SET_SOCK_ERR                                  (-0x000c)  //socket设置失败
#define OPRT_SOCK_CONN_ERR                                 (-0x000d)  //socket连接失败
#define OPRT_SEND_ERR                                      (-0x000e)  //发送失败
#define OPRT_RECV_ERR                                      (-0x000f)  //接收失败
#define OPRT_RECV_DA_NOT_ENOUGH                            (-0x0010)  //接收数据不完整
#define OPRT_KVS_WR_FAIL                                   (-0x0011)  //KV写失败
#define OPRT_KVS_RD_FAIL                                   (-0x0012)  //KV读失败
#define OPRT_CRC32_FAILED                                  (-0x0013)  //CRC校验失败
#define OPRT_TIMEOUT                                       (-0x0014)  //超时
#define OPRT_INIT_MORE_THAN_ONCE                           (-0x0015)  //初始化超过一次
#define OPRT_INDEX_OUT_OF_BOUND                            (-0x0016)  //索引越界
#define OPRT_RESOURCE_NOT_READY                            (-0x0017)  //资源未完善
#define OPRT_EXCEED_UPPER_LIMIT                            (-0x0018)  //超过上限
#define OPRT_FILE_NOT_FIND                                 (-0x0019)  //文件未找到
#define OPRT_GLOBAL_ERRCODE_MAX_CNT 26


/****************************************************************************
            the error code marco define for module BASE_OS_ADAPTER 
****************************************************************************/
#define OPRT_BASE_OS_ADAPTER_REG_NULL_ERROR                (-0x0100)  //系统适配注册失败
#define OPRT_BASE_OS_ADAPTER_INIT_MUTEX_ATTR_FAILED        (-0x0101)  //初始化同步属性失败
#define OPRT_BASE_OS_ADAPTER_SET_MUTEX_ATTR_FAILED         (-0x0102)  //设置同步属性失败
#define OPRT_BASE_OS_ADAPTER_DESTROY_MUTEX_ATTR_FAILED     (-0x0103)  //销毁同步属性失败
#define OPRT_BASE_OS_ADAPTER_INIT_MUTEX_FAILED             (-0x0104)  //初始化互斥量失败
#define OPRT_BASE_OS_ADAPTER_MUTEX_LOCK_FAILED             (-0x0105)  //互斥量加锁失败
#define OPRT_BASE_OS_ADAPTER_MUTEX_TRYLOCK_FAILED          (-0x0106)  //互斥量尝试加锁失败
#define OPRT_BASE_OS_ADAPTER_MUTEX_LOCK_BUSY               (-0x0107)  //互斥量忙
#define OPRT_BASE_OS_ADAPTER_MUTEX_UNLOCK_FAILED           (-0x0108)  //互斥量解锁失败
#define OPRT_BASE_OS_ADAPTER_MUTEX_RELEASE_FAILED          (-0x0109)  //互斥量释放失败
#define OPRT_BASE_OS_ADAPTER_CR_MUTEX_ERR                  (-0x010a)  //互斥量创建失败
#define OPRT_BASE_OS_ADAPTER_MEM_PARTITION_EMPTY           (-0x010b)  //内存分区空
#define OPRT_BASE_OS_ADAPTER_MEM_PARTITION_FULL            (-0x010c)  //内存分区满
#define OPRT_BASE_OS_ADAPTER_MEM_PARTITION_NOT_FOUND       (-0x010d)  //内存分区不存在
#define OPRT_BASE_OS_ADAPTER_INIT_SEM_FAILED               (-0x010e)  //初始化信号量失败
#define OPRT_BASE_OS_ADAPTER_WAIT_SEM_FAILED               (-0x010f)  //等待信号量失败
#define OPRT_BASE_OS_ADAPTER_POST_SEM_FAILED               (-0x0110)  //释放信号量失败
#define OPRT_BASE_OS_ADAPTER_THRD_STA_UNVALID              (-0x0111)  //线程状态非法
#define OPRT_BASE_OS_ADAPTER_THRD_CR_FAILED                (-0x0112)  //线程创建失败
#define OPRT_BASE_OS_ADAPTER_THRD_JOIN_FAILED              (-0x0113)  //线程JOIN函数调用失败
#define OPRT_BASE_OS_ADAPTER_THRD_SELF_CAN_NOT_JOIN        (-0x0114)  //自身线程不能调用JOIN函数
#define OPRT_BASE_OS_ADAPTER_ERRCODE_MAX_CNT 21


/****************************************************************************
            the error code marco define for module BASE_UTILITIES 
****************************************************************************/
#define OPRT_BASE_UTILITIES_PARTITION_EMPTY                (-0x0200)  //无空闲链表
#define OPRT_BASE_UTILITIES_PARTITION_FULL                 (-0x0201)  //链表已满
#define OPRT_BASE_UTILITIES_PARTITION_NOT_FOUND            (-0x0202)  //链表未遍历到
#define OPRT_BASE_UTILITIES_ERRCODE_MAX_CNT 3


/****************************************************************************
            the error code marco define for module BASE_SECURITY 
****************************************************************************/
#define OPRT_BASE_SECURITY_CRC32_FAILED                    (-0x0300)  //CRC32错误
#define OPRT_BASE_SECURITY_ERRCODE_MAX_CNT 1


/****************************************************************************
            the error code marco define for module BASE_LOG_MNG 
****************************************************************************/
#define OPRT_BASE_LOG_MNG_DONOT_FOUND_MODULE               (-0x0400)  //未发现log模块
#define OPRT_BASE_LOG_MNG_PRINT_LOG_LEVEL_HIGHER           (-0x0401)  //log级别低
#define OPRT_BASE_LOG_MNG_FORMAT_STRING_FAILED             (-0x0402)  //log字符串格式化失败
#define OPRT_BASE_LOG_MNG_LOG_SEQ_OPEN_FILE_FAIL           (-0x0403)  //打开日志序文件失败
#define OPRT_BASE_LOG_MNG_LOG_SEQ_WRITE_FILE_FAIL          (-0x0404)  //写日志序文件失败
#define OPRT_BASE_LOG_MNG_LOG_SEQ_FILE_FULL                (-0x0405)  //日志序文件满
#define OPRT_BASE_LOG_MNG_LOG_SEQ_FILE_NOT_EXIST           (-0x0406)  //日志序文件不存在
#define OPRT_BASE_LOG_MNG_LOG_SEQ_NAME_INVALIDE            (-0x0407)  //日志序名称无效
#define OPRT_BASE_LOG_MNG_LOG_SEQ_CREATE_FAIL              (-0x0408)  //日志序创建失败
#define OPRT_BASE_LOG_MNG_ERRCODE_MAX_CNT 9


/****************************************************************************
            the error code marco define for module BASE_TIMEQ 
****************************************************************************/
#define OPRT_BASE_TIMEQ_TIMERID_EXIST                      (-0x0600)  //定时器ID已存在
#define OPRT_BASE_TIMEQ_TIMERID_NOT_FOUND                  (-0x0601)  //未找到指定定时器ID
#define OPRT_BASE_TIMEQ_TIMERID_UNVALID                    (-0x0602)  //定时器ID非法
#define OPRT_BASE_TIMEQ_GET_IDLE_TIMERID_ERROR             (-0x0603)  //获取空闲定时器ID错误
#define OPRT_BASE_TIMEQ_ERRCODE_MAX_CNT 4


/****************************************************************************
            the error code marco define for module BASE_MSGQ 
****************************************************************************/
#define OPRT_BASE_MSGQ_NOT_FOUND                           (-0x0700)  //消息未找到
#define OPRT_BASE_MSGQ_LIST_EMPTY                          (-0x0701)  //列表为空
#define OPRT_BASE_MSGQ_ERRCODE_MAX_CNT 2


/****************************************************************************
            the error code marco define for module MID_HTTP 
****************************************************************************/
#define OPRT_MID_HTTP_BUF_NOT_ENOUGH                       (-0x0800)  //缓冲区长度不足
#define OPRT_MID_HTTP_URL_PARAM_OUT_LIMIT                  (-0x0801)  //URL长度超出限制
#define OPRT_MID_HTTP_OS_ERROR                             (-0x0802)  //系统错误
#define OPRT_MID_HTTP_PR_REQ_ERROR                         (-0x0803)  //准备请求错误
#define OPRT_MID_HTTP_SD_REQ_ERROR                         (-0x0804)  //发送请求错误
#define OPRT_MID_HTTP_RD_ERROR                             (-0x0805)  //读取错误
#define OPRT_MID_HTTP_AD_HD_ERROR                          (-0x0806)  //添加头错误
#define OPRT_MID_HTTP_GET_RESP_ERROR                       (-0x0807)  //获取应答错误
#define OPRT_MID_HTTP_AES_INIT_ERR                         (-0x0808)  //AES初始化错误
#define OPRT_MID_HTTP_AES_OPEN_ERR                         (-0x0809)  //AES打开错误
#define OPRT_MID_HTTP_AES_SET_KEY_ERR                      (-0x080a)  //AES设置KEY错误
#define OPRT_MID_HTTP_AES_ENCRYPT_ERR                      (-0x080b)  //AES加密错误
#define OPRT_MID_HTTP_CR_HTTP_URL_H_ERR                    (-0x080c)  //创建HTTP URL头错误
#define OPRT_MID_HTTP_HTTPS_HANDLE_FAIL                    (-0x080d)  //HTTPS句柄错误
#define OPRT_MID_HTTP_HTTPS_RESP_UNVALID                   (-0x080e)  //HTTPS无效应答
#define OPRT_MID_HTTP_NO_SUPPORT_RANGE                     (-0x080f)  //不支持断点续传
#define OPRT_MID_HTTP_ERRCODE_MAX_CNT 16


/****************************************************************************
            the error code marco define for module MID_MQTT 
****************************************************************************/
#define OPRT_MID_MQTT_DEF_ERR                              (-0x0900)  //定义失败
#define OPRT_MID_MQTT_INVALID_PARM                         (-0x0901)  //参数无效
#define OPRT_MID_MQTT_MALLOC_FAILED                        (-0x0902)  //内存申请失败
#define OPRT_MID_MQTT_DNS_PARSED_FAILED                    (-0x0903)  //DNS解析失败
#define OPRT_MID_MQTT_SOCK_CREAT_FAILED                    (-0x0904)  //socket创建失败
#define OPRT_MID_MQTT_SOCK_SET_FAILED                      (-0x0905)  //socket set失败
#define OPRT_MID_MQTT_TCP_CONNECD_FAILED                   (-0x0906)  //tcp连接失败
#define OPRT_MID_MQTT_TCP_TLS_CONNECD_FAILED               (-0x0907)  //tcp tls连接失败
#define OPRT_MID_MQTT_PACK_SEND_FAILED                     (-0x0908)  //包发送失败
#define OPRT_MID_MQTT_RECV_DATA_FORMAT_WRONG               (-0x0909)  //接收数据格式错误
#define OPRT_MID_MQTT_MSGID_NOT_MATCH                      (-0x090a)  //接收数据msgid未找到
#define OPRT_MID_MQTT_START_TM_MSG_ERR                     (-0x090b)  //开始事件msg错误
#define OPRT_MID_MQTT_OVER_MAX_MESSAGE_LEN                 (-0x090c)  //超过消息最大长度
#define OPRT_MID_MQTT_PING_SEND_ERR                        (-0x090d)  //ping发送失败
#define OPRT_MID_MQTT_PUBLISH_TIMEOUT                      (-0x090e)  //发布超时
#define OPRT_MID_MQTT_ERRCODE_MAX_CNT 15


/****************************************************************************
            the error code marco define for module MID_TLS 
****************************************************************************/
#define OPRT_MID_TLS_NET_SOCKET_ERROR                      (-0x0a00)  //Failed to open a socket
#define OPRT_MID_TLS_NET_CONNECT_ERROR                     (-0x0a01)  //The connection to the given server / port failed.
#define OPRT_MID_TLS_UNKNOWN_HOST_ERROR                    (-0x0a02)  //Failed to get an IP address for the given hostname.
#define OPRT_MID_TLS_CONNECTION_ERROR                      (-0x0a03)  //TLS连接失败
#define OPRT_MID_TLS_DRBG_ENTROPY_ERROR                    (-0x0a04)  //mbedtls随机种子生成失败
#define OPRT_MID_TLS_X509_ROOT_CRT_PARSE_ERROR             (-0x0a05)  //X509根证书解析失败
#define OPRT_MID_TLS_X509_DEVICE_CRT_PARSE_ERROR           (-0x0a06)  //X509设备证书解析失败
#define OPRT_MID_TLS_CTR_DRBG_ENTROPY_SOURCE_ERROR         (-0x0a07)  //The entropy source failed
#define OPRT_MID_TLS_PK_PRIVATE_KEY_PARSE_ERROR            (-0x0a08)  //秘钥解析失败
#define OPRT_MID_TLS_ERRCODE_MAX_CNT 9


/****************************************************************************
            the error code marco define for module SVC_WIFI_NETCFG 
****************************************************************************/
#define OPRT_SVC_WIFI_NETCFG_RECV_CONTINUE                 (-0x0b00)  //继续接收配网包
#define OPRT_SVC_WIFI_NETCFG_ERRCODE_MAX_CNT 1


/****************************************************************************
            the error code marco define for module SVC_MF_TEST 
****************************************************************************/
#define OPRT_SVC_MF_TEST_UPDATE_DATA_LEN_EXECED            (-0x0d00)  //升级数据长度超过处理上限
#define OPRT_SVC_MF_TEST_UPDATE_CRC_ERROR                  (-0x0d01)  //升级crc校验失败
#define OPRT_SVC_MF_TEST_ERRCODE_MAX_CNT 2


/****************************************************************************
            the error code marco define for module SVC_DP 
****************************************************************************/
#define OPRT_SVC_DP_ALREADY_PROCESS                        (-0x0e00)  //DP已经处理
#define OPRT_SVC_DP_ID_NOT_FOUND                           (-0x0e01)  //DP ID没有发现
#define OPRT_SVC_DP_TP_NOT_MATCH                           (-0x0e02)  //DP 类型未匹配
#define OPRT_SVC_DP_DEVICE_NOT_BINDED                      (-0x0e03)  //设备未绑定
#define OPRT_SVC_DP_TYPE_PROP_ILLEGAL                      (-0x0e04)  //类型属性不合法
#define OPRT_SVC_DP_NW_INVALID                             (-0x0e05)  //网络无效
#define OPRT_SVC_DP_SECURITY_VERIFY_ERR                    (-0x0e06)  //安全校验失败
#define OPRT_SVC_DP_ERRCODE_MAX_CNT 7


/****************************************************************************
            the error code marco define for module BASE_EVENT 
****************************************************************************/
#define OPRT_BASE_EVENT_INVALID_EVENT_NAME                 (-0x0f00)  //无效的事件名
#define OPRT_BASE_EVENT_INVALID_EVENT_DESC                 (-0x0f01)  //无效的事件描述
#define OPRT_BASE_EVENT_ERRCODE_MAX_CNT 2


/****************************************************************************
            the error code marco define for module SVC_TIMER_TASK 
****************************************************************************/
#define OPRT_SVC_TIMER_TASK_LOAD_INVALID_CJSON             (-0x1000)  //K/V中保存的数据JSON格式错误
#define OPRT_SVC_TIMER_TASK_LOAD_INVALID_DATA              (-0x1001)  //K/V中保存的数据缺少cnt字段
#define OPRT_SVC_TIMER_TASK_UPDATE_LAST_FETCH_INVALID      (-0x1002)  //云端返回的数据缺少lastFetchTime字段
#define OPRT_SVC_TIMER_TASK_UPDATE_TIMER_CNT_INVALID       (-0x1003)  //云端返回的数据缺少count字段
#define OPRT_SVC_TIMER_TASK_UPDATE_TIMER_CNT_EXCEED        (-0x1004)  //云端返回的定时任务数量超过30个
#define OPRT_SVC_TIMER_TASK_ERRCODE_MAX_CNT 5


/****************************************************************************
            the error code marco define for module SVC_HTTP 
****************************************************************************/
#define OPRT_SVC_HTTP_NOT_ENCRYPT_RET                      (-0x1100)  //结果未加密
#define OPRT_SVC_HTTP_FILL_URL_H_ERR                       (-0x1101)  //构造header错误
#define OPRT_SVC_HTTP_FILL_URL_FULL_ERR                    (-0x1102)  //构造整个URL错误
#define OPRT_SVC_HTTP_FILL_DATA_ERR                        (-0x1103)  //构造data错误
#define OPRT_SVC_HTTP_URL_CFG_AI_SPEAKER_ERR               (-0x1104)  //音响配置失败
#define OPRT_SVC_HTTP_URL_CFG_URL_ERR                      (-0x1105)  //httpUrl/mqttUrl字段缺失
#define OPRT_SVC_HTTP_URL_CFG_URL2IP_ERR                   (-0x1106)  //httpUrl/mqttUrl解析错误
#define OPRT_SVC_HTTP_URL_CFG_URL2IP_SELF_ERR              (-0x1107)  //httpsSelfUrl/mqttsSelfUrl解析错误
#define OPRT_SVC_HTTP_URL_CFG_URL2IP_VERIFY_ERR            (-0x1108)  //httpsVerifyUrl/mqttsVerifyUrl解析错误
#define OPRT_SVC_HTTP_URL_CFG_URL2IP_PSK_ERR               (-0x1109)  //httpsPSKUrl/mqttsPSKUrl解析错误
#define OPRT_SVC_HTTP_RECV_ERR                             (-0x110a)  //接收数据错误
#define OPRT_SVC_HTTP_RECV_DA_NOT_ENOUGH                   (-0x110b)  //接收数据不足
#define OPRT_SVC_HTTP_API_VERIFY_FAILED                    (-0x110c)  //数据校验错误
#define OPRT_SVC_HTTP_GW_NOT_EXIST                         (-0x110d)  //网关信息不存在
#define OPRT_SVC_HTTP_API_TOKEN_EXPIRE                     (-0x110e)  //TOKEN过期
#define OPRT_SVC_HTTP_DEV_RESET_FACTORY                    (-0x110f)  //设备需要恢复出厂
#define OPRT_SVC_HTTP_DEV_NEED_REGISTER                    (-0x1110)  //设备未注册
#define OPRT_SVC_HTTP_ORDER_EXPIRE                         (-0x1111)  //订单已过期
#define OPRT_SVC_HTTP_NOT_EXISTS                           (-0x1112)  //不存在
#define OPRT_SVC_HTTP_SIGNATURE_ERROR                      (-0x1113)  //签名错误
#define OPRT_SVC_HTTP_API_VERSION_WRONG                    (-0x1114)  //API版本错误
#define OPRT_SVC_HTTP_DEVICE_REMOVED                       (-0x1115)  //设备已移除
#define OPRT_SVC_HTTP_DEV_ALREADY_BIND                     (-0x1116)  //设备已经绑定
#define OPRT_SVC_HTTP_REMOTE_API_RUN_UNKNOW_FAILED         (-0x1117)  //无法识别API
#define OPRT_SVC_HTTP_FORMAT_STRING_FAILED                 (-0x1118)  //字符串格式化错误
#define OPRT_SVC_HTTP_API_DECODE_FAILED                    (-0x1119)  //数据解密失败
#define OPRT_SVC_HTTP_SERV_VRFY_FAIL                       (-0x111a)  //服务端校验失败
#define OPRT_SVC_HTTP_ERRCODE_MAX_CNT 27


/****************************************************************************
            the error code marco define for module SVC_BT_API 
****************************************************************************/
#define OPRT_SVC_BT_API_TRSMITR_CONTINUE                   (-0x1200)  //传输未结束
#define OPRT_SVC_BT_API_TRSMITR_ERROR                      (-0x1201)  //传输错误
#define OPRT_SVC_BT_API_ERRCODE_MAX_CNT 2


/****************************************************************************
            the error code marco define for module SVC_LAN 
****************************************************************************/
#define OPRT_SVC_LAN_SOCKET_FAULT                          (-0x1500)  //socket错误
#define OPRT_SVC_LAN_SEND_ERR                              (-0x1501)  //socket发送错误
#define OPRT_SVC_LAN_NO_CLIENT_CONNECTED                   (-0x1502)  //没有可以上报的局域网设备连接
#define OPRT_SVC_LAN_ERRCODE_MAX_CNT 3


/****************************************************************************
            the error code marco define for module SVC_MQTT 
****************************************************************************/
#define OPRT_SVC_MQTT_CMD_NOT_EXEC                         (-0x1700)  //命令未执行
#define OPRT_SVC_MQTT_CMD_OUT_OF_TIME                      (-0x1701)  //命令未在规定时间内执行
#define OPRT_SVC_MQTT_GW_MQ_OFFLILNE                       (-0x1702)  //MQTT离线
#define OPRT_SVC_MQTT_ERRCODE_MAX_CNT 3


/****************************************************************************
            the error code marco define for module SVC_PEGASUS 
****************************************************************************/
#define OPRT_SVC_PEGASUS_DECODE_FAILED                     (-0x1800)  //解码失败
#define OPRT_SVC_PEGASUS_DONOT_FOUND_MODULE                (-0x1801)  //模块未找到
#define OPRT_SVC_PEGASUS_ERRCODE_MAX_CNT 2


/****************************************************************************
            the error code marco define for module SVC_UPGRADE 
****************************************************************************/
#define OPRT_SVC_UPGRADE_APP_NOT_READY                     (-0x1900)  //应用尚未就绪
#define OPRT_SVC_UPGRADE_NO_VALID_FIRMWARE                 (-0x1901)  //升级信息字段校验失败
#define OPRT_SVC_UPGRADE_ERRCODE_MAX_CNT 2


/****************************************************************************
            the error code marco define for module SVC_API_IOT 
****************************************************************************/
#define OPRT_SVC_API_IOT_DISCONNECTED_WITH_ROUTER          (-0x1a00)  //路由器断开
#define OPRT_SVC_API_IOT_DEV_NOT_BIND                      (-0x1a01)  //设备未绑定
#define OPRT_SVC_API_IOT_ERRCODE_MAX_CNT 2


/****************************************************************************
            the error code marco define for module SVC_SMART_GW 
****************************************************************************/
#define OPRT_SVC_SMART_GW_SCMA_INVALID                     (-0x1b00)  //SCMA无效
#define OPRT_SVC_SMART_GW_DEV_DP_CNT_INVALID               (-0x1b01)  //设备DP数量无效
#define OPRT_SVC_SMART_GW_NO_AUTHENTICATION                (-0x1b02)  //无授权
#define OPRT_SVC_SMART_GW_ROUTER_NOT_FIND                  (-0x1b03)  //路由器未找到
#define OPRT_SVC_SMART_GW_ERRCODE_MAX_CNT 4


/****************************************************************************
            the error code marco define for module SVC_DEVOS 
****************************************************************************/
#define OPRT_SVC_DEVOS_NOT_EXISTS                          (-0x1c00)  //不存在
#define OPRT_SVC_DEVOS_ERRCODE_MAX_CNT 1


/****************************************************************************
            the error code marco define for module BASE_DB 
****************************************************************************/
#define OPRT_BASE_DB_FLASH_NOT_ENOUGH_PAGE                 (-0x1d00)  //flash页不够
#define OPRT_BASE_DB_ERRCODE_MAX_CNT 1


#define TUYA_CHECK_NULL_RETURN(x, y)\
do{\
    if (NULL == (x)){\
        PR_ERR("%s null", #x);\
        return (y);\
    }\
}while(0)


#define TUYA_CHECK_NULL_GOTO(x, label)\
do{\
    if (NULL == (x)){\
        PR_ERR("%s null", #x);\
        goto label;\
    }\
}while(0)


#define TUYA_CALL_ERR_LOG(func)\
do{\
    rt = (func);\
    if (OPRT_OK != (rt))\
        PR_ERR("ret:%d", rt);\
}while(0)


#define TUYA_CALL_ERR_GOTO(func, label)\
do{\
    rt = (func);\
    if (OPRT_OK != (rt)){\
        PR_ERR("ret:%d", rt);\
        goto label;\
    }\
}while(0)


#define TUYA_CALL_ERR_RETURN(func)\
do{\
    rt = (func);\
    if (OPRT_OK != (rt)){\
       PR_ERR("ret:%d", rt);\
       return (rt);\
    }\
}while(0)


#define TUYA_CALL_ERR_RETURN_VAL(func, y)\
do{\
    rt = (func);\
    if (OPRT_OK != (rt)){\
        PR_ERR("ret:%d", rt);\
        return (y);\
    }\
}while(0)


#define TUYA_CALL_ERR_LOG_SEQ_RETURN_VAL(func, y, point)\
do{\
    rt = (func);\
    if (OPRT_OK != (rt)){\
        PR_ERR("ret:%d", rt);\
        INSERT_ERROR_LOG_SEQ_DEC((point), rt);\
        return (y);\
    }\
}while(0)


#define TUYA_CALL_ERR_LOG_SEQ_RETURN(func, point)\
do{\
    rt = (func);\
    if (OPRT_OK != (rt)){\
        PR_ERR("ret:%d", rt);\
        INSERT_ERROR_LOG_SEQ_DEC((point), rt);\
        return (rt);\
    }\
}while(0)


#define TUYA_CALL_ERR_LOG_SEQ_GOTO(func, label)\
do{\
    rt = (func);\
    if (OPRT_OK != (rt)){\
        PR_ERR("ret:%d", rt);\
        INSERT_ERROR_LOG_SEQ_DEC((point), rt);\
        goto label;\
    }\
}while(0)


#define TUYA_CALL_ERR_LOG_SEQ(func)\
do{\
    rt = (func);\
    if (OPRT_OK != (rt)) {\
        PR_ERR("ret:%d", rt);\
        INSERT_ERROR_LOG_SEQ_DEC((point), rt);\
    }\
}while(0)


#define TUYA_CHECK_NULL_LOG_SEQ_RETURN(x, y, point)\
do{\
    if (NULL == (x)){\
        PR_ERR("%s null", #x);\
        INSERT_ERROR_LOG_SEQ_DEC((point), y);\
        return (y);\
    }\
}while(0)


#define TUYA_CHECK_NULL_LOG_SEQ_GOTO(x, point, label)\
do{\
    if (NULL == (x)){\
        PR_ERR("%s null", #x);\
        INSERT_ERROR_LOG_SEQ_NULL((point));\
        goto label;\
    }\
}while(0)


#ifdef __cplusplus
}
#endif
#endif
