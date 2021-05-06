/**
 * @file tuya_os_adapt_network.c
 * @brief 网络操作接口
 * 
 * @copyright Copyright(C),2018-2020, 涂鸦科技 www.tuya.com
 * 
 */
#include "tuya_os_adapt_system.h"
#include "tuya_os_adapt_mutex.h"
#include "tuya_os_adapter_error_code.h"

#include <assert.h>
#include "tuya_os_adapt_network.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/inet.h"

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef struct NETWORK_ERRNO_TRANS {
    int sys_err;
    int priv_err;
}NETWORK_ERRNO_TRANS_S;

#define UNW_TO_SYS_FD_SET(fds)  ((fd_set*)fds)

// 编译期结构体大小检查，如果此处编译错误，请加大 UNW_FD_MAX_COUNT 的值
char static_assert_impl_type[(sizeof(UNW_FD_SET_T)>=sizeof(fd_set))?1:-1]; // 编译期校验
/***********************************************************
*************************variable define********************
***********************************************************/
const NETWORK_ERRNO_TRANS_S unw_errno_trans[]= {
    {EINTR,UNW_EINTR},
    {EBADF,UNW_EBADF},
    {EAGAIN,UNW_EAGAIN},
    {EFAULT,UNW_EFAULT},
    {EBUSY,UNW_EBUSY},
    {EINVAL,UNW_EINVAL},
    {ENFILE,UNW_ENFILE},
    {EMFILE,UNW_EMFILE},
    {ENOSPC,UNW_ENOSPC},
    {EPIPE,UNW_EPIPE},
    {EWOULDBLOCK,UNW_EWOULDBLOCK},
    {ENOTSOCK,UNW_ENOTSOCK},
    {ENOPROTOOPT,UNW_ENOPROTOOPT},
    {EADDRINUSE,UNW_EADDRINUSE},
    {EADDRNOTAVAIL,UNW_EADDRNOTAVAIL},
    {ENETDOWN,UNW_ENETDOWN},
    {ENETUNREACH,UNW_ENETUNREACH},
    {ENETRESET,UNW_ENETRESET},
    {ECONNRESET,UNW_ECONNRESET},
    {ENOBUFS,UNW_ENOBUFS},
    {EISCONN,UNW_EISCONN},
    {ENOTCONN,UNW_ENOTCONN},
    {ETIMEDOUT,UNW_ETIMEDOUT},
    {ECONNREFUSED,UNW_ECONNREFUSED},
    {EHOSTDOWN,UNW_EHOSTDOWN},
    {EHOSTUNREACH,UNW_EHOSTUNREACH},
    {ENOMEM ,UNW_ENOMEM},
    {EMSGSIZE,UNW_EMSGSIZE}
};

static const TUYA_OS_NETWORK_INTF m_tuya_os_network_intfs = {
    .get_errno        =    tuya_os_adapt_net_get_errno,
    .fd_set1          =    tuya_os_adapt_net_fd_set,
    .fd_clear         =    tuya_os_adapt_net_fd_clear,
    .fd_isset         =    tuya_os_adapt_net_fd_isset,
    .fd_zero          =    tuya_os_adapt_net_fd_zero,
    .select           =    tuya_os_adapt_net_select,
    .get_nonblock     =    tuya_os_adapt_net_get_nonblock,
    .set_block        =    tuya_os_adapt_net_set_block,
    .close            =    tuya_os_adapt_net_close,
    .shutdown         =    tuya_os_adapt_net_shutdown,
    .socket_create    =    tuya_os_adapt_net_socket_create,
    .connect          =    tuya_os_adapt_net_connect,
    .connect_raw      =    tuya_os_adapt_net_connect_raw,
    .bind             =    tuya_os_adapt_net_bind,
    .listen           =    tuya_os_adapt_net_listen,
    .send             =    tuya_os_adapt_net_send,
    .send_to          =    tuya_os_adapt_net_send_to,
    .recv             =    tuya_os_adapt_net_recv,
    .recvfrom         =    tuya_os_adapt_net_recvfrom,
    .set_timeout      =    tuya_os_adapt_net_set_timeout,
    .set_bufsize      =    tuya_os_adapt_net_set_bufsize,
    .set_reuse        =    tuya_os_adapt_net_set_reuse,
    .disable_nagle    =    tuya_os_adapt_net_disable_nagle,
    .set_boardcast    =    tuya_os_adapt_net_set_boardcast,
    .gethostbyname    =    tuya_os_adapt_net_gethostbyname,
    .accept           =    tuya_os_adapt_net_accept,
    .recv_nd_size     =    tuya_os_adapt_net_recv_nd_size, 
    .str2addr         =    tuya_os_adapt_net_str2addr,
    .addr2str         =    NULL,
    .set_keepalive    =    tuya_os_adapt_net_set_keepalive,
    .socket_bind      =    tuya_os_adapt_net_socket_bind,
    .set_cloexec      =    NULL,
    .get_socket_ip    =    NULL,
    .addr             =    tuya_os_adapt_net_addr,
};

/***********************************************************
*************************function define********************
***********************************************************/
/**
 * @brief 用于获取错误序号
 * 
 * @retval         errno
 */
UNW_ERRNO_T tuya_os_adapt_net_get_errno(void)
{
    int i = 0;

    int sys_err = errno;

    for(i = 0; i < (int)sizeof(unw_errno_trans)/sizeof(unw_errno_trans[0]); i++) {
        if(unw_errno_trans[i].sys_err == sys_err) {
            return unw_errno_trans[i].priv_err;
        }
    }

    return -100 - sys_err;
}

/**
 * @brief : 用于ip字符串数据转换网络序ip地址(4B)
 * @param[in]      ip    ip字符串    "192.168.1.1"
 * @return  ip地址(4B)
 */
UNW_IP_ADDR_T tuya_os_adapt_net_addr(const signed char *ip)
{
    if(ip == NULL) {
        return 0xFFFFFFFF;
    }

    return inet_addr((char *)ip);
}

/**
 * @brief : Ascii网络字符串地址转换为主机序(4B)地址 
 * @param[in]            ip_str
 * @return   主机序ip地址(4B)
 */
UNW_IP_ADDR_T tuya_os_adapt_net_str2addr(signed char *ip)
{
    if(ip == NULL) {
        return 0xFFFFFFFF;
    }
    
    UNW_IP_ADDR_T addr1 = inet_addr((char *)ip);
    UNW_IP_ADDR_T addr2 = ntohl(addr1);

    return addr2;
}

/**
 * @brief : set fds
 * @param[in]      fd
 * @param[inout]      fds
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_fd_set(const int fd, UNW_FD_SET_T* fds)
{
    if((fd < 0) || (fds == NULL)) {
        return -3000 + fd;
    }

    FD_SET(fd, UNW_TO_SYS_FD_SET(fds));
    
    return UNW_SUCCESS;
}

/**
 * @brief : clear fds
 * @param[in]      fd
 * @param[inout]      fds
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_fd_clear(const int fd, UNW_FD_SET_T* fds)
{
    if((fd < 0) || (fds == NULL)) {
        return -3000 + fd;
    }
    
    FD_CLR(fd, UNW_TO_SYS_FD_SET(fds));

    return UNW_SUCCESS;
}

/**
 * @brief : 判断fds是否被置位
 * @param[in]      fd
 * @param[in]      fds
 * @return  0-没有可读fd other-有可读fd
 */
int tuya_os_adapt_net_fd_isset(const int fd, UNW_FD_SET_T* fds)
{
    if((fd < 0) || (fds == NULL)) {
        return -3000 + fd;
    }

    return FD_ISSET(fd, UNW_TO_SYS_FD_SET(fds));
}

/**
 * @brief : init fds
 * @param[inout]      fds
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_fd_zero(UNW_FD_SET_T* fds)
{
    if(fds == NULL) {
        return 0xFFFFFFFF;
    }

    FD_ZERO(UNW_TO_SYS_FD_SET(fds));

    return UNW_SUCCESS;
}

/**
 * @brief : select
 * @param[in]         maxfd
 * @param[inout]      readfds
 * @param[inout]      writefds
 * @param[inout]      errorfds
 * @param[inout]      ms_timeout
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_select(const int maxfd, UNW_FD_SET_T *readfds, UNW_FD_SET_T *writefds, UNW_FD_SET_T *errorfds, const unsigned int ms_timeout)
{
    if(maxfd <= 0) {
        return -3000 + maxfd;
    }

    struct timeval *tmp = NULL;
    struct timeval timeout = {ms_timeout/1000, (ms_timeout%1000)*1000};
    if(0 != ms_timeout) {
        tmp = &timeout;
    }else {
        tmp = NULL;
    }

    return select(maxfd, UNW_TO_SYS_FD_SET(readfds), UNW_TO_SYS_FD_SET(writefds), UNW_TO_SYS_FD_SET(errorfds), tmp);
}

/**
 * @brief : close fd
 * @param[in]      fd
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_close(const int fd)
{
    if(fd < 0) {
        return -3000 + fd;
    }

    return close(fd);
}

/**
 * @brief : shutdow fd
 * @param[in]      fd
 * @param[in]      how
 * @return  0: success  <0: fail
*/
int tuya_os_adapt_net_shutdown(const int fd, const int how)
{
    if(fd < 0) {
        return -3000 + fd;
    }

    return shutdown(fd,how);
}

/**
 * @brief : creat fd
 * @param[in]      type
 * @return  >=0: socketfd  <0: fail
*/
int tuya_os_adapt_net_socket_create(const UNW_PROTOCOL_TYPE type)
{
    int fd = -1;

    if(PROTOCOL_TCP == type) {
        fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    } else if (PROTOCOL_RAW == type) {
        fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    } else {
        fd = socket(AF_INET, SOCK_DGRAM, 0);
    }

    return fd;
}

/**
 * @brief : connect
 * @param[in]      fd
 * @param[in]      addr
 * @param[in]      port
 * @return  0: success  Other: fail
*/
int tuya_os_adapt_net_connect(const int fd, const UNW_IP_ADDR_T addr, const unsigned short port)
{
    if(fd < 0) {
        return -3000 + fd;
    }

    struct sockaddr_in sock_addr;
    unsigned short tmp_port = port;
    UNW_IP_ADDR_T tmp_addr = addr;

    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(tmp_port);
    sock_addr.sin_addr.s_addr = htonl(tmp_addr);

    return connect(fd, (struct sockaddr*)&sock_addr, sizeof(struct sockaddr_in));
}

/**
 * @brief : raw connect
 * @param[in]      fd
 * @param[in]      p_socket
 * @param[in]      len
 * @return  0: success  Other: fail
*/
int tuya_os_adapt_net_connect_raw(const int fd, void *p_socket_addr, const int len)
{
    if(fd < 0) {
        return -3000 + fd;
    }

    return connect(fd, (struct sockaddr *)p_socket_addr, len);
}

/**
 * @brief : bind
 * @param[in]      fd
 * @param[in]      addr
 * @param[in]      port
 * @return  0: success  Other: fail
*/
int tuya_os_adapt_net_bind(const int fd, const UNW_IP_ADDR_T addr, const unsigned short port)
{
    if( fd < 0 ) {
        return -3000 + fd;
    }

    unsigned short tmp_port = port;
    UNW_IP_ADDR_T tmp_addr = addr;

    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(tmp_port);
    sock_addr.sin_addr.s_addr = htonl(tmp_addr);

    return bind(fd,(struct sockaddr*)&sock_addr, sizeof(struct sockaddr_in));
}

/**
 * @brief : bind ip
 * @param[in]            fd
 * @param[inout]         addr
 * @param[inout]         port
 * @return  0: success  <0: fail
*/
int tuya_os_adapt_net_socket_bind(const int fd, const char *ip)
{
    if(NULL == ip) {
        return -3000;
    }

    struct sockaddr_in addr_client   = {0};
    addr_client.sin_family   = AF_INET;
    addr_client.sin_addr.s_addr      = inet_addr(ip);
    addr_client.sin_port     = 0;    /// 0 表示由系统自动分配端口号

    if (0 != bind(fd,(struct sockaddr*)&addr_client,sizeof(addr_client))) {
        return UNW_FAIL;
    }

    return UNW_SUCCESS;
}

/**
 * @brief : listen
 * @param[in]      fd
 * @param[in]      backlog
 * @return  0: success  < 0: fail
*/
int tuya_os_adapt_net_listen(const int fd, const int backlog)
{
    if(fd < 0) {
        return -3000 + fd;
    }

    return listen(fd,backlog);
}

/**
 * @brief : accept
 * @param[in]            fd
 * @param[inout]         addr
 * @param[inout]         port
 * @return  >=0: 新接收到的socketfd  others: fail
*/
int tuya_os_adapt_net_accept(const int fd, UNW_IP_ADDR_T *addr, unsigned short *port)
{
    if( fd < 0 ) {
        return -3000 + fd;
    }

    struct sockaddr_in sock_addr;
    socklen_t len = sizeof(struct sockaddr_in);
    int cfd = accept(fd, (struct sockaddr *)&sock_addr,&len);
    if(cfd < 0) {
        return UNW_FAIL;
    }

    if(addr) {
        *addr = ntohl((sock_addr.sin_addr.s_addr));
    }

    if(port) {
        *port = ntohs((sock_addr.sin_port));
    }

    return cfd;
}

/**
 * @brief : send
 * @param[in]      fd
 * @param[in]      buf
 * @param[in]      nbytes
 * @return  nbytes has sended
*/
int tuya_os_adapt_net_send(const int fd, const void *buf, const unsigned int nbytes)
{
    if((fd < 0) || (buf == NULL) || (nbytes == 0)) {
        return -3000 + fd;
    }

    return send(fd,buf,nbytes,0);
}

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
                         const UNW_IP_ADDR_T addr,const unsigned short port)
{
    if((fd < 0) || (buf == NULL) || (nbytes == 0)) {
        return -3000 + fd;
    }

    unsigned short tmp_port = port;
    UNW_IP_ADDR_T tmp_addr = addr;

    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(tmp_port);
    sock_addr.sin_addr.s_addr = htonl(tmp_addr);

    return sendto(fd,buf,nbytes,0,(struct sockaddr *)&sock_addr,sizeof(sock_addr));
}

/**
 * @brief : recv
 * @param[in]         fd
 * @param[inout]      buf
 * @param[in]         nbytes
 * @return  nbytes has received
 */
int tuya_os_adapt_net_recv(const int fd, void *buf, const unsigned int nbytes)
{
    if((fd < 0) || (buf == NULL) || (nbytes == 0)) {
        return -3000 + fd;
    }
    int flags = fcntl(fd, F_GETFL, 0);

    int noblock = flags & O_NONBLOCK;

    if(!noblock) {
        fd_set set;
        FD_ZERO(&set);
        FD_SET(fd, &set);
        if(select(fd + 1, &set, NULL, NULL, NULL) < 0){
            return -1;
        }
    }

    return recv(fd,buf,nbytes,0);
}

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
                              const unsigned int nd_size)
{
    if((fd < 0) || (NULL == buf) || (buf_size == 0) || \
       (nd_size == 0) || (buf_size < nd_size)) {
        return -3000 + fd;
    }


    unsigned int rd_size = 0;
    int ret = 0;

    while(rd_size < nd_size) {
        ret = recv(fd,((uint8_t *)buf+rd_size),nd_size-rd_size,0);
        if(ret <= 0) {
            UNW_ERRNO_T err = tuya_os_adapt_net_get_errno();
            if(UNW_EWOULDBLOCK == err || \
               UNW_EINTR == err || \
               UNW_EAGAIN == err) {
                tuya_os_adapt_system_sleep(10);
                continue;
            }

            break;
        }

        rd_size += ret;
    }

    if(rd_size < nd_size) {
        return -2;
    }

    return rd_size;
}


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
                          unsigned short *port)
{
    if((fd < 0) || (buf == NULL) || (nbytes == 0)) {
        return -3000 + fd;
    }

    struct sockaddr_in sock_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    int ret = recvfrom(fd,buf,nbytes,0,(struct sockaddr *)&sock_addr,&addr_len);
    if(ret <= 0) {
        return ret;
    }

    if(addr) {
        *addr = ntohl(sock_addr.sin_addr.s_addr);
    }

    if(port) {
        *port = ntohs(sock_addr.sin_port);
    }

    return ret;
}

/**
 * @brief : set block block or not
 * @param[in]      fd
 * @param[in]      block
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_set_block(const int fd, const bool_t block)
{
    if(fd < 0) {
        return -3000 + fd;
    }

    int flags = fcntl(fd, F_GETFL, 0);
    if(block) {
        flags &= (~O_NONBLOCK);
    }else {
        flags |= O_NONBLOCK;
    }

    if (fcntl(fd,F_SETFL,flags) < 0) {
        return UNW_FAIL;
    }

    return UNW_SUCCESS;
}

/**
 * @brief : get fd block info
 * @param[in]      fd
 * @return  <0-失败   >0-非阻塞    0-阻塞
*/
int tuya_os_adapt_net_get_nonblock(const int fd)
{
    if(fd < 0) {
        return -3000 + fd;
    }


    if((fcntl(fd, F_GETFL, 0) & O_NONBLOCK) != O_NONBLOCK) {
        return 0;
    }

    if(errno == EAGAIN || errno == EWOULDBLOCK) {
        return 1;
    }

    return 0 ;
}

/**
 * @brief : set timeout
 * @param[in]         fd
 * @param[in]         ms_timeout
 * @param[in]         type
 * @return  0: success  <0: fail
*/
int tuya_os_adapt_net_set_timeout(const int fd, \
                             const int ms_timeout,\
                             const UNW_TRANS_TYPE_E type)

{
    if(fd < 0) {
        return -3000 + fd;
    }

    struct timeval timeout = {ms_timeout/1000, (ms_timeout%1000)*1000};
    int optname = ((type == TRANS_RECV) ? SO_RCVTIMEO:SO_SNDTIMEO);

    if(0 != setsockopt(fd, SOL_SOCKET, optname, (char *)&timeout, sizeof(timeout))) {
        return UNW_FAIL;
    }

    return UNW_SUCCESS;
}

/**
 * @brief : set buf size
 * @param[in]         fd
 * @param[in]         buf_size
 * @param[in]         type
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_set_bufsize(const int fd, \
                             const int buf_size,\
                             const UNW_TRANS_TYPE_E type)
{
    if( fd < 0 ) {
        return -3000 + fd;
    }

    int size = buf_size;
    int optname = ((type == TRANS_RECV) ? SO_RCVBUF:SO_SNDBUF);

    if(0 != setsockopt(fd, SOL_SOCKET, optname, (char *)&size, sizeof(size))) {
        return UNW_FAIL;
    }

    return UNW_SUCCESS;
}

/**
 * @brief : set reuse
 * @param[in]         fd
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_set_reuse(const int fd)
{
    if(fd < 0) {
        return -3000 + fd;
    }

    int flag = 1;
    if(0 != setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(const char*)&flag,sizeof(int))) {
        return UNW_FAIL;
    }

    return UNW_SUCCESS;
}

/**
 * @brief : disable nagle
 * @param[in]         fd
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_disable_nagle(const int fd)
{
    if(fd < 0) {
        return -3000 + fd;
    }

    int flag = 1;
    if(0 != setsockopt(fd,IPPROTO_TCP,TCP_NODELAY,(const char*)&flag,sizeof(int))) {
        return UNW_FAIL;
    }

    return UNW_SUCCESS;
}

/**
 * @brief : set broadcast
 * @param[in]         fd
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_set_boardcast(const int fd)
{
    if( fd < 0 ) {
        return -3000 + fd;
    }

    int flag = 1;
    if(0 != setsockopt(fd,SOL_SOCKET,SO_BROADCAST,(const char*)&flag,sizeof(int))) {
        return UNW_FAIL;
    }

    return UNW_SUCCESS;
}

/**
 * @brief : tcp保活设置 
 * @param[in]            fd-the socket fd
 * @param[in]            alive-open(1) or close(0) 
 * @param[in]            idle-how long to send a alive packet(in seconds) 
 * @param[in]            intr-time between send alive packets (in seconds)
 * @param[in]            cnt-keep alive packets fail times to close the connection
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_set_keepalive(int fd, const bool_t alive, const unsigned int idle, const unsigned int intr, const unsigned int cnt)
{
    if( fd < 0 ) {
        return -3000 + fd;
    }

    int ret = 0;
    int keepalive = alive;
    int keepidle = idle;
    int keepinterval = intr;
    int keepcount = cnt;

    ret |= setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive , sizeof(keepalive));
    ret |= setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, (void*)&keepidle , sizeof(keepidle));
    ret |= setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&keepinterval , sizeof(keepinterval));
    ret |= setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, (void *)&keepcount , sizeof(keepcount));
    if(0 != ret) {
        return UNW_FAIL;
    }

    return UNW_SUCCESS;
}

/**
 * @brief : dns parse
 * @param[in]            domain
 * @param[inout]         addr
 * @return  0: success  <0: fail
 */
int tuya_os_adapt_net_gethostbyname(const char *domain, UNW_IP_ADDR_T *addr)
{
    if((domain == NULL) || (addr == NULL)) {
        return OPRT_OS_ADAPTER_INVALID_PARM;
    }

    struct hostent* h = NULL;
    if ((h = gethostbyname(domain)) == NULL) {
        return UNW_FAIL;
    }

    *addr = ntohl(((struct in_addr *)(h->h_addr_list[0]))->s_addr);
    
    return UNW_SUCCESS;
}

OPERATE_RET tuya_os_adapt_reg_network_intf(void)
{
    return tuya_os_adapt_reg_intf(INTF_NETWORK, (void *)&m_tuya_os_network_intfs);
}

