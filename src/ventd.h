#ifndef VENTD_H
#define VENTD_H

#include <stdlib.h>

typedef enum {
  false=0,
  true=-1
} bool;

typedef struct {
    char* string;
    void (*func)(int artc, char **argv);
} stringcase_t;

void (*string_switch( char* token, stringcase_t *cases, size_t case_count ))(int argc, char **args); 
int command_loop( int ctrl_fd );
char *readcmd(int fd, size_t len, char separator); 

#endif
