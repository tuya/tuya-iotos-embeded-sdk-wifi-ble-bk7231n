/***********************************************************
*  File: uni_base64.h 
*  Author: nzy
*  Date: 20150609
***********************************************************/
#ifndef _UNI_BASE64_H
#define _UNI_BASE64_H

#ifdef __cplusplus
	extern "C" {
#endif

char * tuya_base64_encode( const unsigned char * bindata, char * base64, int binlength );

int tuya_base64_decode( const char * base64, unsigned char * bindata );

#ifdef __cplusplus
}
#endif
#endif

