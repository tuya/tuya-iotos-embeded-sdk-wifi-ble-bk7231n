/***********************************************************
*  File: uf_file.h 
*  Author: lql
*  Date: 20181213
***********************************************************/
#ifndef __UF_FILE_H
#define __UF_FILE_H
#include "tuya_cloud_types.h"

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef  __UF_FILE_GLOBALS
    #define __UF_FILE_EXT
#else
    #define __UF_FILE_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef VOID uFILE;

typedef unsigned char umode_t;
#define UF_SEEK_SET 0
#define UF_SEEK_CUR 1
#define UF_SEEK_END 2

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/

/***********************************************************
*  Function: init uf mode
*  Input:  none
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__UF_FILE_EXT \
OPERATE_RET ufinit(VOID);

/***********************************************************
*  Function: fopen
*  Input: filepath mode
*  Output: none
*  Return: none
***********************************************************/
__UF_FILE_EXT \
uFILE* ufopen(IN CONST CHAR_T *filepath, IN CONST CHAR_T *mode);

/***********************************************************
*  Function: fclose
*  Input: fd
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__UF_FILE_EXT \
OPERATE_RET ufclose(IN uFILE *fd);

/***********************************************************
*  Function: fdelete
*  Input: filepath
*  Output: none
*  Return: OPERATE_RET
***********************************************************/
__UF_FILE_EXT \
OPERATE_RET ufdelete(IN CONST CHAR_T *filepath);

/***********************************************************
*  Function: fwrite
*  Input: fd buf len
*  Output: none
*  Return: UINT_T
***********************************************************/
__UF_FILE_EXT \
UINT_T ufwrite(IN uFILE *fd, IN CONST UCHAR_T *buf,IN CONST UINT_T len);

/***********************************************************
*  Function: fread
*  Input: fd buf len
*  Output: none
*  Return: UINT_T
***********************************************************/
__UF_FILE_EXT \
UINT_T ufread(IN uFILE *fd, IN UCHAR_T *buf,IN CONST UINT_T len);

/***********************************************************
*  Function: fseek
*  Input: fd offset uf_mode
*  Output: none
*  Return: INT_T
***********************************************************/
__UF_FILE_EXT \
INT_T ufseek(IN uFILE *fd, IN UINT_T offset,IN CONST umode_t uf_mode);

/***********************************************************
*  Function: ftell
*  Input: fd 
*  Output: none
*  Return: UINT_T
***********************************************************/
__UF_FILE_EXT \
INT_T uftell(IN uFILE *fd);

/***********************************************************
*  Function: feof
*  Input: fd
*  Output: none
*  Return: BOOL_T
***********************************************************/
__UF_FILE_EXT \
BOOL_T ufeof(IN uFILE *fd);

/***********************************************************
*  Function: fgetsize
*  Input: filepath
*  Output: none
*  Return: INT_T
***********************************************************/
__UF_FILE_EXT \
INT_T ufgetsize(IN CONST CHAR_T *filepath);

/***********************************************************
*  Function: ufprintf
*  Input: stream pFmt
*  Output: none
*  Return: INT_T
***********************************************************/
__UF_FILE_EXT \
INT_T ufprintf(IN uFILE * stream,IN const char* pFmt,...);

/***********************************************************
*  Function: ufscanf
*  Input: stream count pFmt
*  Output: none
*  Return: INT_T
***********************************************************/
__UF_FILE_EXT \
INT_T ufscanf(IN uFILE * stream,IN INT_T count,IN PCHAR_T pFmt,...);

/***********************************************************
*  Function: ufexist
*  Input: filepath
*  Output: none
*  Return: BOOL_T
***********************************************************/
__UF_FILE_EXT \
BOOL_T ufexist(IN CONST CHAR_T *filepath);

/***********************************************************
*  Function: ufaccess
*  Input: filepath mode
*  Output: none
*  Return: INT_T
***********************************************************/
INT_T ufaccess(IN CONST CHAR_T *filepath,IN INT_T mode);

/***********************************************************
*  Function: fflush
*  Input: stream
*  Output: none
*  Return: INT_T
***********************************************************/
__UF_FILE_EXT \
INT_T ufflush(IN uFILE * stream);

/***********************************************************
*  Function: ufsync
*  Input: fd
*  Output: none
*  Return: INT_T
***********************************************************/
__UF_FILE_EXT \
INT_T ufsync(IN INT_T fd);

/***********************************************************
*  Function: ufileno
*  Input: fd
*  Output: none
*  Return: INT_T
***********************************************************/
__UF_FILE_EXT \
INT_T ufileno(IN uFILE * stream);

/***********************************************************
*  Function: ufgetc
*  Input: stream
*  Output: none
*  Return: INT_T
***********************************************************/
__UF_FILE_EXT \
INT_T ufgetc(IN uFILE *stream);

/***********************************************************
*  Function: ufgets
*  Input: s , n , stream
*  Output: none
*  Return: CHAR_T *
***********************************************************/
__UF_FILE_EXT \
CHAR_T *ufgets(IN CHAR_T *s,IN INT_T n, IN uFILE *stream);

#ifdef __cplusplus
}

#endif
#endif
