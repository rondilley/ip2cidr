/*****
 *
 * Common definitions, structures and unions
 *
 * BSD 3-Clause License
 *
 * Copyright (c) 2008-2023, Ron Dilley
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****/

#ifndef COMMON_H
#define COMMON_H 1

/****
 *
 * defines
 *
 ****/

#define FAILED -1
#define FALSE 0
#define TRUE 1

#ifndef EXIT_SUCCESS
# define EXIT_SUCCESS 0
# define EXIT_FAILURE 1
#endif

#define MODE_DAEMON 0
#define MODE_INTERACTIVE 1
#define MODE_DEBUG 2

#define PRIVATE static
#define PUBLIC
#define EQ ==
#define NE !=

#ifndef PATH_MAX
# ifdef MAXPATHLEN
#  define PATH_MAX MAXPATHLEN
# else
#  define PATH_MAX 1024
# endif
#endif

#ifdef __cplusplus
# define BEGIN_C_DECLS extern "C" {
# define END_C_DECLS }
#else /* !__cplusplus */
# define BEGIN_C_DECLS
# define END_C_DECLS
#endif

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "../include/sysdep.h"

#ifndef __SYSDEP_H__
# error something is messed up
#endif

/****
 *
 * enums & typedefs
 *
 ****/

typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long dword;

/* prog config */

typedef struct {
  FILE *syslog_st;
  time_t current_time;
  int debug;
  int mode;
  int facility;
  int priority;
  int verbose;
  pid_t cur_pid;
  float threshold;
  int minBits;
  int maxBits;
} Config_t;

#endif	/* end of COMMON_H */

