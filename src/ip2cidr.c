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

PRIVATE uint32_t netMasks[] = {
    0x00000000, 0x80000000, 0xc0000000, 0xe0000000, 0xf0000000, 0xf8000000, 0xfc000000, 0xfe000000,
    0xff000000, 0xff800000, 0xffc00000, 0xffe00000, 0xfff00000, 0xfff80000, 0xfffc0000, 0xfffe0000,
    0xffff0000, 0xffff8000, 0xffffc000, 0xffffe000, 0xfffff000, 0xfffff800, 0xfffffc00, 0xfffffe00,
    0xffffff00, 0xffffff80, 0xffffffc0, 0xffffffe0, 0xfffffff0, 0xfffffff8, 0xfffffffc, 0xfffffffe,
    0xffffffff};

PRIVATE uint32_t hostSize[] = {
    4294967294, 2147483648, 1073741824, 536870912, 268435456, 134217728, 67108864, 33554432, 16777216,
    8388608, 4194304, 2097152, 1048576, 524288, 262144, 131072, 65536, 32768, 16384, 8192, 4096, 2048,
    1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1};

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
  struct in6_addr ip6_addr;
  struct in_addr ip_addr;
  uint32_t *ipv4List = NULL, ipv4Count = 0, *tmpPtr;
  struct networkList_s netList;

  if (config->verbose)
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
    if (inet_pton(AF_INET, inBuf, &ip_addr) EQ TRUE)
    {
      /* process IPv4 address */

      /* XXX add to unsorted buffer */
      ipv4Count++;
      if ((tmpPtr = XREALLOC(ipv4List, ipv4Count * sizeof(uint32_t))) EQ NULL)
      {
        display(LOG_ERR, "Unable to allocate memory for IPv4 address buffer");
        if (ipv4List != NULL)
          XFREE(ipv4List);
        return (FAILED);
      }
      ipv4List = tmpPtr;
      ipv4List[ipv4Count - 1] = ntohl(ip_addr.s_addr);
#ifdef DEBUG
      if (config->debug >= 9)
        display(LOG_DEBUG, "%s [%u]", inBuf, ipv4List[ipv4Count - 1]);
#endif
    }
    else if (inet_pton(AF_INET6, inBuf, &ip6_addr) EQ TRUE)
    {
      /* IPv6 address, not processed */
      if (config->verbose)
        fprintf(stderr, "IPv6 address [%s] sent to output without processing\n", inBuf);
      printf("%s\n", inBuf);
    }
    else
    {
      /* pass line alone without processing, probably a network range or IPv6 address */
      if (config->verbose)
        fprintf(stderr, "Non-IP address [%s] sent to output without processing\n", inBuf);
      printf("%s\n", inBuf);
    }
  }

  /* sort ipv4 list */
  if (config->verbose)
    display(LOG_INFO, "Sorting IPv4 List");
  quickSort32(ipv4List, 0, ipv4Count - 1);

  /* bitmask summarization */
  if (config->verbose)
    display(LOG_INFO, "Consolidating IPs to CIDRs");

  netList.ipv4List = ipv4List;
  netList.ipv4Count = ipv4Count;

  if (config->verbose)
    fprintf(stderr, "Starting IP list size [%d]\n", netList.ipv4Count);

  for (int mask = config->minBits; mask <= config->maxBits; ++mask)
  {
    if (consolidateIPv4List(&netList, mask) EQ EXIT_FAILURE)
    {
      display(LOG_ERR, "Problem consolidating to CIDR");
      if (netList.ipv4List != NULL)
        XFREE(netList.ipv4List);
      return (FAILED);
    }
  }

  if (config->verbose)
    fprintf(stderr, "Sending remaining IP addresses to output\n");

  /* print what is left after consolidation */
  for (uint32_t i = 0; i < netList.ipv4Count; ++i)
  {
    ip_addr.s_addr = htonl(netList.ipv4List[i]);
    printf("%s/32\n", inet_ntoa(ip_addr));
  }

  /* cleanup memory */
  if (netList.ipv4List != NULL)
    XFREE(netList.ipv4List);

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
  PRIVATE uint32_t curNet = 0, curCount = 0, curStart = 0, curEnd = 0, network;
  uint32_t *list = netList->ipv4List;
  uint32_t *newList, newListCount = 0;
  struct in_addr ip_addr, mask_addr;
  char ipAddr[INET_ADDRSTRLEN], netAddr[INET_ADDRSTRLEN];

  if ((newList = XMALLOC(netList->ipv4Count * sizeof(uint32_t))) EQ NULL)
  {
    display(LOG_ERR, "Unable to allocate memory for new list");
    return (EXIT_FAILURE);
  }

  if (config->verbose)
    fprintf(stderr, "Consolidating /%d\n", mask);

  for (uint32_t i = 0; i < netList->ipv4Count; ++i)
  {
    network = netList->ipv4List[i] & netMasks[mask];

    if (i EQ 0)
      curNet = network;
    else if (network != curNet)
    {
      /* network has changed, process the count */
      curEnd = i - 1;

      if (((float)curCount / (float)hostSize[mask]) > config->threshold)
      {
        mask_addr.s_addr = htonl(curNet);
        sprintf(netAddr, "%s", inet_ntoa(mask_addr));
#ifdef DEBUG
        if (config->debug >= 1)
          display(LOG_DEBUG, "Consolidating to %s/%d (%d)", netAddr, mask, curCount);
#endif

        printf("%s/%d\n", netAddr, mask);
      }
      else
      {
        for (uint32_t x = curStart; x < i; ++x)
          newList[newListCount++] = list[x];
      }

      /* starting a new network block */
      curNet = network;
      curCount = 0;
      curStart = i;
    }
    curCount++;
  }

  /* switch to new shorter list */
  netList->ipv4List = newList;
  netList->ipv4Count = newListCount;

  /* cleanup old list */
  if (list != NULL)
    XFREE(list);

  return (EXIT_SUCCESS);
}