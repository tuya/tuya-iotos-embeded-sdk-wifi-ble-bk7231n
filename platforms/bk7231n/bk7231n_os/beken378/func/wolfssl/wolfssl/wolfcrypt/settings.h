/* settings.h
 *
 * Copyright (C) 2006-2019 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */


/* Place OS specific preprocessor flags, defines, includes here, will be
   included into every file because types.h includes it */


#ifndef WOLF_CRYPT_SETTINGS_H
#define WOLF_CRYPT_SETTINGS_H

#ifdef __cplusplus
    extern "C" {
#endif

/* Uncomment next line if using IPHONE */
/* #define IPHONE */

/* Uncomment next line if using ThreadX */
/* #define THREADX */

/* Uncomment next line if using Micrium uC/OS-III */
/* #define MICRIUM */

/* Uncomment next line if using Deos RTOS*/
/* #define WOLFSSL_DEOS*/

/* Uncomment next line if using Mbed */
/* #define MBED */

/* Uncomment next line if using Microchip PIC32 ethernet starter kit */
/* #define MICROCHIP_PIC32 */

/* Uncomment next line if using Microchip TCP/IP stack, version 5 */
/* #define MICROCHIP_TCPIP_V5 */

/* Uncomment next line if using Microchip TCP/IP stack, version 6 or later */
/* #define MICROCHIP_TCPIP */

/* Uncomment next line if using PIC32MZ Crypto Engine */
/* #define WOLFSSL_MICROCHIP_PIC32MZ */

/* Uncomment next line if using FreeRTOS */
/* #define FREERTOS */

/* Uncomment next line if using FreeRTOS+ TCP */
/* #define FREERTOS_TCP */

/* Uncomment next line if using FreeRTOS Windows Simulator */
/* #define FREERTOS_WINSIM */

/* Uncomment next line if using RTIP */
/* #define EBSNET */

/* Uncomment next line if using lwip */
/* #define WOLFSSL_LWIP */

/* Uncomment next line if building wolfSSL for a game console */
/* #define WOLFSSL_GAME_BUILD */

/* Uncomment next line if building wolfSSL for LSR */
/* #define WOLFSSL_LSR */

/* Uncomment next line if building for Freescale Classic MQX version 4.0 */
/* #define FREESCALE_MQX_4_0 */

/* Uncomment next line if building for Freescale Classic MQX/RTCS/MFS */
/* #define FREESCALE_MQX */

/* Uncomment next line if building for Freescale KSDK MQX/RTCS/MFS */
/* #define FREESCALE_KSDK_MQX */

/* Uncomment next line if building for Freescale KSDK Bare Metal */
/* #define FREESCALE_KSDK_BM */

/* Uncomment next line if building for Freescale KSDK FreeRTOS, */
/* (old name FREESCALE_FREE_RTOS) */
/* #define FREESCALE_KSDK_FREERTOS */

/* Uncomment next line if using STM32F2 */
/* #define WOLFSSL_STM32F2 */

/* Uncomment next line if using STM32F4 */
/* #define WOLFSSL_STM32F4 */

/* Uncomment next line if using STM32FL */
/* #define WOLFSSL_STM32FL */

/* Uncomment next line if using STM32F7 */
/* #define WOLFSSL_STM32F7 */

/* Uncomment next line if using QL SEP settings */
/* #define WOLFSSL_QL */

/* Uncomment next line if building for EROAD */
/* #define WOLFSSL_EROAD */

/* Uncomment next line if building for IAR EWARM */
/* #define WOLFSSL_IAR_ARM */

/* Uncomment next line if building for Rowley CrossWorks ARM */
/* #define WOLFSSL_ROWLEY_ARM */

/* Uncomment next line if using TI-RTOS settings */
/* #define WOLFSSL_TIRTOS */

/* Uncomment next line if building with PicoTCP */
/* #define WOLFSSL_PICOTCP */

/* Uncomment next line if building for PicoTCP demo bundle */
/* #define WOLFSSL_PICOTCP_DEMO */

/* Uncomment next line if building for uITRON4  */
/* #define WOLFSSL_uITRON4 */

/* Uncomment next line if building for uT-Kernel */
/* #define WOLFSSL_uTKERNEL2 */

/* Uncomment next line if using Max Strength build */
/* #define WOLFSSL_MAX_STRENGTH */

/* Uncomment next line if building for VxWorks */
/* #define WOLFSSL_VXWORKS */

/* Uncomment next line if building for Nordic nRF5x platofrm */
/* #define WOLFSSL_NRF5x */

/* Uncomment next line to enable deprecated less secure static DH suites */
/* #define WOLFSSL_STATIC_DH */

/* Uncomment next line to enable deprecated less secure static RSA suites */
/* #define WOLFSSL_STATIC_RSA */

/* Uncomment next line if building for ARDUINO */
/* Uncomment both lines if building for ARDUINO on INTEL_GALILEO */
/* #define WOLFSSL_ARDUINO */
/* #define INTEL_GALILEO */

/* Uncomment next line to enable asynchronous crypto WC_PENDING_E */
/* #define WOLFSSL_ASYNC_CRYPT */

/* Uncomment next line if building for uTasker */
/* #define WOLFSSL_UTASKER */

/* Uncomment next line if building for embOS */
/* #define WOLFSSL_EMBOS */

/* Uncomment next line if building for RIOT-OS */
/* #define WOLFSSL_RIOT_OS */

/* Uncomment next line if building for using XILINX hardened crypto */
/* #define WOLFSSL_XILINX_CRYPT */

/* Uncomment next line if building for using XILINX */
/* #define WOLFSSL_XILINX */

/* Uncomment next line if building for WICED Studio. */
/* #define WOLFSSL_WICED  */

/* Uncomment next line if building for Nucleus 1.2 */
/* #define WOLFSSL_NUCLEUS_1_2 */

/* Uncomment next line if building for using Apache mynewt */
/* #define WOLFSSL_APACHE_MYNEWT */

/* Uncomment next line if building for using ESP-IDF */
/* #define WOLFSSL_ESPIDF */

/* Uncomment next line if using Espressif ESP32-WROOM-32 */
/* #define WOLFSSL_ESPWROOM32 */

/* Uncomment next line if using Espressif ESP32-WROOM-32SE */
/* #define WOLFSSL_ESPWROOM32SE */

/* Uncomment next line if using ARM CRYPTOCELL*/
/* #define WOLFSSL_CRYPTOCELL */

#include <wolfssl/wolfcrypt/visibility.h>

#ifdef WOLFSSL_USER_SETTINGS
    #include "user_settings.h"
#endif

#include "sys_config.h"

/* make sure old RNG name is used with CTaoCrypt FIPS */
#ifdef HAVE_FIPS
    #if !defined(HAVE_FIPS_VERSION) || (HAVE_FIPS_VERSION < 2)
        #define WC_RNG RNG
    #else
        #ifndef WOLFSSL_STM32L4
            #define RNG WC_RNG
        #endif
    #endif
    /* blinding adds API not available yet in FIPS mode */
    #undef WC_RSA_BLINDING
#endif

#if defined(WOLFSSL_BEKEN)
#define FREERTOS
#define SIZEOF_LONG_LONG 8
#define WOLFSSL_HAVE_MIN
#define WOLFSSL_HAVE_MAX
#define WOLFSSL_NO_CURRDIR
#define TFM_TIMING_RESISTANT
#define ECC_TIMING_RESISTANT
#define NO_DSA
#define NO_RSA
#define TFM_ECC256
#define ECC_SHAMIR
#define NO_RC4
#define NO_HC128
#define NO_RABBIT
#define HAVE_HASHDRBG
#define HAVE_SUPPORTED_CURVES
#define NO_PSK
#define NO_PWDBASED
#define USE_FAST_MATH
#define WC_NO_ASYNC_THREADING
#define NO_DES3
#define NO_WOLFSSL_DIR
#define WOLFSSL_NO_ASM
#define WOLFSSL_NO_SOCK
#define NO_CERTS
#define NO_WRITEV
#define NO_ERROR_STRINGS
#define WOLFSSL_SMALL_STACK
//#define WOLFSSL_SMALL_STACK_CACHE
#define HAVE_COMP_KEY
#define WOLFSSL_KEY_GEN
#define SQRTMOD_USE_MOD_EXP
#define FP_MAX_BITS	768

//#define WC_RSA_BLINDING
//#define BUILDING_WOLFSSL
//#define HAVE_FFDHE_2048
//#define _POSIX_THREADS
//#define HAVE_THREAD_LS
//#define HAVE_AESGCM
//#define WOLFSSL_SHA512
//#define WOLFSSL_SHA384
//#define WOLFSSL_SHA224
//#define WOLFSSL_SHA3
//#define WOLFSSL_BASE64_ENCODE
//#define HAVE_POLY1305
//#define HAVE_ONE_TIME_AUTH
//#define HAVE_CHACHA
//#define HAVE_TLS_EXTENSIONS
//#define HAVE_EXTENDED_MASTER
//#define WOLFSSL_X86_64_BUILD

/* Customize ECC */
#define HAVE_ECC
#define ECC_USER_CURVES
//#define ECC192
//#define ECC224
#define ECC256
#define ECC384
//#define ECC521
#endif


#if defined(WOLFSSL_ESPIDF)
    #define FREERTOS
    #define WOLFSSL_LWIP
    #define NO_WRITEV
    #define SIZEOF_LONG_LONG 8
    #define NO_WOLFSSL_DIR
    #define WOLFSSL_NO_CURRDIR

    #define TFM_TIMING_RESISTANT
    #define ECC_TIMING_RESISTANT
    #define WC_RSA_BLINDING

#if defined(WOLFSSL_ESPWROOM32) || defined(WOLFSSL_ESPWROOM32SE)
   #ifndef NO_ESP32WROOM32_CRYPT
        #define WOLFSSL_ESP32WROOM32_CRYPT
        #if defined(ESP32_USE_RSA_PRIMITIVE) && \
            !defined(NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI)
            #define WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI
            #define USE_FAST_MATH
            #define WOLFSSL_SMALL_STACK
        #endif
   #endif
#endif
#endif /* WOLFSSL_ESPIDF */

#if defined(HAVE_LWIP_NATIVE) /* using LwIP native TCP socket */
    #define WOLFSSL_LWIP
    #define NO_WRITEV
    #define SINGLE_THREADED
    #define WOLFSSL_USER_IO
    #define NO_FILESYSTEM
#endif

#ifdef MBED
    #define WOLFSSL_USER_IO
    #define NO_FILESYSTEM
    #define NO_CERTS
    #if !defined(USE_CERT_BUFFERS_2048) && !defined(USE_CERT_BUFFERS_4096)
        #define USE_CERT_BUFFERS_1024
    #endif
    #define NO_WRITEV
    #define NO_DEV_RANDOM
    #define NO_SHA512
    #define NO_DH
    /* Allows use of DH with fixed points if uncommented and NO_DH is removed */
    /* WOLFSSL_DH_CONST */
    #define NO_DSA
    #define NO_HC128
    #define HAVE_ECC
    #define NO_SESSION_CACHE
    #define WOLFSSL_CMSIS_RTOS
#endif


#ifdef FREERTOS
    #include "FreeRTOS.h"

    /* FreeRTOS pvPortRealloc() only in AVR32_UC3 port */
    #if !defined(XMALLOC_USER) && !defined(NO_WOLFSSL_MEMORY) && \
        !defined(WOLFSSL_STATIC_MEMORY)

	#if OSMALLOC_STATISTICAL
        #define XMALLOC(s, h, type)  pvPortMalloc_cm((const char*)NULL,__LINE__,(s), 0)
        #define XFREE(p, h, type)    vPortFree_cm((const char*)NULL,__LINE__,(p))
	#else
		#define XMALLOC(s, h, type)  pvPortMalloc((s))
		#define XFREE(p, h, type)	 vPortFree((p))
	#endif
    #endif
    #if defined(HAVE_ED25519) || defined(WOLFSSL_ESPIDF)
        #define XREALLOC(p, n, h, t) wolfSSL_Realloc((p), (n))
    #endif
    #ifndef NO_WRITEV
        #define NO_WRITEV
    #endif
    #ifndef HAVE_SHA512
        #ifndef NO_SHA512
            #define NO_SHA512
        #endif
    #endif
    #ifndef HAVE_DH
        #ifndef NO_DH
            #define NO_DH
        #endif
    #endif
    #ifndef NO_DSA
        #define NO_DSA
    #endif
    #ifndef NO_HC128
        #define NO_HC128
    #endif

    #ifndef SINGLE_THREADED
        #include "semphr.h"
    #endif
#endif

#ifdef FREERTOS_TCP
    #if !defined(NO_WOLFSSL_MEMORY) && !defined(XMALLOC_USER) && \
        !defined(WOLFSSL_STATIC_MEMORY)
        #define XMALLOC(s, h, type)  pvPortMalloc((s))
        #define XFREE(p, h, type)    vPortFree((p))
    #endif

    #define WOLFSSL_GENSEED_FORTEST

    #define NO_WOLFSSL_DIR
    #define NO_WRITEV
    #define USE_FAST_MATH
    #define TFM_TIMING_RESISTANT
    #define NO_MAIN_DRIVER
#endif

#ifdef WOLFSSL_LOW_MEMORY
    #undef  RSA_LOW_MEM
    #define RSA_LOW_MEM
    #undef  WOLFSSL_SMALL_STACK
    #define WOLFSSL_SMALL_STACK
    #undef  TFM_TIMING_RESISTANT
    #define TFM_TIMING_RESISTANT
#endif

#ifdef WOLFSSL_QL
    #ifndef WOLFSSL_SEP
        #define WOLFSSL_SEP
    #endif
    #ifndef OPENSSL_EXTRA
        #define OPENSSL_EXTRA
    #endif
    #ifndef SESSION_CERTS
        #define SESSION_CERTS
    #endif
    #ifndef HAVE_AESCCM
        #define HAVE_AESCCM
    #endif
    #ifndef ATOMIC_USER
        #define ATOMIC_USER
    #endif
    #ifndef WOLFSSL_DER_LOAD
        #define WOLFSSL_DER_LOAD
    #endif
    #ifndef KEEP_PEER_CERT
        #define KEEP_PEER_CERT
    #endif
    #ifndef HAVE_ECC
        #define HAVE_ECC
    #endif
    #ifndef SESSION_INDEX
        #define SESSION_INDEX
    #endif
#endif /* WOLFSSL_QL */


/* if defined turn on all CAAM support */
#if !defined(XMALLOC_USER) && !defined(MICRIUM_MALLOC) && \
    !defined(WOLFSSL_LEANPSK) && !defined(NO_WOLFSSL_MEMORY) && \
    !defined(XMALLOC_OVERRIDE)
    #define USE_WOLFSSL_MEMORY
#endif


#if defined(OPENSSL_EXTRA) && !defined(NO_CERTS)
    #undef  KEEP_PEER_CERT
    #define KEEP_PEER_CERT
#endif


/* stream ciphers except arc4 need 32bit alignment, intel ok without */
#ifndef XSTREAM_ALIGN
    #if defined(__x86_64__) || defined(__ia64__) || defined(__i386__)
        #define NO_XSTREAM_ALIGN
    #else
        #define XSTREAM_ALIGN
    #endif
#endif

/* write dup cannot be used with secure renegotiation because write dup
 * make write side write only and read side read only */
#if defined(HAVE_WRITE_DUP) && defined(HAVE_SECURE_RENEGOTIATION)
    #error "WRITE DUP and SECURE RENEGOTIATION cannot both be on"
#endif

/* if using hardware crypto and have alignment requirements, specify the
   requirement here.  The record header of SSL/TLS will prevent easy alignment.
   This hint tries to help as much as possible.  */
#ifndef WOLFSSL_GENERAL_ALIGNMENT
    #ifdef WOLFSSL_AESNI
        #define WOLFSSL_GENERAL_ALIGNMENT 16
    #elif defined(XSTREAM_ALIGN)
        #define WOLFSSL_GENERAL_ALIGNMENT  4
    #elif defined(FREESCALE_MMCAU) || defined(FREESCALE_MMCAU_CLASSIC)
        #define WOLFSSL_GENERAL_ALIGNMENT  WOLFSSL_MMCAU_ALIGNMENT
    #else
        #define WOLFSSL_GENERAL_ALIGNMENT  0
    #endif
#endif

#if defined(WOLFSSL_GENERAL_ALIGNMENT) && (WOLFSSL_GENERAL_ALIGNMENT > 0)
    #if defined(_MSC_VER)
        #define XGEN_ALIGN __declspec(align(WOLFSSL_GENERAL_ALIGNMENT))
    #elif defined(__GNUC__)
        #define XGEN_ALIGN __attribute__((aligned(WOLFSSL_GENERAL_ALIGNMENT)))
    #else
        #define XGEN_ALIGN
    #endif
#else
    #define XGEN_ALIGN
#endif

#ifdef HAVE_CRL
    /* not widely supported yet */
    #undef NO_SKID
    #define NO_SKID
#endif


#ifdef __INTEL_COMPILER
    #pragma warning(disable:2259) /* explicit casts to smaller sizes, disable */
#endif

/* user can specify what curves they want with ECC_USER_CURVES otherwise
 * all curves are on by default for now */
#ifndef ECC_USER_CURVES
    #if !defined(WOLFSSL_SP_MATH) && !defined(HAVE_ALL_CURVES)
        #define HAVE_ALL_CURVES
    #endif
#endif

/* ECC Configs */
#ifdef HAVE_ECC
    /* By default enable Sign, Verify, DHE, Key Import and Key Export unless explicitly disabled */
    #ifndef NO_ECC_SIGN
        #undef HAVE_ECC_SIGN
        #define HAVE_ECC_SIGN
    #endif
    #ifndef NO_ECC_VERIFY
        #undef HAVE_ECC_VERIFY
        #define HAVE_ECC_VERIFY
    #endif
    #ifndef NO_ECC_CHECK_KEY
        #undef HAVE_ECC_CHECK_KEY
        #define HAVE_ECC_CHECK_KEY
    #endif
    #ifndef NO_ECC_DHE
        #undef HAVE_ECC_DHE
        #define HAVE_ECC_DHE
    #endif
    #ifndef NO_ECC_KEY_IMPORT
        #undef HAVE_ECC_KEY_IMPORT
        #define HAVE_ECC_KEY_IMPORT
    #endif
    #ifndef NO_ECC_KEY_EXPORT
        #undef HAVE_ECC_KEY_EXPORT
        #define HAVE_ECC_KEY_EXPORT
    #endif
#endif /* HAVE_ECC */

/* Curve255519 Configs */
#ifdef HAVE_CURVE25519
    /* By default enable shared secret, key export and import */
    #ifndef NO_CURVE25519_SHARED_SECRET
        #undef HAVE_CURVE25519_SHARED_SECRET
        #define HAVE_CURVE25519_SHARED_SECRET
    #endif
    #ifndef NO_CURVE25519_KEY_EXPORT
        #undef HAVE_CURVE25519_KEY_EXPORT
        #define HAVE_CURVE25519_KEY_EXPORT
    #endif
    #ifndef NO_CURVE25519_KEY_IMPORT
        #undef HAVE_CURVE25519_KEY_IMPORT
        #define HAVE_CURVE25519_KEY_IMPORT
    #endif
#endif /* HAVE_CURVE25519 */

/* Ed255519 Configs */
#ifdef HAVE_ED25519
    /* By default enable sign, verify, key export and import */
    #ifndef NO_ED25519_SIGN
        #undef HAVE_ED25519_SIGN
        #define HAVE_ED25519_SIGN
    #endif
    #ifndef NO_ED25519_VERIFY
        #undef HAVE_ED25519_VERIFY
        #define HAVE_ED25519_VERIFY
    #endif
    #ifndef NO_ED25519_KEY_EXPORT
        #undef HAVE_ED25519_KEY_EXPORT
        #define HAVE_ED25519_KEY_EXPORT
    #endif
    #ifndef NO_ED25519_KEY_IMPORT
        #undef HAVE_ED25519_KEY_IMPORT
        #define HAVE_ED25519_KEY_IMPORT
    #endif
#endif /* HAVE_ED25519 */

/* AES Config */
#ifndef NO_AES
    /* By default enable all AES key sizes, decryption and CBC */
    #ifndef AES_MAX_KEY_SIZE
        #undef  AES_MAX_KEY_SIZE
        #define AES_MAX_KEY_SIZE    256
    #endif

    #ifndef NO_AES_128
        #undef  WOLFSSL_AES_128
        #define WOLFSSL_AES_128
    #endif
    #if !defined(NO_AES_192) && AES_MAX_KEY_SIZE >= 192
        #undef  WOLFSSL_AES_192
        #define WOLFSSL_AES_192
    #endif
    #if !defined(NO_AES_256) && AES_MAX_KEY_SIZE >= 256
        #undef  WOLFSSL_AES_256
        #define WOLFSSL_AES_256
    #endif
    #if !defined(WOLFSSL_AES_128) && defined(HAVE_ECC_ENCRYPT)
        #warning HAVE_ECC_ENCRYPT uses AES 128 bit keys
     #endif

    #ifndef NO_AES_DECRYPT
        #undef  HAVE_AES_DECRYPT
        #define HAVE_AES_DECRYPT
    #endif
    #ifndef NO_AES_CBC
        #undef  HAVE_AES_CBC
        #define HAVE_AES_CBC
    #endif
    #ifdef WOLFSSL_AES_XTS
        /* AES-XTS makes calls to AES direct functions */
        #ifndef WOLFSSL_AES_DIRECT
        #define WOLFSSL_AES_DIRECT
        #endif
    #endif
    #ifdef WOLFSSL_AES_CFB
        /* AES-CFB makes calls to AES direct functions */
        #ifndef WOLFSSL_AES_DIRECT
        #define WOLFSSL_AES_DIRECT
        #endif
    #endif
#endif

#if (defined(WOLFSSL_TLS13) && defined(WOLFSSL_NO_TLS12)) || \
    (!defined(HAVE_AES_CBC) && defined(NO_DES3) && defined(NO_RC4) && \
     !defined(HAVE_CAMELLIA) && !defined(HAVE_IDEA) && \
     !defined(HAVE_NULL_CIPHER) && !defined(HAVE_HC128))
    #define WOLFSSL_AEAD_ONLY
#endif

#if !defined(NO_DH) && !defined(HAVE_FFDHE)
    #if defined(HAVE_FFDHE_2048) || defined(HAVE_FFDHE_3072) || \
            defined(HAVE_FFDHE_4096) || defined(HAVE_FFDHE_6144) || \
            defined(HAVE_FFDHE_8192)
        #define HAVE_FFDHE
    #endif
#endif
#if defined(HAVE_FFDHE_8192)
    #define MIN_FFDHE_FP_MAX_BITS 16384
#elif defined(HAVE_FFDHE_6144)
    #define MIN_FFDHE_FP_MAX_BITS 12288
#elif defined(HAVE_FFDHE_4096)
    #define MIN_FFDHE_FP_MAX_BITS 8192
#elif defined(HAVE_FFDHE_3072)
    #define MIN_FFDHE_FP_MAX_BITS 6144
#elif defined(HAVE_FFDHE_2048)
    #define MIN_FFDHE_FP_MAX_BITS 4096
#else
    #define MIN_FFDHE_FP_MAX_BITS 0
#endif
#if defined(HAVE_FFDHE) && defined(FP_MAX_BITS)
    #if MIN_FFDHE_FP_MAX_BITS > FP_MAX_BITS
        #error "FFDHE parameters are too large for FP_MAX_BIT as set"
    #endif
#endif

/* if desktop type system and fastmath increase default max bits */
#ifdef WOLFSSL_X86_64_BUILD
    #if defined(USE_FAST_MATH) && !defined(FP_MAX_BITS)
        #if MIN_FFDHE_FP_MAX_BITS <= 8192
            #define FP_MAX_BITS 8192
        #else
            #define FP_MAX_BITS MIN_FFDHE_FP_MAX_BITS
        #endif
    #endif
#endif

/* If using the max strength build, ensure OLD TLS is disabled. */
#ifdef WOLFSSL_MAX_STRENGTH
    #undef NO_OLD_TLS
    #define NO_OLD_TLS
#endif


/* Default AES minimum auth tag sz, allow user to override */
#ifndef WOLFSSL_MIN_AUTH_TAG_SZ
    #define WOLFSSL_MIN_AUTH_TAG_SZ 12
#endif


/* sniffer requires:
 * static RSA cipher suites
 * session stats and peak stats
 */
#ifdef WOLFSSL_SNIFFER
    #ifndef WOLFSSL_STATIC_RSA
        #define WOLFSSL_STATIC_RSA
    #endif
    #ifndef WOLFSSL_SESSION_STATS
        #define WOLFSSL_SESSION_STATS
    #endif
    #ifndef WOLFSSL_PEAK_SESSIONS
        #define WOLFSSL_PEAK_SESSIONS
    #endif
#endif

/* Decode Public Key extras on by default, user can turn off with
 * WOLFSSL_NO_DECODE_EXTRA */
#ifndef WOLFSSL_NO_DECODE_EXTRA
    #ifndef RSA_DECODE_EXTRA
        #define RSA_DECODE_EXTRA
    #endif
    #ifndef ECC_DECODE_EXTRA
        #define ECC_DECODE_EXTRA
    #endif
#endif

/* C Sharp wrapper defines */
#ifdef HAVE_CSHARP
    #ifndef WOLFSSL_DTLS
        #define WOLFSSL_DTLS
    #endif
    #undef NO_PSK
    #undef NO_SHA256
    #undef NO_DH
#endif

/* Asynchronous Crypto */
#ifdef WOLFSSL_ASYNC_CRYPT
    /* Make sure wolf events are enabled */
    #undef HAVE_WOLF_EVENT
    #define HAVE_WOLF_EVENT

    #ifdef WOLFSSL_ASYNC_CRYPT_TEST
        #define WC_ASYNC_DEV_SIZE 168
    #else
        #define WC_ASYNC_DEV_SIZE 336
    #endif

    #if !defined(HAVE_CAVIUM) && !defined(HAVE_INTEL_QA) && \
        !defined(WOLFSSL_ASYNC_CRYPT_TEST)
        #error No async hardware defined with WOLFSSL_ASYNC_CRYPT!
    #endif

    /* Enable ECC_CACHE_CURVE for ASYNC */
    #if !defined(ECC_CACHE_CURVE)
        #define ECC_CACHE_CURVE
    #endif
#endif /* WOLFSSL_ASYNC_CRYPT */
#ifndef WC_ASYNC_DEV_SIZE
    #define WC_ASYNC_DEV_SIZE 0
#endif

/* leantls checks */
#ifdef WOLFSSL_LEANTLS
    #ifndef HAVE_ECC
        #error leantls build needs ECC
    #endif
#endif /* WOLFSSL_LEANTLS*/

/* restriction with static memory */
#ifdef WOLFSSL_STATIC_MEMORY
    #if defined(HAVE_IO_POOL) || defined(XMALLOC_USER) || defined(NO_WOLFSSL_MEMORY)
         #error static memory cannot be used with HAVE_IO_POOL, XMALLOC_USER or NO_WOLFSSL_MEMORY
    #endif
    #if !defined(USE_FAST_MATH) && !defined(NO_BIG_INT)
        #error static memory requires fast math please define USE_FAST_MATH
    #endif
    #ifdef WOLFSSL_SMALL_STACK
        #error static memory does not support small stack please undefine
    #endif
#endif /* WOLFSSL_STATIC_MEMORY */

#ifdef HAVE_AES_KEYWRAP
    #ifndef WOLFSSL_AES_DIRECT
        #error AES key wrap requires AES direct please define WOLFSSL_AES_DIRECT
    #endif
#endif

#ifdef HAVE_PKCS7
    #if defined(NO_AES) && defined(NO_DES3)
        #error PKCS7 needs either AES or 3DES enabled, please enable one
    #endif
    #ifndef HAVE_AES_KEYWRAP
        #error PKCS7 requires AES key wrap please define HAVE_AES_KEYWRAP
    #endif
    #if defined(HAVE_ECC) && !defined(HAVE_X963_KDF)
        #error PKCS7 requires X963 KDF please define HAVE_X963_KDF
    #endif
#endif

#ifndef NO_PKCS12
    #undef  HAVE_PKCS12
    #define HAVE_PKCS12
#endif

#ifndef NO_PKCS8
    #undef  HAVE_PKCS8
    #define HAVE_PKCS8
#endif

#if !defined(NO_PBKDF1) || defined(WOLFSSL_ENCRYPTED_KEYS) || defined(HAVE_PKCS8) || defined(HAVE_PKCS12)
    #undef  HAVE_PBKDF1
    #define HAVE_PBKDF1
#endif

#if !defined(NO_PBKDF2) || defined(HAVE_PKCS7) || defined(HAVE_SCRYPT)
    #undef  HAVE_PBKDF2
    #define HAVE_PBKDF2
#endif


#if !defined(WOLFCRYPT_ONLY) && !defined(NO_OLD_TLS) && \
        (defined(NO_SHA) || defined(NO_MD5))
    #error old TLS requires MD5 and SHA
#endif

/* for backwards compatibility */
#if defined(TEST_IPV6) && !defined(WOLFSSL_IPV6)
    #define WOLFSSL_IPV6
#endif


/* Place any other flags or defines here */

#if defined(WOLFSSL_MYSQL_COMPATIBLE) && defined(_WIN32) \
                                      && defined(HAVE_GMTIME_R)
    #undef HAVE_GMTIME_R /* don't trust macro with windows */
#endif /* WOLFSSL_MYSQL_COMPATIBLE */

#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
    #define SSL_OP_NO_COMPRESSION    SSL_OP_NO_COMPRESSION
    #define OPENSSL_NO_ENGINE
    #define X509_CHECK_FLAG_ALWAYS_CHECK_SUBJECT
    #ifndef OPENSSL_EXTRA
        #define OPENSSL_EXTRA
    #endif
    #ifndef HAVE_SESSION_TICKET
        #define HAVE_SESSION_TICKET
    #endif
    #ifndef HAVE_OCSP
        #define HAVE_OCSP
    #endif
    #ifndef KEEP_OUR_CERT
        #define KEEP_OUR_CERT
    #endif
    #ifndef HAVE_SNI
        #define HAVE_SNI
    #endif
#endif

#if defined(WOLFSSL_NGINX)
    #define SSL_CTRL_SET_TLSEXT_HOSTNAME
#endif


/* both CURVE and ED small math should be enabled */
#ifdef CURVED25519_SMALL
        #define CURVE25519_SMALL
        #define ED25519_SMALL
#endif


#ifndef WOLFSSL_ALERT_COUNT_MAX
    #define WOLFSSL_ALERT_COUNT_MAX 5
#endif

/* warning for not using harden build options (default with ./configure) */
#ifndef WC_NO_HARDEN
    #if (defined(USE_FAST_MATH) && !defined(TFM_TIMING_RESISTANT)) || \
        (defined(HAVE_ECC) && !defined(ECC_TIMING_RESISTANT)) || \
        (!defined(NO_RSA) && !defined(WC_RSA_BLINDING) && !defined(HAVE_FIPS) && \
            !defined(WC_NO_RNG))

        #ifndef _MSC_VER
            #warning "For timing resistance / side-channel attack prevention consider using harden options"
        #else
            #pragma message("Warning: For timing resistance / side-channel attack prevention consider using harden options")
        #endif
    #endif
#endif

#if defined(NO_OLD_WC_NAMES) || defined(OPENSSL_EXTRA)
    /* added to have compatibility with SHA256() */
    #if !defined(NO_OLD_SHA_NAMES) && !defined(HAVE_FIPS)
        #define NO_OLD_SHA_NAMES
    #endif
#endif

/* switch for compatibility layer functionality. Has subparts i.e. BIO/X509
 * When opensslextra is enabled all subparts should be turned on. */
#ifdef OPENSSL_EXTRA
    #undef  OPENSSL_EXTRA_X509_SMALL
    #define OPENSSL_EXTRA_X509_SMALL
#endif /* OPENSSL_EXTRA */

/* support for converting DER to PEM */
#if defined(WOLFSSL_KEY_GEN) || defined(WOLFSSL_CERT_GEN) || \
        defined(OPENSSL_EXTRA)
    #undef  WOLFSSL_DER_TO_PEM
    #define WOLFSSL_DER_TO_PEM
#endif

/* keep backwards compatibility enabling encrypted private key */
#ifndef WOLFSSL_ENCRYPTED_KEYS
    #if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL) || \
        defined(HAVE_WEBSERVER)
        #define WOLFSSL_ENCRYPTED_KEYS
    #endif
#endif

/* support for disabling PEM to DER */
#if !defined(WOLFSSL_NO_PEM)
    #undef  WOLFSSL_PEM_TO_DER
    #define WOLFSSL_PEM_TO_DER
#endif

/* Parts of the openssl compatibility layer require peer certs */
#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
    #undef  KEEP_PEER_CERT
    #define KEEP_PEER_CERT
#endif

/* RAW hash function APIs are not implemented with ARMv8 hardware acceleration*/
#ifdef WOLFSSL_ARMASM
    #undef  WOLFSSL_NO_HASH_RAW
    #define WOLFSSL_NO_HASH_RAW
#endif

#if !defined(WOLFSSL_SHA384) && !defined(WOLFSSL_SHA512) && defined(NO_AES) && \
                                                          !defined(WOLFSSL_SHA3)
    #undef  WOLFSSL_NO_WORD64_OPS
    #define WOLFSSL_NO_WORD64_OPS
#endif

#if !defined(WOLFCRYPT_ONLY) && !defined(WOLFSSL_NO_TLS12)
    #undef  WOLFSSL_HAVE_PRF
    #define WOLFSSL_HAVE_PRF
#endif

#if defined(NO_AES) && defined(NO_DES3) && !defined(HAVE_CAMELLIA) && \
       !defined(WOLFSSL_HAVE_PRF) && defined(NO_PWDBASED) && !defined(HAVE_IDEA)
    #undef  WOLFSSL_NO_XOR_OPS
    #define WOLFSSL_NO_XOR_OPS
#endif

#if defined(NO_ASN) && defined(WOLFCRYPT_ONLY)
    #undef  WOLFSSL_NO_INT_ENCODE
    #define WOLFSSL_NO_INT_ENCODE
    #undef  WOLFSSL_NO_INT_DECODE
    #define WOLFSSL_NO_INT_DECODE
#endif

#if defined(WOLFCRYPT_ONLY) && defined(WOLFSSL_RSA_VERIFY_ONLY) && \
    defined(WC_NO_RSA_OAEP)
    #undef  WOLFSSL_NO_CT_OPS
    #define WOLFSSL_NO_CT_OPS
#endif

#if defined(WOLFCRYPT_ONLY) && defined(NO_AES) && !defined(HAVE_CURVE25519) && \
                                   defined(WC_NO_RNG) && defined(WC_NO_RSA_OAEP)
    #undef  WOLFSSL_NO_CONST_CMP
    #define WOLFSSL_NO_CONST_CMP
#endif

#if defined(WOLFCRYPT_ONLY) && defined(NO_AES) && !defined(WOLFSSL_SHA384) && \
    !defined(WOLFSSL_SHA512) && defined(WC_NO_RNG) && \
                    defined(WOLFSSL_SP_MATH) && defined(WOLFSSL_RSA_PUBLIC_ONLY)
    #undef  WOLFSSL_NO_FORCE_ZERO
    #define WOLFSSL_NO_FORCE_ZERO
#endif

#if defined(WOLFSSL_TLS13) && defined(WOLFSSL_NO_SIGALG)
    #error TLS 1.3 requires the Signature Algorithms extension to be enabled
#endif

#ifdef __cplusplus
    }   /* extern "C" */
#endif

#endif
