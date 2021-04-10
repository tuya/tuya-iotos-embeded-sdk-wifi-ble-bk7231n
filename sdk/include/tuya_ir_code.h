#include "tuya_cloud_types.h"

#ifndef __TUYA_IR_CODE_H__
#define __TUYA_IR_CODE_H__

VOID tuya_ir_clear_irtime(VOID);

BOOL_T tuya_ir_get_irtime(USHORT_T data[], USHORT_T data_len);

BOOL_T tuya_ir_encode_head(USHORT_T feq, USHORT_T data[], USHORT_T data_len, CHAR_T *ir_head, USHORT_T ir_head_size);
/* 编码 */
BOOL_T tuya_ir_encode(BYTE_T framecount, USHORT_T data[], USHORT_T length, CHAR_T *ir_head, USHORT_T ir_head_size, CHAR_T *ir_key, USHORT_T ir_key_size);

/* 解码 */
USHORT_T tuya_ir_decode(CHAR_T *head, CHAR_T *key, USHORT_T *decode_buf, USHORT_T buf_size, BYTE_T *framecount, USHORT_T *feq);


#endif
