#include <stdio.h>
#include <argp.h>

/* genudfimage, the command-line tool */


/* Command-line option parsing. */
/* argp */

//const char * argp_program_version = VERSION;
const char * argp_program_version = "0.0.1";

/* Send reports to this email address. */
// const char * argp_program_bug_address = "bugs@?...";

struct argp_option argp_options[] = {
      { "license", 0, NULL, 0, "Display license information" },
      { 0 }
};


static
error_t argp_onparse (int key, char *arg, struct argp_state * state)
{
  void * cb_data = state->input;

  return 0;
}

const char argp_args [] = "[-o OUTPUT] [OPTIONS] file [file [...]]\n\
[-o OUTPUT] [OPTIONS] dir";

// PREAMBLE + "\v" + POSTAMBLE
const char * argp_docs = NULL;

struct argp argp_desc = { argp_options, argp_onparse, argp_args };




/*
   Aliased main().
   Allows easier building of library by leaving out main.c/main.o.
   Eases testing, which has its own main().
*/
int genudfimage_main (int argc, char * argv[])
{
  int nonopt;  /* first index of non-option argument -- the list of files. */
//  argp_parse(&argp_desc, argc, argv, 0, &nonopt, NULL);
  argp_parse(&argp_desc, argc, argv, 0, &nonopt, NULL);
  return 0;
}

#ifdef GENUDFIMAGE_STANDALONE
int main (int argc, char *argv[]) { return genudfimage_main(argc, argv); }
#endif // GENUDFIMAGE_STANDALONE
