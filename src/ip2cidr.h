/*****
 *
 * Description: IP To CIDR Headers
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

#ifndef IP2CIDR_DOT_H
#define IP2CIDR_DOT_H

/****
 *
 * includes
 *
 ****/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/sysdep.h"

#ifndef __SYSDEP_H__
#error something is messed up
#endif

#include "../include/sysdep.h"
#include "util.h"
#include "mem.h"
#include "sort.h"

/****
 *
 * defines
 *
 ****/

#define LINEBUF_SIZE 4096

#define MASK_23 0xfffffe00
#define MASK_24 0xffffff00
#define MASK_25 0xffffff80
#define MASK_26 0xffffffc0
#define MASK_27 0xffffffe0
#define MASK_28 0xfffffff0
#define MASK_29 0xfffffff8
#define MASK_30 0xfffffffc
#define MASK_31 0xfffffffe
#define MASK_32 0xffffffff

/****
 *
 * consts & enums
 *
 ****/

/****
 *
 * typedefs & structs
 *
 ****/

struct networkList_s
{
  uint32_t *ipv4List;
  uint64_t *ipv6List;
  uint32_t ipv4Count;
  uint32_t ipv6Count;
};

/****
 *
 * function prototypes
 *
 ****/

int processFile(const char *fName);
int consolidateIPv4List(struct networkList_s *netList, uint32_t mask);

#endif /* IP2CIDR_DOT_H */
