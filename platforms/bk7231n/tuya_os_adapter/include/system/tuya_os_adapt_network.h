/**
 * @file tuya_os_adapt_network.h
 * @brief 网络操作接口
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */


#ifndef __TUYA_OS_ADAPT_NETWORK_H__
#define __TUYA_OS_ADAPT_NETWORK_H__


#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include "tuya_os_adapter.h"

#ifdef __cplusplus
    extern "C" {
#endif

/* tuya sdk definition of shutdown type */
#define UNW_SHUT_RD   0
#define UNW_SHUT_WR   1
#define UNW_SHUT_RDWR 2

#define UNW_FD_SET(n,p)     tuya_os_adapt_net_fd_set(n, p)
#define UNW_FD_CLR(n, p)    tuya_os_adapt_net_fd_clear(n, p)
#define UNW_FD_ISSET(n,p)   tuya_os_adapt_net_fd_isset(n,p)
#define UNW_FD_ZERO(p)      tuya_os_adapt_net_fd_zero(p)

/**
 * @brief 用于获取错误序号
 * 
 * @retval         errno
 */
UNW_ERRNO_T tuya_os_adapt_net_get_errno(void);

/**
 * @brief : 用于ip字符串数据转换网络序ip地址(4B)
 * @param[in]      ip    ip字符串    "192.168.1.1"
 * @return  ip地址(4B)
 */
UNW_IP_ADDR_T tuya_os_adapt_net_addr(const signed char *ip);

/**
 * @brief : Ascii网络字符串地址转换为主机序(4B)地址 
 * @param[in]            ip_str
 * @return   主机序ip地址(4B)
*/
UNW_IP_ADDR_T tuya_os_adapt_net_str2addr(signed char *ip);

/**
 * @brief : set fds
 * @param[in]      fd
 * @param[inout]      fds
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_fd_set(const int fd, UNW_FD_SET_T* fds);

/**
 * @brief : clear fds
 * @param[in]      fd
 * @param[inout]      fds
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_fd_clear(const int fd, UNW_FD_SET_T* fds);

/**
 * @brief : 判断fds是否被置位
 * @param[in]      fd
 * @param[in]      fds
 * @return  0-没有可读fd other-有可读fd
 */
int tuya_os_adapt_net_fd_isset(const int fd, UNW_FD_SET_T* fds);

/**
 * @brief : init fds
 * @param[inout]      fds
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_fd_zero(UNW_FD_SET_T* fds);

/**
 * @brief : select
 * @param[in]         maxfd
 * @param[inout]      readfds
 * @param[inout]      writefds
 * @param[inout]      errorfds
 * @param[inout]      ms_timeout
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_select(const int maxfd, UNW_FD_SET_T *readfds, UNW_FD_SET_T *writefds, UNW_FD_SET_T *errorfds, const unsigned int ms_timeout);

/**
 * @brief : close fd
 * @param[in]      fd
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_close(const int fd);

/**
 * @brief : shutdow fd
 * @param[in]      fd
 * @param[in]      how
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_shutdown(const int fd, const int how);

/**
 * @brief : creat fd
 * @param[in]      type
 * @return  >=0: socketfd  <0: fail
 */
int tuya_os_adapt_net_socket_create(const UNW_PROTOCOL_TYPE type);

/**
 * @brief : connect
 * @param[in]      fd
 * @param[in]      addr
 * @param[in]      port
 * @return  0: success  Other: fail
 */
int tuya_os_adapt_net_connect(const int fd, const UNW_IP_ADDR_T addr, const unsigned short port);

/**
 * @brief : raw connect
 * @param[in]      fd
 * @param[in]      p_socket
 * @param[in]      len
 * @return  0: success  Other: fail
 */
int tuya_os_adapt_net_connect_raw(const int fd, void *p_socket_addr, const int len);

/**
 * @brief : bind
 * @param[in]      fd
 * @param[in]      addr
 * @param[in]      port
 * @return  0: success  Other: fail
 */
int tuya_os_adapt_net_bind(const int fd, const UNW_IP_ADDR_T addr, const unsigned short port);

/**
 * @brief : bind ip
 * @param[in]            fd
 * @param[inout]         addr
 * @param[inout]         port
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_socket_bind(const int fd, const char *ip);

/**
 * @brief : listen
 * @param[in]      fd
 * @param[in]      backlog
 * @return  0: success  < 0: fail
 */
int tuya_os_adapt_net_listen(const int fd, const int backlog);

/**
 * @brief : accept
 * @param[in]            fd
 * @param[inout]         addr
 * @param[inout]         port
 * @return  >=0: 新接收到的socketfd  others: fail
 */
int tuya_os_adapt_net_accept(const int fd, UNW_IP_ADDR_T *addr, unsigned short *port);

/**
 * @brief : send
 * @param[in]      fd
 * @param[in]      buf
 * @param[in]      nbytes
 * @return  nbytes has sended
 */
int tuya_os_adapt_net_send(const int fd, const void *buf, const unsigned int nbytes);

/**
 * @brief : send to
 * @param[in]      fd
 * @param[in]      buf
 * @param[in]      nbytes
 * @param[in]      addr
 * @param[in]      port
 * @return  nbytes has sended
 */
int tuya_os_adapt_net_send_to(const int fd, const void *buf, const unsigned int nbytes,\
                         const UNW_IP_ADDR_T addr,const unsigned short port);

/**
 * @brief : recv
 * @param[in]         fd
 * @param[inout]      buf
 * @param[in]         nbytes
 * @return  nbytes has received
 */
int tuya_os_adapt_net_recv(const int fd, void *buf, const unsigned int nbytes);

/**
 * @brief : Receive enough data to specify
 * @param[in]            fd
 * @param[inout]         buf
 * @param[in]            buf_size
 * @param[in]            nd_size
 * @return  nbytes has received
 */
int tuya_os_adapt_net_recv_nd_size(const int fd, \
                              void *buf, \
                              const unsigned int buf_size, \
                              const unsigned int nd_size);

/**
 * @brief : recvfrom
 * @param[in]         fd
 * @param[inout]      buf
 * @param[in]         nbytes
 * @param[inout]         addr
 * @param[inout]         port
 * @return  nbytes has received
 */
int tuya_os_adapt_net_recvfrom(const int fd, \
                          void *buf, \
                          const unsigned int nbytes,\
                          UNW_IP_ADDR_T *addr, \
                          unsigned short *port);

/**
 * @brief : get fd block info
 * @param[in]      fd
 * @return  <0-失败   >0-非阻塞    0-阻塞
 */
int tuya_os_adapt_net_get_nonblock(const int fd);

/**
 * @brief : set block block or not
 * @param[in]      fd
 * @param[in]      block
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_set_block(const int fd, const bool_t block);

/**
 * @brief : set timeout
 * @param[in]         fd
 * @param[in]         ms_timeout
 * @param[in]         type
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_set_timeout(const int fd, \
                           const int ms_timeout,\
                           const UNW_TRANS_TYPE_E type);

/**
 * @brief : set buf size
 * @param[in]         fd
 * @param[in]         buf_size
 * @param[in]         type
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_set_bufsize(const int fd, \
                            const int buf_size,\
                            const UNW_TRANS_TYPE_E type);

/**
 * @brief : set reuse
 * @param[in]         fd
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_set_reuse(const int fd);

/**
 * @brief : disable nagle
 * @param[in]         fd
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_disable_nagle(const int fd);

/**
 * @brief : set broadcast
 * @param[in]         fd
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_set_boardcast(const int fd);

/**
 * @brief : tcp保活设置 
 * @param[in]            fd-the socket fd
 * @param[in]            alive-open(1) or close(0) 
 * @param[in]            idle-how long to send a alive packet(in seconds) 
 * @param[in]            intr-time between send alive packets (in seconds)
 * @param[in]            cnt-keep alive packets fail times to close the connection
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_set_keepalive(int fd, const bool_t alive, const unsigned int idle, const unsigned int intr, const unsigned int cnt);


/**
 * @brief : dns parse
 * @param[in]            domain
 * @param[inout]         addr
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_gethostbyname(const char *domain, UNW_IP_ADDR_T *addr);

/* add begin: by sunkz, interface regist */
OPERATE_RET tuya_os_adapt_reg_network_intf(void);
/* add end */


#ifdef __cplusplus
}
#endif

#endif // __TUYA_OS_ADAPT_NETWORK_H__

