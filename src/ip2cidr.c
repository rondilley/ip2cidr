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

/* hashes */
struct hash_s *templateHash = NULL;

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
  PRIVATE int c = 0, i, ret;
  unsigned int lineCount = 0, lineLen = 0, minLineLen = sizeof(inBuf), maxLineLen = 0, totLineLen = 0;
  unsigned int argCount = 0, totArgCount = 0;
  struct in_addr sa_addr;
  struct in6_addr sa6_addr;

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

    if (reload EQ TRUE)
    {
      fprintf(stderr, "Processed %d lines/min\n", lineCount);
      lineCount = 0;
      reload = FALSE;
    }

    /* test key is IPv4 */
    if (inet_pton(AF_INET, inBuf, &sa_addr) EQ TRUE)
    {
      /* process IPv4 address */
      printf("%s [%u]\n", inBuf, ntohl(sa_addr.s_addr));

    }
    else if (inet_pton(AF_INET6, inBuf, &sa6_addr) EQ TRUE)
    {
      /* IPv6 address, not processed */
      fprintf(stderr, "Ignoring IPv6 address [%s]\n", inBuf);
    }
    else
    {
      /* pass line alone without processing, probably a network range or IPv6 address */
      fprintf(stderr, "Ignoring non-IP address [%s]\n", inBuf);
    }
  }

  fprintf(stderr, "Closing [%s]\n", fName);

  if (inFile != stdin)
    fclose(inFile);

  return (EXIT_SUCCESS);
}