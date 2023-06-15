/*****
 *
 * Description: IP To CIDR Functions
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

/****
 *
 * includes
 *
 ****/

#include "ip2cidr.h"

/****
 *
 * local variables
 *
 ****/

/****
 *
 * global variables
 *
 ****/

/****
 *
 * external variables
 *
 ****/

extern int errno;
extern char **environ;
extern Config_t *config;
extern int quit;
extern int reload;

/****
 *
 * functions
 *
 ****/

/****
 *
 * process file
 *
 ****/

int processFile(const char *fName)
{
  FILE *inFile = NULL, *outFile = NULL;
  char inBuf[8192];
  char outFileName[PATH_MAX];
  char patternBuf[4096];
  char oBuf[4096];
  // PRIVATE int c = 0, i, ret;
  unsigned int lineCount = 0, lineLen = 0, minLineLen = sizeof(inBuf), maxLineLen = 0, totLineLen = 0;
  unsigned int argCount = 0, totArgCount = 0;
  struct in_addr sa_addr;
  struct in6_addr sa6_addr;
  struct in_addr ip_addr;
  uint32_t *ipv4List = NULL, ipv4Count = 0, *tmpPtr;
  struct networkList_s netList;

  fprintf(stderr, "Opening [%s] for read\n", fName);

  if (strcmp(fName, "-") EQ 0)
  {
    inFile = stdin;
  }
  else
  {
#ifdef HAVE_FOPEN64
    if ((inFile = fopen64(fName, "r")) EQ NULL)
    {
#else
    if ((inFile = fopen(fName, "r")) EQ NULL)
    {
#endif
      fprintf(stderr, "ERR - Unable to open file [%s] %d (%s)\n", fName, errno, strerror(errno));
      return (EXIT_FAILURE);
    }
  }

  while (fgets(inBuf, sizeof(inBuf), inFile) != NULL && !quit)
  {
    /* strip trailing <CR><LF> */
    inBuf[strcspn(inBuf, "\r\n")] = 0;

    /* test key is IPv4 */
    if (inet_pton(AF_INET, inBuf, &sa_addr) EQ TRUE)
    {
      /* process IPv4 address */

      /* XXX add to unsorted buffer */
      ipv4Count++;
      if ((tmpPtr = realloc(ipv4List, ipv4Count * sizeof(uint32_t))) EQ NULL)
      {
        display(LOG_ERR, "Unable to allocate memory for IPv4 address buffer");
        if (ipv4List != NULL)
          XFREE(ipv4List);
        return (FAILED);
      }
      ipv4List = tmpPtr;
      ipv4List[ipv4Count - 1] = ntohl(sa_addr.s_addr);
#ifdef DEBUG
      if (config->debug >= 9)
        display(LOG_DEBUG, "%s [%u]", inBuf, ipv4List[ipv4Count - 1]);
#endif
    }
    else if (inet_pton(AF_INET6, inBuf, &sa6_addr) EQ TRUE)
    {
      /* IPv6 address, not processed */
      if (config->verbose)
        fprintf(stderr, "Ignoring IPv6 address [%s]\n", inBuf);
      printf("%s\n", inBuf);
    }
    else
    {
      /* pass line alone without processing, probably a network range or IPv6 address */
      if (config->verbose)
        fprintf(stderr, "Ignoring non-IP address [%s]\n", inBuf);
      printf("%s\n", inBuf);
    }
  }

  /* sort ipv4 list */
  if (config->verbose)
    display(LOG_INFO, "Sorting IPv4 List");
  quickSort32(ipv4List, 0, ipv4Count - 1);

#ifdef DEBUG
  if (config->debug >= 7)
  {

    for (uint32_t i = 0; i < ipv4Count; ++i)
    {
      ip_addr.s_addr = htonl(ipv4List[i]);
      display(LOG_DEBUG, "[%u] %s (%u)", i, inet_ntoa(ip_addr), ipv4List[i]);
    }
  }
#endif

  /* bitmask summarization */
  if (config->verbose)
    display(LOG_INFO, "Consolidating IPs to CIDRs");
  netList.ipv4List = ipv4List;
  netList.ipv4Count = ipv4Count;
  if (config->verbose)
    fprintf(stderr, "Starting IP list size [%d]\n", netList.ipv4Count);
  for (int mask = config->minBits; mask <= config->maxBits; ++mask)
  {
    if (config->verbose)
      display(LOG_INFO, "Processing /%d bit mask", mask);

    if (consolidateIPv4List(&netList, mask) EQ EXIT_FAILURE)
    {
      display(LOG_ERR, "Problem consolidating to CIDR");
      if (ipv4List != NULL)
        XFREE(ipv4List);
      return (FAILED);
    }
  }

  /* cleanup memory */
  if (ipv4List != NULL)
    XFREE(ipv4List);

  if (config->verbose)
    display(LOG_INFO, "[%d] IPv4 addresses processed", ipv4Count);

  if (config->verbose)
    fprintf(stderr, "Closing [%s]\n", fName);

  if (inFile != stdin)
    fclose(inFile);

  return (EXIT_SUCCESS);
}

/****
 *
 * consolidate ipv4 list to cidr blocks
 *
 ****/

int consolidateIPv4List(struct networkList_s *netList, uint32_t mask)
{
  uint32_t curNet = 0, curStart = 0, curCount = 0, curEnd = 0, network, i;
  uint32_t count = netList->ipv4Count;
  uint32_t *list = netList->ipv4List;
  uint32_t *tmpPtr;
  uint32_t *srcPtr, *dstPtr, moveSize;
  struct in_addr ip_addr, mask_addr;
  char ipAddr[INET_ADDRSTRLEN], netAddr[INET_ADDRSTRLEN];

  if (config->verbose)
    fprintf(stderr, "Consolidating /%d\n", mask);

  while (i <= count)
  {
    // if (list[i] != 0)
    //{
    network = (uint32_t)list[i] & (uint32_t)netMasks[mask];

    if (i EQ 0)
      curNet = network;
    else if (network != curNet)
    {
      /* process the count */
      curEnd = i - 1;

#ifdef DEBUG
      if (config->debug >= 5)
        display(LOG_DEBUG, "OFF: %d START: %d END: %d COUNT: %d\n", i, curStart, curEnd, curCount);
#endif

#ifdef DEBUG
      if (config->debug >= 7)
        display(LOG_DEBUG, "%f %f %f", (float)curCount, (float)hostSize[mask], config->threshold);
#endif

      if (((float)curCount / (float)hostSize[mask]) > config->threshold)
      {
        mask_addr.s_addr = htonl(curNet);
        sprintf(netAddr, "%s", inet_ntoa(mask_addr));
#ifdef DEBUG
        if (config->debug >= 1)
          display(LOG_DEBUG, "Consolidating to %s/%d (%d)", netAddr, mask, curCount);
#endif

        printf("%s/%d\n", netAddr, mask);

        /* remove all IPs associated with this CIDR */
        if (curEnd < count)
        {
#ifdef DEBUG
          if (config->debug >= 5)
            display(LOG_DEBUG, "MOVE: DST: %d SRC: %d CNT: %d", curStart, curStart + curCount, count - i);
#endif
          /* XXX memmove would probably be faster */
          // memmove(list + ((i - curCount) * sizeof(uint32_t)), list + (i * sizeof(uint32_t)), (count - i) * sizeof(uint32_t));
          /* slow move */
          for (uint32_t x = i; x < count; ++x)
            list[x - curCount] = list[x];
          /* XXX just zero out the consolidated IPs */
          // for (uint32_t x = curStart; x < i; ++x)
          //   list[x] = 0;
          i = curStart;
          count -= curCount;
        }
        else
        {
          if (config->verbose)
            display(LOG_INFO, "Not moving memory, at end of IP list");
        }

        // if ((tmpPtr = realloc(list, (count - curCount) * sizeof(uint32_t))) EQ NULL)
        //{
        //   display(LOG_ERR, "Unable to allocate memory for IP list");
        //   netList->ipv4List = list;
        //   netList->ipv4Count = count;
        //   return (EXIT_FAILURE);
        // }
        // list = tmpPtr;
      }

      /* starting a new network block */
      curNet = network;
      curCount = 0;
      curStart = i;
    }
    curCount++;
    //}
    i++;
  }

  // netList->ipv4List = list;
  netList->ipv4Count = count;

  return (EXIT_SUCCESS);
}