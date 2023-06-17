/*****
 *
 * Description: Sort Function Headers
 *
 * Copyright (c) 2009-2023, Ron Dilley
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

/****
 *
 * typedefs and enums
 *
 ****/

/****
 *
 * function prototypes
 *
 ****/

uint32_t quickSortPartition32( uint32_t a[], uint32_t low, uint32_t high);
void quickSort32(uint32_t a[], uint32_t low, uint32_t high);

#endif /* end of SORT_DOT_H */
