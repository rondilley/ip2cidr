/*****
 *
 * Description: Sort Function Headers
 *
 * Copyright (c) 2009-2025, Ron Dilley
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ****/

#ifndef SORT_DOT_H
#define SORT_DOT_H

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

#include "mem.h"
#include "util.h"
#include "../include/common.h"
#include <stdint.h>
#include <time.h>

/****
 *
 * typedefs and enums
 *
 ****/

typedef struct {
    time_t start_time;
    time_t last_progress_time;
    uint32_t total_elements;
    uint32_t recursion_calls;
} progress_info_t;

/****
 *
 * function prototypes
 *
 ****/

uint32_t quickSortPartition32( uint32_t a[], uint32_t low, uint32_t high);
void quickSort32(uint32_t a[], uint32_t low, uint32_t high);
void quickSort32WithProgress(uint32_t a[], uint32_t low, uint32_t high, progress_info_t *progress);
void radixSort32(uint32_t a[], uint32_t n);
void radixSort32WithProgress(uint32_t a[], uint32_t n, progress_info_t *progress);

#endif /* end of SORT_DOT_H */
