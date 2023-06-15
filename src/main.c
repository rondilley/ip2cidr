/*****
 *
 * Description: Main Functions
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
 *.
 ****/

#include "main.h"

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

PUBLIC int quit = FALSE;
PUBLIC int reload = FALSE;
PUBLIC Config_t *config = NULL;

/****
 *
 * external variables
 *
 ****/

extern int errno;
extern char **environ;

/****
 *
 * main function
 *
 ****/

int main(int argc, char *argv[])
{
  PRIVATE int c = 0;

#ifndef DEBUG
  struct rlimit rlim;

  rlim.rlim_cur = rlim.rlim_max = 0;
  setrlimit(RLIMIT_CORE, &rlim);
#endif

  /* setup config */
  config = (Config_t *)XMALLOC(sizeof(Config_t));
  XMEMSET(config, 0, sizeof(Config_t));

  /* force mode to forground */
  config->mode = MODE_INTERACTIVE;

  /* store current pid */
  config->cur_pid = getpid();

  while (1)
  {
    int this_option_optind = optind ? optind : 1;
#ifdef HAVE_GETOPT_LONG
    int option_index = 0;
    static struct option long_options[] = {
        {"verbose", no_argument, 0, 'V'},
                {"version", no_argument, 0, 'v'},
        {"debug", required_argument, 0, 'd'},
        {"help", no_argument, 0, 'h'},
        {"hbit", required_argument, 0, 'H'},
        {"lbit", required_argument, 0, 'l'},
        {"thold", required_argument, 0, 't'},
        {0, no_argument, 0, 0}};
    c = getopt_long(argc, argv, "Vvd:hH:l:t:", long_options, &option_index);
#else
    c = getopt(argc, argv, "Vvd:hH:l:t:");
#endif

    if (c EQ - 1)
      break;

    switch (c)
    {

    case 'v':
      /* show the version */
      print_version();
      return (EXIT_SUCCESS);

    case 'V':
      /* enable verbose mode */
      config->verbose = TRUE;
      break;
      
    case 'd':
      /* show debig info */
      config->debug = atoi(optarg);
      break;

    case 'h':
      /* show help info */
      print_help();
      return (EXIT_SUCCESS);

    case 'H':
      /* max network bits */
      config->maxBits = atoi(optarg);
      break;

    case 'l':
      /* min network bits */
      config->minBits = atoi(optarg);
      break;

    case 't':
      /* consolidation threshold */
      config->threshold = atof(optarg) / 100;
      break;

    default:
      fprintf(stderr, "Unknown option code [0%o]\n", c);
    }
  }

  /* set required defaults if not defined */
  if (config->threshold EQ 0)
    config->threshold = DEFAULT_THRESHOLD;

  if ( config->minBits EQ 0)
    config->minBits = DEFAULT_MIN_BITS;

  if (config->maxBits EQ 0)
    config->maxBits = DEFAULT_MAX_BITS;

  /*
   * get to work
   */

  /* process all the files */
  while (optind < argc)
  {
    processFile(argv[optind++]);
  }

  /*
   * finished with the work
   */

  cleanup();

  return (EXIT_SUCCESS);
}

/****
 *
 * display prog info
 *
 ****/

void show_info(void)
{
  fprintf(stderr, "%s v%s [%s - %s]\n", PROGNAME, VERSION, __DATE__, __TIME__);
  fprintf(stderr, "By: Ron Dilley\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "%s comes with ABSOLUTELY NO WARRANTY.\n", PROGNAME);
  fprintf(stderr, "This is free software, and you are welcome\n");
  fprintf(stderr, "to redistribute it under certain conditions;\n");
  fprintf(stderr, "See the GNU General Public License for details.\n");
  fprintf(stderr, "\n");
}

/*****
 *
 * display version info
 *
 *****/

PRIVATE void print_version(void)
{
  printf("%s v%s [%s - %s]\n", PROGNAME, VERSION, __DATE__, __TIME__);
}

/*****
 *
 * print help info
 *
 *****/

PRIVATE void print_help(void)
{
  print_version();

  fprintf(stderr, "\n");
  fprintf(stderr, "syntax: %s [options] filename [filename ...]\n", PACKAGE);

#ifdef HAVE_GETOPT_LONG
  fprintf(stderr, " -d|--debug (0-9)       enable debugging info\n");
  fprintf(stderr, " -h|--help              this info\n");
  fprintf(stderr, " -H|--hbit {bits}       max network bits (default: 31)\n");
  fprintf(stderr, " -l|--lbit {bits}       min network bits (default: 24)\n");
  fprintf(stderr, " -t|--thold {percent}   consolidation threshold (default: 51)\n");
  fprintf(stderr, " -v|--version           display version information\n");
  fprintf(stderr, " filename               one or more files to process, use '-' to read from stdin\n");
#else
  fprintf(stderr, " -d {lvl}               enable debugging info\n");
  fprintf(stderr, " -h                     this info\n");
  fprintf(stderr, " -H {bits}              max network bits (default: 31)\n");
  fprintf(stderr, " -l {bits}              min network bits (default: 24)\n");
  frptinf(stderr, " -t|--thold {percent}   consolidation threshold (default: 51)\n");
  fprintf(stderr, " -v                     display version information\n");
  fprintf(stderr, " filename               one or more files to process, use '-' to read from stdin\n");
#endif

  fprintf(stderr, "\n");
}

/****
 *
 * cleanup
 *
 ****/

PRIVATE void cleanup(void)
{
#ifdef MEM_DEBUG
  XFREE_ALL();
#else
  XFREE(config);
#endif
}
