/*****
 *
 * Description: Sort Functions
 *
 * Copyright (c) 2008-2023, Ron Dilley
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

/****
 *
 * defines
 *
 ****/

/****
 *
 * includes
 *
 ****/

#include "sort.h"
#include "mem.h"

/****
 *
 * local variables
 *
 ****/

/****
 *
 * external global variables
 *
 ****/

extern Config_t *config;

/****
 *
 * functions
 *
 ****/


/****
 * 
 * swap 32 bit integers
 * 
 ****/

void swap32(uint32_t *a, uint32_t *b) {
  uint32_t t = *a;
  *a = *b;
  *b = t;
}


/****
 *
 * quick sort partition - 32 bit
 *
 ****/

uint32_t quickSortPartition32( uint32_t array[], uint32_t low, uint32_t high)
{
  uint32_t pivot = array[high];
  uint32_t i = (low - 1);

  for (uint32_t j = low; j < high; j++) {
    if (array[j] <= pivot) {
      i++;
      
      swap32(&array[i], &array[j]);
    }
  }

  swap32(&array[i + 1], &array[high]);
  
  return (i + 1);
}

/****
 *
 * quick sort partition - 32 bit
 *
 ****/

void quickSort32( uint32_t array[], uint32_t low, uint32_t high)
{
  if (low < high) {
    uint32_t pi = quickSortPartition32(array, low, high);
    quickSort32(array, low, pi - 1);
    quickSort32(array, pi + 1, high);
  }
}
