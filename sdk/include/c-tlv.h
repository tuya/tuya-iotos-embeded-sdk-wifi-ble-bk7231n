#ifndef __C_TLV_H__
#define __C_TLV_H__

#include "tuya_cloud_types.h"

#ifdef __cplusplus
    extern "C" {
#endif

/* 强转成某一类型的指针 */
#define UINT8(ptr)					((UINT8_T *)(ptr))
#define INT8(ptr)					((INT8_T *)(ptr))

#define UINT16(ptr)					((UINT16_T *)(ptr))
#define INT16(ptr)					((INT16_T *)(ptr))

#define UINT32(ptr)					((UINT_T *)(ptr))
#define INT32(ptr)					((INT_T *)(ptr))

/* 向指针赋值 */
#define PACKED						__attribute__((packed))
#define PUT8(ptr, c)				(*UINT8((ptr)) = (UINT8_T)(c))
#define PUT16(ptr, s)				(*UINT16((ptr)) = (UINT16_T)(s))
#define PUT32(ptr, l)				(*UINT32((ptr)) = (UINT_T)(l))

/* 从指针取值 */
#define GET_UINT8(ptr)				(*UINT8((ptr)))
#define GET_INT8(ptr)   			(*INT8((ptr)))

#define GET_UINT16(ptr)				(*UINT16((ptr)))
#define GET_INT16(ptr)				(*INT16((ptr)))

#define GET_UINT32(ptr)					(*UINT32((ptr)))
#define GET_INT32(ptr)					(*INT32((ptr)))

/* 指针向下移位 */
#define ADVANCE8(ptr)				ptr = (UINT8(UINT8((ptr)) + 1))
#define ADVANCE16(ptr)				ptr = (UINT8(UINT16((ptr)) + 1))
#define ADVANCE32(ptr)				ptr = (UINT8(UINT32((ptr)) + 1))

typedef enum {
	TLV_TYPE_INT8 	= 7,
	TLV_TYPE_UINT8 	= 8,
	TLV_TYPE_INT16	= 9,
	TLV_TYPE_UINT16	= 10,
	TLV_TYPE_INT32	= 11,
	TLV_TYPE_UINT32	= 12,
	TLV_TYPE_BYTES	= 13,
    TLV_TYPE_MSG	= 14, //复合嵌套结构
} tlv_type_t;

/* tlv/msg structures */
typedef struct {
    UINT16_T id;
    UINT16_T type;
    UINT16_T length;
    UINT8_T * value;
} PACKED tlv_t;

typedef struct {
    UINT_T magic;
    UINT_T nitems;
    UINT_T capacity;
	tlv_t tlvs[0];
} PACKED message_t;

#define MSG_MAGIC					(0x12345678)
#define MSG_AVAILABLE_TLVS(msg)		(((msg)->capacity) - ((msg)->nitems))
#define MSG_TLV(msg, index)			(&((msg)->tlvs[(index)]))

//最后一个无效的tlv元素
#define MSG_LAST_TLV(msg)			(MSG_TLV((msg), (msg)->nitems))

#define MSG_SIZE(msg)				((UINT16_T)(SIZEOF(message_t) + SIZEOF(tlv_t) * (msg)->capacity))

/* 构造一个新的msg */
message_t * msg_init(UINT_T max_items);
/* 释放msg */
VOID msg_free(message_t * msg);
/* 向msg中添加一个item */
tlv_t * msg_append(message_t * msg, tlv_type_t type, UINT16_T id, VOID * value, UINT16_T length);
/* 获取一个msg打包后的长度 */
UINT_T msg_get_packed_size(message_t * msg);
/* dump一个msg数据 */
VOID msg_print(message_t * msg);
/* 将一个打包后的数据流拆解成一个msg */
message_t * msg_unpack(UINT8_T * packed, UINT_T size);
/* 将一个msg打包成一个数据流
out是调用者预先分配的buffer，打包前，*out_size是buffer的长度，打包后，*out_size是打包后的数据流长度
*/
OPERATE_RET msg_pack(message_t * msg, UINT8_T * out, UINT_T * out_size);


#ifdef __cplusplus
}
#endif
#endif
