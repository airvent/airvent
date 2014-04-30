#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700

#include "ventd.h"
#include "commands.h"
#include "configure.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>

// comparator function
int stringcase_cmp(const void *lhs, const void *rhs ) {
  return strcasecmp( 
      ((stringcase_t *) lhs)->string,
      ((stringcase_t *) rhs)->string
  );
}

/**
 * @brief Switch on a string (Case-Insensitive)
 * @param token String on which to switch
 * @param cases Array of stringcase_t structures containing callbacs
 * @param case_count    Number of cases on which to switch
 * @return A function pointer to the callback for the specified case.
 */
void (*string_switch( char* token, stringcase_t *cases, size_t case_count ))(int argc, char **argv) {
  qsort(cases, case_count, sizeof(stringcase_t), stringcase_cmp );

  stringcase_t val;
  val.string=token;

  void* match = bsearch( &val, cases, case_count, sizeof(stringcase_t), stringcase_cmp );
  if (match) {
    stringcase_t* foundVal = (stringcase_t*) match;
    return *foundVal->func;
  } else
  return notfound;
}

/**
 * @brief execute a command
 * @param cmd Null terminated string.
 */
void execute(char *cmd) {
  char ** argv  = NULL;
  char *  p    = strtok (cmd, " ");
  int argc = 0, i;

  /* split string and append tokens to 'argv' */
  while (p) {
    argv = realloc (argv, sizeof (char*) * ++argc);
    if (argv == NULL)
      exit (-1); /* memory allocation failed */
    argv[argc-1] = p;
    p = strtok (NULL, " ");
  }

  /* realloc one extra element for the last NULL */
  argv = realloc (argv, sizeof (char*) * (argc+1));
  argv[argc] = 0;

  argc--;

  /*for (i = 0; i < (argc+1); ++i)
    printf ("argv[%d] = %s\n", i, argv[i]);*/

  string_switch(argv[0], commands, command_count)(argc, argv);

  /* free the memory allocated */
  free (argv);
}

/**
 * @defgroup commands Utility commands
 * @{
 */
/**
 * @brief if the specified command was not found, return a pointer to this function.
 */
command(notfound) {
  printf("not found: %s", argv[0]);
}

/**
 * @brief exit command handler.
 */
command(quit) {
  exit(EXIT_SUCCESS);
}

/**
 * @brief run command handler, for debugging purposes.
 */
command(run) {
  printf("run:");
  for (int i = 1; i <= argc; i++) {
    printf(" %s" , argv[i]); 
  }
  printf("\n");
  string_switch(argv[1], commands, command_count)(argc-1, &(argv[1]) );
}

/** @} */

/**
 * @brief   continuously read on the control file descriptor, and execute
 *          commands received.
 * @param   ctrl_fd
 * @return
 */
int command_loop( int ctrl_fd ) {
  // Wait for commands on the control fifo
  while(1) {
    char *cmd = readcmd(ctrl_fd, 255, '\n');
    execute(cmd);
    free(cmd);
  }
}

