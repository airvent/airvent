#pragma once
#include <unistd.h>

typedef struct {
  pid_t pid;
  int argc;
  char **argv;
  int in, out, err;
} child_t;

typedef struct {
  int count;
  child_t *children;
} children_t;

extern children_t children; 
