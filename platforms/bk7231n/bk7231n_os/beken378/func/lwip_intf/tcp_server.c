/**
 ******************************************************************************
 * @file    tcp_server.c
 * @version V1.0.0
 * @date    21-May-2015
 * @brief   tcp server Demo
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2014 BEKEN Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 */

#include "include.h"
#include "rtos_pub.h"
#include "rtos_error.h"
#include "lwip/sockets.h"
#include "mem_pub.h"
#include "str_pub.h"

#define tcp_server_log(M, ...) os_printf("TCP", M, ##__VA_ARGS__)

#define SERVER_PORT            20000 /*set up a tcp server,port at 20000*/

int my_fd = -1; 


int unw_recv(const int fd, void *buf, u32 nbytes)
{
    fd_set readfds, errfds;
    int ret = 0;   

    if( fd < 0 ) 
    {        
        return -1;//UNW_FAIL;
    } 

    FD_ZERO( &readfds );
    FD_ZERO( &errfds ); 
    FD_SET( fd, &readfds );
    FD_SET( fd, &errfds );

    ret = select( fd+1, &readfds, NULL, &errfds, NULL);
    os_printf("select ret:%d, %d, %d\r\n", ret, FD_ISSET( fd, &readfds ), FD_ISSET( fd, &errfds ));

    if(ret > 0 && FD_ISSET( fd, &readfds ))
        return recv(fd,buf,nbytes,0); 
    else
        return -1;//UNW_FAIL;
    
}

void tcp_client_thread( beken_thread_arg_t arg )
{
    OSStatus err = kNoErr;
    int fd = (int) arg;
    int len = 0;
    fd_set readfds, errfds, readfds2; 
    char *buf = NULL;

    my_fd = fd;

    buf = (char*) os_malloc( 1024 );
    ASSERT(buf);
    
    #if CFG_USE_TUYA_CCA_TEST
    for(int i=0; i<1024; i++)
    {
        buf[i] = i & 0xff;
    }
    #endif
    
    while ( 1 )
    {
           
        {
            #if !CFG_USE_TUYA_CCA_TEST
            len = recv( fd, buf, 1024, 0 );

            if ( len <= 0 )
            {
                os_printf( "TCP Client is disconnected, fd: %d", fd );
                goto exit;
            }
            #else
            len = 1024;
            #endif

            len = send( fd, buf, len, 0 );

            #if CFG_USE_TUYA_CCA_TEST
            rtos_delay_milliseconds(10);
            #endif
        }
    }
	
exit:
    if ( err != kNoErr ) 
		tcp_server_log( "TCP client thread exit with err: %d", err );
	
    if ( buf != NULL ) 
		os_free( buf );
	
    close( fd );
    rtos_delete_thread( NULL );
}

volatile u8 test_flag = 0;
void close_tcp_client(void)
{
    os_printf("close_tcp_client:%d, %p\r\n", my_fd, rtos_get_current_thread());
    test_flag = 1;
    close( my_fd );
    my_fd = -1;
}

/* TCP server listener thread */
void tcp_server_thread( beken_thread_arg_t arg )
{
    (void)( arg );
    OSStatus err = kNoErr;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sockaddr_t_size = sizeof(client_addr);
    char client_ip_str[16];
    int tcp_listen_fd = -1, client_fd = -1;
    fd_set readfds;

    tcp_listen_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;/* Accept conenction request on all network interface */
    server_addr.sin_port = htons( SERVER_PORT );/* Server listen on port: 20000 */
    err = bind( tcp_listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr) );
    
    err = listen( tcp_listen_fd, 0 );
    
    while ( 1 )
    {
        FD_ZERO( &readfds );
        FD_SET( tcp_listen_fd, &readfds );

        select( tcp_listen_fd + 1, &readfds, NULL, NULL, NULL);

        if ( FD_ISSET( tcp_listen_fd, &readfds ) )
        {
            client_fd = accept( tcp_listen_fd, (struct sockaddr *) &client_addr, &sockaddr_t_size );
            if ( client_fd >= 0 )
            {
                os_strcpy( client_ip_str, inet_ntoa( client_addr.sin_addr ) );
                tcp_server_log( "TCP Client %s:%d connected, fd: %d", client_ip_str, client_addr.sin_port, client_fd );
                if ( kNoErr
                     != rtos_create_thread( NULL, BEKEN_APPLICATION_PRIORITY, 
							                     "TCP Clients",
                                                 (beken_thread_function_t)tcp_client_thread,
                                                 0x800, 
                                                 (beken_thread_arg_t)client_fd ) ) 
                {
                    close( client_fd );
					client_fd = -1;
                }
            }
        }
    }
	
    if ( err != kNoErr ) 
		tcp_server_log( "Server listerner thread exit with err: %d", err );
	
    close( tcp_listen_fd );
    rtos_delete_thread( NULL );
}

void make_tcp_server_command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    OSStatus err = kNoErr;

    err = rtos_create_thread( NULL, BEKEN_APPLICATION_PRIORITY, 
									"TCP_server", 
									(beken_thread_function_t)tcp_server_thread,
									0x800,
									(beken_thread_arg_t)0 );
    if(err != kNoErr)
    {
       os_printf("create \"TCP_server\" thread failed!\r\n");
    }
}

#if CFG_USE_TUYA_CCA_TEST
#include "udp_server_demo.h"
#include "include.h"

#include "rtos_pub.h"
#include "doubly_list.h"
#include "mem_pub.h"
#include "str_pub.h"
#include "lwip/sockets.h"
#include "BkDriverUart.h"
#include "Error.h"
#include "uart_pub.h"
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"
#include "lwip/inet.h"

#define UDP_LISTENER_PORT 55000

char * gbuf;
int udp_listen_fd = -1;
int client_udp_flag = 0;

void show_udp_recv(char *buf, unsigned int len)
{
    while(len--)
    {
        os_printf("%02x", *buf++);
    }
    os_printf("\r\n");
}

void udp_client_thread( beken_thread_arg_t arg )
{
    OSStatus err = kNoErr;
    struct sockaddr_in client_addr;
    int len = 0;
    fd_set readfds, errfds, readfds2; 
    char *buf = NULL;

    buf = gbuf;
    os_memcpy(&client_addr, arg, sizeof(struct sockaddr_in));
    
    for(int i=0; i<1024; i++)
    {
        buf[i] = i & 0xff;
    }

    
    while ( 1 )
    {
           
        {
            #if 0
            len = recv( fd, buf, 1024, 0 );

            if ( len <= 0 )
            {
                os_printf( "TCP Client is disconnected, fd: %d", fd );
                goto exit;
            }
            #else
            len = 1024;
            #endif

            len = sendto( udp_listen_fd, buf, len, 0 , (struct sockaddr *)&client_addr, sizeof(client_addr));

            rtos_delay_milliseconds(10);
        }
    }
	
exit:

    client_udp_flag = 0;
    rtos_delete_thread( NULL );
}


/* TCP server listener thread */
void udp_server_thread( beken_thread_arg_t arg )
{

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    OSStatus err = kNoErr;
    char *buf = NULL;
    int len = 0;
    fd_set readfds;

    buf = (char *) os_malloc( 1024 );
    if(buf == NULL)
    {
        os_printf("buf == NULL\r\n");
        goto exit;
    }
    gbuf = buf;

    udp_listen_fd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );//Make UDP socket
    if(udp_listen_fd == -1)
    {
        os_printf("udp_listen_fd == -1\r\n");
        goto exit;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(UDP_LISTENER_PORT);

    err = bind( udp_listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr) );

    while ( 1 )
    {
        FD_ZERO( &readfds );
        FD_SET( udp_listen_fd, &readfds );

        select( udp_listen_fd + 1, &readfds, NULL, NULL, NULL);

        if ( FD_ISSET( udp_listen_fd, &readfds ) )
        {
            if ( FD_ISSET( udp_listen_fd, &readfds ) ) /*one client has data*/
            {
                len = recvfrom( udp_listen_fd, buf, 1024, 0 , (struct sockaddr *) &client_addr, &client_addr_len);
                #if 0
                //show_udp_recv(buf, len);
                if(len)
                {
                    len = sendto( udp_listen_fd, buf, len, 0 , (struct sockaddr *) &client_addr, sizeof(client_addr));
                }
                #else
                if(client_udp_flag == 1)
                    continue;

                client_udp_flag = 1;
                if ( kNoErr
                     != rtos_create_thread( NULL, BEKEN_APPLICATION_PRIORITY, 
							                     "UDP Clients",
                                                 (beken_thread_function_t)udp_client_thread,
                                                 0x800, 
                                                 (beken_thread_arg_t)&client_addr ) ) 
                {
                    client_udp_flag = 0;
                }
                
                #endif
            }
        }
    }

exit:
    if ( err != kNoErr )
    {
        os_printf( "Server listener thread exit with err: %d", err );
    }

    if(udp_listen_fd)
    {
        close( udp_listen_fd );
    }

    if(buf)
    {
        os_free(buf);
    }

    rtos_delete_thread( NULL );
}


int demo_start_upd( void )
{
    OSStatus err = kNoErr;

    err = rtos_create_thread( NULL, BEKEN_APPLICATION_PRIORITY,
                              "udp_server",
                              (beken_thread_function_t)udp_server_thread,
                              0x800,
                              (beken_thread_arg_t)0 );
    return err;
}



#endif

