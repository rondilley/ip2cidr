/*****
 *
 * Description: Sort Functions
 *
 * Copyright (c) 2008-2025, Ron Dilley
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

#define INSERTION_SORT_THRESHOLD 16
#define MEDIAN_OF_THREE_THRESHOLD 8

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
 * insertion sort for small arrays - 32 bit
 *
 ****/

void insertionSort32(uint32_t array[], uint32_t low, uint32_t high) {
  for (uint32_t i = low + 1; i <= high; i++) {
    uint32_t key = array[i];
    uint32_t j = i;
    
    while (j > low && array[j - 1] > key) {
      array[j] = array[j - 1];
      j--;
    }
    array[j] = key;
  }
}

/****
 *
 * median of three pivot selection - 32 bit
 *
 ****/

uint32_t medianOfThree32(uint32_t array[], uint32_t low, uint32_t high) {
  uint32_t mid = low + (high - low) / 2;
  
  if (array[mid] < array[low]) {
    swap32(&array[low], &array[mid]);
  }
  if (array[high] < array[low]) {
    swap32(&array[low], &array[high]);
  }
  if (array[high] < array[mid]) {
    swap32(&array[mid], &array[high]);
  }
  
  /* Place median at the end */
  swap32(&array[mid], &array[high]);
  return high;
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
    /* Use insertion sort for small arrays */
    if (high - low < INSERTION_SORT_THRESHOLD) {
      insertionSort32(array, low, high);
      return;
    }
    
    /* Use median-of-three pivot selection for larger arrays */
    if (high - low >= MEDIAN_OF_THREE_THRESHOLD) {
      medianOfThree32(array, low, high);
    }
    
    uint32_t pi = quickSortPartition32(array, low, high);
    /* Avoid underflow on unsigned integers */
    if (pi > 0) {
      quickSort32(array, low, pi - 1);
    }
    if (pi < high) {
      quickSort32(array, pi + 1, high);
    }
  }
}

/****
 *
 * quick sort with progress tracking - 32 bit
 *
 ****/

void quickSort32WithProgress( uint32_t array[], uint32_t low, uint32_t high, progress_info_t *progress)
{
  if (low < high) {
    /* Use insertion sort for small arrays */
    if (high - low < INSERTION_SORT_THRESHOLD) {
      insertionSort32(array, low, high);
      return;
    }
    
    /* Use median-of-three pivot selection for larger arrays */
    if (high - low >= MEDIAN_OF_THREE_THRESHOLD) {
      medianOfThree32(array, low, high);
    }
    
    uint32_t pi = quickSortPartition32(array, low, high);
    
    /* Update progress counter */
    progress->recursion_calls++;
    
    /* Check if we should print progress (every 60 seconds) */
    if (config->verbose) {
      time_t current_time = time(NULL);
      if (current_time - progress->last_progress_time >= 60) {
        double elapsed = difftime(current_time, progress->start_time);
        fprintf(stderr, "Sorting progress: %u partitions processed (%.0f seconds elapsed)\n", 
                progress->recursion_calls, elapsed);
        progress->last_progress_time = current_time;
      }
    }
    
    /* Avoid underflow on unsigned integers */
    if (pi > 0) {
      quickSort32WithProgress(array, low, pi - 1, progress);
    }
    if (pi < high) {
      quickSort32WithProgress(array, pi + 1, high, progress);
    }
  }
}

/****
 *
 * radix sort for 32-bit integers - much faster for IP addresses
 *
 ****/

void radixSort32(uint32_t a[], uint32_t n) {
  if (n <= 1) return;
  
  uint32_t *output = (uint32_t *)XMALLOC(n * sizeof(uint32_t));
  if (!output) return;
  
  /* Sort by each byte (8 bits) - 4 passes for 32-bit integers */
  for (int shift = 0; shift < 32; shift += 8) {
    uint32_t count[256] = {0};
    
    /* Count occurrences of each byte value */
    for (uint32_t i = 0; i < n; i++) {
      count[(a[i] >> shift) & 0xFF]++;
    }
    
    /* Calculate cumulative counts */
    for (int i = 1; i < 256; i++) {
      count[i] += count[i - 1];
    }
    
    /* Build output array */
    for (int i = n - 1; i >= 0; i--) {
      uint32_t byte = (a[i] >> shift) & 0xFF;
      output[count[byte] - 1] = a[i];
      count[byte]--;
    }
    
    /* Copy output back to input array */
    for (uint32_t i = 0; i < n; i++) {
      a[i] = output[i];
    }
  }
  
  XFREE(output);
}

/****
 *
 * radix sort with progress tracking - 32 bit
 *
 ****/

void radixSort32WithProgress(uint32_t a[], uint32_t n, progress_info_t *progress) {
  if (n <= 1) return;
  
  uint32_t *output = (uint32_t *)XMALLOC(n * sizeof(uint32_t));
  if (!output) return;
  
  /* Sort by each byte (8 bits) - 4 passes for 32-bit integers */
  for (int shift = 0; shift < 32; shift += 8) {
    uint32_t current_pass = (shift / 8) + 1;
    
    /* Check if we should print progress */
    if (config->verbose) {
      time_t current_time = time(NULL);
      if (current_time - progress->last_progress_time >= 60) {
        double elapsed = difftime(current_time, progress->start_time);
        double percent = (double)current_pass / 4.0 * 100.0;
        fprintf(stderr, "Sorting progress: %.1f%% complete (pass %u of 4, %.0f seconds elapsed)\n", 
                percent, current_pass, elapsed);
        progress->last_progress_time = current_time;
      }
    }
    
    uint32_t count[256] = {0};
    
    /* Count occurrences of each byte value */
    for (uint32_t i = 0; i < n; i++) {
      count[(a[i] >> shift) & 0xFF]++;
    }
    
    /* Calculate cumulative counts */
    for (int i = 1; i < 256; i++) {
      count[i] += count[i - 1];
    }
    
    /* Build output array */
    for (int i = n - 1; i >= 0; i--) {
      uint32_t byte = (a[i] >> shift) & 0xFF;
      output[count[byte] - 1] = a[i];
      count[byte]--;
    }
    
    /* Copy output back to input array */
    for (uint32_t i = 0; i < n; i++) {
      a[i] = output[i];
    }
  }
  
  XFREE(output);
}
