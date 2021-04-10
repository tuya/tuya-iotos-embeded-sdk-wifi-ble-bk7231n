 /*============================================================================
 *                                                                            *
 * Copyright (C) by Tuya Inc                                                  *
 * All rights reserved                                                        *
 *                                                                            *
 =============================================================================*/

#ifndef __TUYA_SPI_H__
#define __TUYA_SPI_H__
/*============================ INCLUDES ======================================*/
#include "tuya_driver.h"

/*============================ MACROS ========================================*/
#define TUYA_SPI_CPHA             (1 << 0)
#define TUYA_SPI_CPOL             (1 << 1)
#define TUYA_SPI_MODE_MASK        (TUYA_SPI_CPHA | TUYA_SPI_CPOL)

/*============================ TYPES =========================================*/
typedef enum {
    TUYA_SPI0  = 1,                  
    TUYA_SPI1,          
} tuya_spi_port_t;

typedef enum {
    TUYA_SPI_MODE0  = (0 | 0),                          //! CPOL = 0, CPHA = 0 
    TUYA_SPI_MODE1  = (0 | TUYA_SPI_CPHA),              //! CPOL = 0, CPHA = 1 
    TUYA_SPI_MODE2  = (TUYA_SPI_CPOL | 0),              //! CPOL = 1, CPHA = 0 
    TUYA_SPI_MODE3  = (TUYA_SPI_CPOL | TUYA_SPI_CPHA)   //! CPOL = 1, CPHA = 1 
} tuya_spi_mode_t;

typedef enum {
    TUYA_SPI_DATA_BIT8  = 0,                     
    TUYA_SPI_DATA_BIT16,           
} tuya_spi_databits_t;

typedef enum {
    TUYA_SPI_CS_MODE_AUTO  = 0,           //! hardware auto set            
    TUYA_SPI_CS_MODE_SOFT,                //! software manual set
} tuya_spi_cs_mode_t;

typedef struct {
    uint8_t         cs_pin;
} tuya_spi_cfg_t;

typedef struct {
    void            *send_buf;
    void            *recv_buf;
    uint32_t         length;
} tuya_spi_msg_t;

typedef struct {
    uint8_t                   flag;
    tuya_spi_mode_t           mode;
    tuya_spi_databits_t       databits;
    tuya_spi_cs_mode_t        cs_mode;
    uint32_t                  freq_hz;
} tuya_spi_bus_cfg_t;

typedef struct tuya_spi_bus tuya_spi_bus_t;

typedef struct {
    int (*init)         (tuya_spi_bus_t *spi, tuya_spi_bus_cfg_t *cfg);
    int (*xfer)         (tuya_spi_bus_t *spi, tuya_spi_msg_t *msg);
    int (*control)      (tuya_spi_bus_t *spi, uint8_t cmd, void *arg);
    int (*deinit)       (tuya_spi_bus_t *spi);
} tuya_spi_ops_t;

typedef struct tuya_spi_bus {
    tuya_drv_node_t         node;
    tuya_spi_bus_cfg_t      cfg;
    MUTEX_HANDLE            lock;
    uint8_t                 ref_count;
    tuya_spi_ops_t         *ops;
};

typedef struct {
    tuya_spi_cfg_t            cfg;    
    tuya_spi_bus_t            *bus;
} tuya_spi_t;

/*============================ PROTOTYPES ====================================*/
int tuya_spi_bus_init        (tuya_spi_bus_t *spi_bus);
int tuya_spi_bus_add_device  (tuya_spi_bus_t *spi_bus, tuya_spi_t **spi, tuya_spi_cfg_t  *cfg);

int tuya_spi_xfer            (tuya_spi_t  *spi, void *send_buf, void *recv_buf, uint32_t length);
int tuya_spi_xfer_msg        (tuya_spi_t  *spi, tuya_spi_msg_t *msg, uint8_t num);
int tuya_spi_exchange        (tuya_spi_t  *spi, uint16_t out, uint16_t *in);
int tuya_spi_control         (tuya_spi_t *spi, uint8_t cmd, void *arg);
int tuya_spi_deinit          (tuya_spi_t *spi);

#define tuya_spi_send_then_send(spi, send1, len1, send2, len2)              \
    do {                                                                    \
        tuya_spi_msg_t    msg[2];                                           \
        msg[0].send_buf = send1;                                            \
        msg[0].recv_buf = NULL;                                             \
        msg[0].length   = len1;                                             \
        msg[1].send_buf = send2;                                            \
        msg[1].recv_buf = NULL;                                             \
        msg[1].length   = len2;                                             \
        tuya_spi_xfer_msg(spi, msg, 2);                                     \
    } while (0)

#define tuya_spi_send_then_recv(spi, send, len1, recv, len2)                \
    do {                                                                    \
        tuya_spi_msg_t    msg[2];                                           \
        msg[0].send_buf = send;                                             \
        msg[0].recv_buf = NULL;                                             \
        msg[0].length   = len1;                                             \
        msg[1].send_buf = NULL;                                             \
        msg[1].recv_buf = recv;                                             \
        msg[1].length   = len2;                                             \
        tuya_spi_xfer_msg(spi, msg, 2);                                     \
    } while (0)


#define tuya_spi_send(spi, buf, len)                                        \
    tuya_spi_xfer(spi, buf, NULL, len)

#define tuya_spi_recv(spi, buf, len)                                        \
    tuya_spi_xfer(spi, NULL, buf, len)


#endif
