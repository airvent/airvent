#include "ventd.h"
#include "configure.h"
#include <stdio.h>
#include <string.h>

// comparator function
int stringcase_cmp(const void *lhs, const void *rhs ) {
  return strcasecmp( 
      ((stringcase_t *) lhs)->string,
      ((stringcase_t *) rhs)->string
  );
}

void notfound(int argc, char **argv) {
  printf("not found: %s", argv[0]);
}

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

void spawn(int argc, char **argv) {
  printf("spawn(");
  for (int i = 1; i <= argc; i++) {
    printf("%s,", argv[i]); 
  }
  printf(");\n");
}

void quit(int argc, char **argv) {
  exit(EXIT_SUCCESS);
}

int command_loop( int ctrl_fd ) {
  stringcase_t cases[] = {
    {"spawn", spawn},
    {"quit", quit}
  };
  size_t count = sizeof(cases)/sizeof(stringcase_t);

  // Wait for commands on the control fifo
  while(1) {
    char *cmd = readcmd(ctrl_fd, 255, '\n');

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

    string_switch(argv[0], cases, count)(argc, argv);

    /* free the memory allocated */
    free (argv);
    free(cmd);
  }
}

