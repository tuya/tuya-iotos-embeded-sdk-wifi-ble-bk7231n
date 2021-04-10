/**
 * @file tuya_hal_network.h
 * @author maht@tuya.com
 * @brief 网络接口封装文件
 * @version 0.1
 * @date 2019-08-15
 * 
 * @copyright Copyright (c) tuya.inc 2019
 * 
 */


#ifndef __TUYA_HAL_NETWORK_H__
#define __TUYA_HAL_NETWORK_H__

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include "tuya_os_adapter.h"

#ifdef __cplusplus
    extern "C" {
#endif

/***********************************************************
*  Function: unw_get_errno
*  Desc:     tuya sdk definition of errno
*  Return:   tuya sdk definition of socket errno
***********************************************************/
UNW_ERRNO_T tuya_hal_net_get_errno(void);

int tuya_hal_net_fd_set(int fd, UNW_FD_SET_T* fds);
int tuya_hal_net_fd_clear(int fd, UNW_FD_SET_T* fds);
int tuya_hal_net_fd_isset(int fd, UNW_FD_SET_T* fds);
int tuya_hal_net_fd_zero(UNW_FD_SET_T* fds);

#define UNW_FD_SET(n,p)     tuya_hal_net_fd_set(n, p)
#define UNW_FD_CLR(n, p)    tuya_hal_net_fd_clear(n, p)
#define UNW_FD_ISSET(n,p)   tuya_hal_net_fd_isset(n,p)
#define UNW_FD_ZERO(p)      tuya_hal_net_fd_zero(p)

/***********************************************************
*  Function: unw_select
*  Desc:     tuya sdk definition of socket select
*  Input && Output && Return: refer to std select
***********************************************************/
int tuya_hal_net_select(const int maxfd, UNW_FD_SET_T *readfds, UNW_FD_SET_T *writefds,
               UNW_FD_SET_T *errorfds, const UINT_T ms_timeout);

/***********************************************************
*  Function: unw_get_nonblock
*  Desc:     check where a socket fd is blocked
*  Input:    fd: socket fd
*  Return:   <0: fail  >0: non-block  0: block
***********************************************************/
int tuya_hal_net_get_nonblock(const int fd);

/***********************************************************
*  Function: unw_set_block
*  Desc:     set the socket fd to block/non-block state
*  Input:    fd: socket fd  block: the new state
*  Return:   UNW_SUCCESS: success   others: fail
***********************************************************/
int tuya_hal_net_set_block(const int fd, const bool block);

/***********************************************************
*  Function: unw_close
*  Desc:     tuya sdk definition of socket close
*  Input && Output && Return: refer to std close
***********************************************************/
int tuya_hal_net_close(const int fd);

/***********************************************************
*  Function: unw_shutdown
*  Desc:     tuya sdk definition of socket shutdown
*  Input && Output && Return: refer to std shutdown
***********************************************************/
int tuya_hal_net_shutdown(const int fd, const int how);

/***********************************************************
*  Function: unw_socket_create
*  Desc:     create a tcp/udp socket
*  Input:    type: tcp/udp type
*  Return: refer to std socket
***********************************************************/
int tuya_hal_net_socket_create(const UNW_PROTOCOL_TYPE type);

/***********************************************************
*  Function: unw_connect
*  Desc:     connect the socket fd to a addr and a port
*  Return:   refer to std connect
***********************************************************/
int tuya_hal_net_connect(const int fd, const UNW_IP_ADDR_T addr, const UINT16_T port);

/***********************************************************
*  Function: unw_connect_raw
*  Desc:     tuya sdk definition of socket connect
*  Input && Output && Return: refer to std connect
***********************************************************/
int tuya_hal_net_connect_raw(const int fd, void *p_socket, const int len);

/***********************************************************
*  Function: unw_bind
*  Desc:     bind the socket fd to a addr and a port
*  Return:   refer to std bind
***********************************************************/
int tuya_hal_net_bind(const int fd, const UNW_IP_ADDR_T addr, const UINT16_T port);

/***********************************************************
*  Function: unw_listen
*  Desc:     tuya sdk definition of socket listen
*  Input && Output && Return: refer to std listen
***********************************************************/
int tuya_hal_net_listen(const int fd, const int backlog);

/***********************************************************
*  Function: unw_send
*  Desc:     tuya sdk definition of socket send
*  Input && Output && Return: refer to std send
***********************************************************/
int tuya_hal_net_send(const int fd, const void *buf, const UINT_T nbytes);

/***********************************************************
*  Function: unw_send_to
*  Desc:     tuya sdk definition of socket sendto
*  Input && Output && Return: refer to std sendto
***********************************************************/
int tuya_hal_net_send_to(const int fd, const void *buf, const UINT_T nbytes,
                const UNW_IP_ADDR_T addr, const UINT16_T port);

/***********************************************************
*  Function: unw_recv
*  Desc:     tuya sdk definition of socket recv
*  Input && Output && Return: refer to std recv
***********************************************************/
int tuya_hal_net_recv(const int fd, void *buf, const UINT_T nbytes);

/***********************************************************
*  Function: unw_recvfrom
*  Desc:     tuya sdk definition of socket recvfrom
*  Input && Output && Return: refer to std recvfrom
***********************************************************/
int tuya_hal_net_recvfrom(const int fd, void *buf, const UINT_T nbytes,
                 UNW_IP_ADDR_T *addr, UINT16_T *port);

/***********************************************************
*  Function: unw_set_timeout
*  Desc:     set socket fd timeout option
*  Input:    fd: socket fd
*  Input:    ms_timeout: timeout in ms
*  Input:    type: transfer type
*  Return:   UNW_SUCCESS: success   others: fail
***********************************************************/
int tuya_hal_net_set_timeout(const int fd, const int ms_timeout,
                    const UNW_TRANS_TYPE_E type);

/***********************************************************
*  Function: unw_set_bufsize
*  Desc:     set socket fd buffer_size option
*  Input:    fd: socket fd
*  Input:    buf_size: buffer size in byte
*  Input:    type: transfer type
*  Return:   UNW_SUCCESS: success   others: fail
***********************************************************/
int tuya_hal_net_set_bufsize(const int fd, const int buf_size,
                    const UNW_TRANS_TYPE_E type);

/***********************************************************
*  Function: unw_set_reuse
*  Desc:     enable socket fd reuse option
*  Input:    fd: socket fd
*  Return:   UNW_SUCCESS: success   others: fail
***********************************************************/
int tuya_hal_net_set_reuse(const int fd);

/***********************************************************
*  Function: unw_disable_nagle
*  Desc:     disable socket fd nagle option
*  Input:    fd: socket fd
*  Return:   UNW_SUCCESS: success   others: fail
***********************************************************/
int tuya_hal_net_disable_nagle(const int fd);

/***********************************************************
*  Function: unw_set_boardcast
*  Desc:     enable socket broadcast option
*  Input:    fd: socket fd
*  Return:   UNW_SUCCESS: success   others: fail
***********************************************************/
int tuya_hal_net_set_boardcast(const int fd);

/***********************************************************
*  Function: unw_gethostbyname
*  Desc:     change the domain to addr info
*  Input:    domain: domin info
*  Output:   addr: addr info
*  Return:   UNW_SUCCESS: success   others: fail
***********************************************************/
int tuya_hal_net_gethostbyname(const char *domain, UNW_IP_ADDR_T *addr);

/***********************************************************
*  Function: unw_accept
*  Desc:     accept the coming socket connect of the server fd
*  Input:    fd: the server fd
*  Output:   addr && port: the coming addr info && port
*  Return:   >0: the coming socket fd.   others: fail
***********************************************************/
int tuya_hal_net_accept(const int fd, UNW_IP_ADDR_T *addr, UINT16_T *port);

/***********************************************************
*  Function: unw_recv_nd_size
*  Desc:     recv <nd_size> from the socket fd, store in the <buf>.
*  Input:    fd: the socket fd
*  Input:    buf && buf_size: the buffer info
*  Input:    nd_size: the need size
*  Output:   buf: the content recv from socket fd
*  Return:   >0: success   others: fail
***********************************************************/
int tuya_hal_net_recv_nd_size(const int fd, void *buf, 
                     const UINT_T buf_size, const UINT_T nd_size);

UNW_IP_ADDR_T tuya_hal_net_str2addr(signed char *ip_str);

/*
	convert Internet host address "ipaddr", given in host byte order, 
		TO a string in IPv4 numbers-and-dots(xx.xx.xx.xx) notion; 
*/
char* tuya_hal_net_addr2str(UNW_IP_ADDR_T ipaddr);

/***********************************************************
*  Function: unw_set_keepalive
*  Desc:     set the socket option:SO_KEEPALIVE TCP_KEEPIDLE TCP_KEEPINTVL TCP_KEEPCNT
*  Input:    fd: the socket fd
*  Input:    alive && idle && intr && cnt: options
*  Return:   UNW_SUCCESS: success   others: fail
***********************************************************/
int tuya_hal_net_set_keepalive(int fd, const bool alive,
                              const UINT_T idle, const UINT_T intr,
                              const UINT_T cnt);

/***********************************************************
*  Function: unw_socket_bind
*  Desc:     bind the socket fd to a ip
*  Input:    fd: the socket fd
*  Input:    ip: ip addr
*  Return:   UNW_SUCCESS: success   others: fail
***********************************************************/
int tuya_hal_net_socket_bind(int fd, const char *ip);

/***********************************************************
*  Function: unw_set_cloexec
*  Desc:     enable socket fd CLOEXEC
*  Input:    fd: the socket fd
*  Return:   UNW_SUCCESS: success   others: fail
***********************************************************/
int tuya_hal_net_set_cloexec(const int fd);

int tuya_hal_net_get_socket_ip(int fd, UNW_IP_ADDR_T *addr);
UNW_IP_ADDR_T tuya_hal_net_addr(const char *cp);


/* add begin: by sunkz, interface regist */
VOID_T tuya_os_network_intf_init(VOID_T);
/* add end */


#ifdef __cplusplus
}
#endif

#endif // __TUYA_HAL_NETWORK_H__

