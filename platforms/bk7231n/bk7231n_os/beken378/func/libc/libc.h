/****************************************************************************
 * libs/libc/libc.h
 *
 *   Copyright (C) 2007-2014, 2016-2017 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef __LIBC_LIBC_H
#define __LIBC_LIBC_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

// #include <nuttx/config.h>

#include <sys/types.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#include <stdarg.h>
// #include <semaphore.h>

// #include <nuttx/streams.h>

#define CONFIG_HAVE_LONG_LONG
// #define CONFIG_LIBC_FLOATINGPOINT
#define CONFIG_LIBC_SCANSET
#define CONFIG_STDIO_DISABLE_BUFFERING
#define FAR
#define CODE
#define IPTR
#define ERROR -1

#define DEBUGASSERT(...) do {} while(0)
#define linfo(...) do {} while(0)

struct lib_instream_s;
typedef CODE int  (*lib_getc_t)(FAR struct lib_instream_s *this);

struct lib_instream_s
{
	lib_getc_t             get;     /* Get one character from the instream */
	int                    nget;    /* Total number of characters gotten.  Written
									 * by get method, readable by user */
};

/* These are streams that operate on a fixed-sized block of memory */

struct lib_meminstream_s
{
	struct lib_instream_s  public;
	FAR const char        *buffer;  /* Address of first byte in the buffer */
	size_t                 buflen;  /* Size of the buffer in bytes */
};

/* Seek-able streams */

struct lib_sistream_s;
typedef CODE int   (*lib_sigetc_t)(FAR struct lib_sistream_s *this);
typedef CODE off_t (*lib_siseek_t)(FAR struct lib_sistream_s *this,
		off_t offset, int whence);

struct lib_sostream_s;
typedef CODE void  (*lib_soputc_t)(FAR struct lib_sostream_s *this, int ch);
typedef CODE int   (*lib_soflush_t)(FAR struct lib_sostream_s *this);
typedef CODE off_t (*lib_soseek_t)(FAR struct lib_sostream_s *this,
		off_t offset, int whence);

struct lib_sistream_s
{
	lib_sigetc_t           get;     /* Get one character from the instream */
	lib_siseek_t           seek;    /* Seek to a position in the instream */
	int                    nget;    /* Total number of characters gotten.  Written
									 * by get method, readable by user */
};

struct lib_memsistream_s
{
  struct lib_sistream_s  public;
  FAR const char        *buffer;  /* Address of first byte in the buffer */
  size_t                 offset;  /* Current buffer offset in bytes */
  size_t                 buflen;  /* Size of the buffer in bytes */
};


/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* This configuration directory is used in environment variable processing
 * when we need to reference the user's home directory.  There are no user
 * directories in NuttX so, by default, this always refers to the root
 * directory.
 */

#ifndef CONFIG_LIB_HOMEDIR
# define CONFIG_LIB_HOMEDIR "/"
#endif

/* If C std I/O buffering is not supported, then we don't need its semaphore
 * protection.
 */

#ifdef CONFIG_STDIO_DISABLE_BUFFERING
#  define lib_sem_initialize(s)
#  define lib_take_semaphore(s)
#  define lib_give_semaphore(s)
#endif

/* The NuttX C library an be build in two modes: (1) as a standard, C-library
 * that can be used by normal, user-space applications, or (2) as a special,
 * kernel-mode C-library only used within the OS.  If NuttX is not being
 * built as separated kernel- and user-space modules, then only the first
 * mode is supported.
 */

#if (defined(CONFIG_BUILD_PROTECTED) && defined(__KERNEL__)) || \
	defined(CONFIG_BUILD_KERNEL)
#  include <nuttx/kmalloc.h>

/* Domain-specific allocations */

#  define lib_malloc(s)     kmm_malloc(s)
#  define lib_zalloc(s)     kmm_zalloc(s)
#  define lib_realloc(p,s)  kmm_realloc(p,s)
#  define lib_memalign(p,s) kmm_memalign(p,s)
#  define lib_free(p)       kmm_free(p)

/* User-accessible allocations */

#  define lib_umalloc(s)    kumm_malloc(s)
#  define lib_uzalloc(s)    kumm_zalloc(s)
#  define lib_urealloc(p,s) kumm_realloc(p,s)
#  define lib_ufree(p)      kumm_free(p)

#else
#  include <stdlib.h>

/* Domain-specific allocations */

#  define lib_malloc(s)     malloc(s)
#  define lib_zalloc(s)     zalloc(s)
#  define lib_realloc(p,s)  realloc(p,s)
#  define lib_free(p)       free(p)

/* User-accessible allocations */

#  define lib_umalloc(s)    malloc(s)
#  define lib_uzalloc(s)    zalloc(s)
#  define lib_urealloc(p,s) realloc(p,s)
#  define lib_ufree(p)      free(p)

#endif

#define LIB_BUFLEN_UNKNOWN INT_MAX

/****************************************************************************
 * Public Types
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

	/****************************************************************************
	 * Public Function Prototypes
	 ****************************************************************************/

	/* Defined in lib_streamsem.c */

#if CONFIG_NFILE_STREAMS > 0
	void  stream_semtake(FAR struct streamlist *list);
	void  stream_semgive(FAR struct streamlist *list);
#endif

	/* Defined in lib_dtoa.c */

#ifdef CONFIG_LIBC_FLOATINGPOINT
	char *__dtoa(double d, int mode, int ndigits, int *decpt, int *sign,
			char **rve);
#endif

	/* Defined in lib_fopen.c */

	int lib_mode2oflags(FAR const char *mode);

	/* Defined in lib_libfwrite.c */

	ssize_t lib_fwrite(FAR const void *ptr, size_t count, FAR FILE *stream);

	/* Defined in lib_libfread.c */

	ssize_t lib_fread(FAR void *ptr, size_t count, FAR FILE *stream);

	/* Defined in lib_libfgets.c */

	FAR char *lib_fgets(FAR char *buf, size_t buflen, FILE *stream,
			bool keepnl, bool consume);

	/* Defined in lib_libfflush.c */

	ssize_t lib_fflush(FAR FILE *stream, bool bforce);

	/* Defined in lib_rdflush.c */

	int lib_rdflush(FAR FILE *stream);

	/* Defined in lib_wrflush.c */

	int lib_wrflush(FAR FILE *stream);

	/* Defined in lib_sem.c */

#ifndef CONFIG_STDIO_DISABLE_BUFFERING
	void lib_sem_initialize(FAR struct file_struct *stream);
	void lib_take_semaphore(FAR struct file_struct *stream);
	void lib_give_semaphore(FAR struct file_struct *stream);
#endif

	/* Defined in lib_libgetbase.c */

	int lib_getbase(const char *nptr, const char **endptr);

	/* Defined in lib_skipspace.c */

	void lib_skipspace(const char **pptr);

	/* Defined in lib_isbasedigit.c */

	bool lib_isbasedigit(int ch, int base, int *value);

	/* Defined in lib_checkbase.c */

	int lib_checkbase(int base, const char **pptr);

	/* Defined in lib_expi.c */

#ifdef CONFIG_LIBM
	float  lib_expif(size_t n);
	double lib_expi(size_t n);
#endif

	/* Defined in lib_libsqrtapprox.c */

#ifdef CONFIG_LIBM
	float lib_sqrtapprox(float x);
#endif

	/* Defined in lib_parsehostfile.c */

#ifdef CONFIG_NETDB_HOSTFILE
	struct hostent;
	ssize_t lib_parse_hostfile(FAR FILE *stream, FAR struct hostent *host,
			FAR char *buf, size_t buflen);
#endif


	void lib_meminstream(FAR struct lib_meminstream_s *instream,
				FAR const char *bufstart, int buflen);


	/****************************************************************************
	 * Name: lib_vscanf
	 *
	 * Description:
	 *  Stream-oriented implementation that underlies scanf family:  scanf,
	 *  fscanf, vfscanf, sscanf, and vsscanf
	 *
	 ****************************************************************************/

	int lib_vscanf(FAR struct lib_instream_s *obj, FAR int *lastc,
	               FAR const IPTR char *src, va_list ap);


#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __LIBC_LIBC_H */

