/*
 * coap.h -- main header file for CoAP stack of libcoap
 *
 * Copyright (C) 2010-2012,2015-2017 Olaf Bergmann <bergmann@tzi.org>
 *               2015 Carsten Schoenert <c.schoenert@t-online.de>
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 */

#ifndef COAP_H_
#define COAP_H_

/* Define the address where bug reports for libcoap should be sent. */
#define LIBCOAP_PACKAGE_BUGREPORT "libcoap-developers@lists.sourceforge.net"

/* Define the full name of libcoap. */
#define LIBCOAP_PACKAGE_NAME "libcoap"

/* Define the full name and version of libcoap. */
#define LIBCOAP_PACKAGE_STRING "libcoap 4.2.0"

/* Define the home page for libcoap. */
#define LIBCOAP_PACKAGE_URL "https://libcoap.net/"

/* Define the version of libcoap this file belongs to. */
#define LIBCOAP_PACKAGE_VERSION "4.2.0rc4"

#ifdef __cplusplus
extern "C" {
#endif

#include "coap_config.h"
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#include <unistd.h>

#include "libcoap.h"

#include "address.h"
#include "async.h"
#include "bits.h"
#include "block.h"
#include "coap_dtls.h"
#include "coap_event.h"
#include "coap_io.h"
#include "coap_time.h"
#include "coap_debug.h"
#include "encode.h"
#include "coap_mem.h"
#include "coap_net.h"
#include "option.h"
#include "pdu.h"
#include "prng.h"
#include "resource.h"
#include "str.h"
#include "subscribe.h"
#include "uri.h"

#ifdef __cplusplus
}
#endif

#endif /* COAP_H_ */
