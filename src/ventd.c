/* vim: set foldmethod=syntax : */

#define _POSIX_C_SORCE 200809L
#define _XOPEN_SOURCE 700

#include "configure.h"
#include "ventd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <argp.h>
#include <assert.h>

#define PROG_NAME "airvent"


char *instance_name=PROG_NAME;
char *pipe_path;
bool ctrl_stdin = false;

static int parse_opt (int key, char *arg, struct argp_state *state) {
  switch (key) {
    case 'i':
      instance_name=malloc(sizeof(PROG_NAME)+strlen(arg)+2);
      sprintf(instance_name, PROG_NAME ".%s", arg);
      break;
    case 's':
      ctrl_stdin=true;
      break;
  }
  return 0;
} 

static int parse_options(int argc, char **argv) {
  struct argp_option options[] = {
    {"instance-name", 'i', "NAME", 0, "Set instance name"},
    {"stdin", 's', 0, 0, "Use standard input as control pipe"},
    {"daemon", 'd', 0 , 0, "Run as a daemon"},
    {0}
  };
  struct argp argp = { options, parse_opt, 0, 0 };
  return argp_parse (&argp, argc, argv, 0, 0, 0);
}

char * readcmd(int fd, size_t len, char separator) {
  int i=0;
  char c;
  char buf[len+1];

  while( i < len ) {
    if( read(fd, &c, sizeof(char)) > 0 ) {
      buf[i]=c;
      if (c==separator) {
        buf[i]='\0'; 
        break;
      }
      i++;
    }
  }
  buf[len]='\0';

  char *cmd = malloc((i>len?i:len)); 
  strncpy(cmd, buf, (i>len?i:len));
  return cmd;
}

void term() {
  syslog(LOG_NOTICE, "instance stopping: %s", instance_name);
  unlink(pipe_path);
  closelog();
}

static void signal_handler(int sig) {
  printf("received signal: %s\n", strsignal(sig));
  switch(sig) {
  case SIGTERM:
  case SIGINT: 
    exit(0);
  }
}

int main (int argc, char **argv) {
  // parse options
  parse_options(argc, argv);

  // Setup signal handlers
  signal(SIGTERM, signal_handler);
  signal(SIGINT, signal_handler);
  atexit(term);

  // Setup logging
  setlogmask (LOG_UPTO (LOG_NOTICE));
  openlog (PROG_NAME, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
  syslog (LOG_NOTICE, "instance started: %s", instance_name);

  // Create control fifo
  pipe_path = malloc(sizeof(PIPE_PATH)+strlen(instance_name)+1);
  sprintf(pipe_path, PIPE_PATH "%s", instance_name); 

  struct stat st;
  if(stat(pipe_path, &st)!=0){
    mknod(pipe_path, S_IFIFO | 0666, 0);
    syslog(LOG_NOTICE, "creating control pipe: %s", pipe_path);
  }
  else {
    syslog(LOG_ERR, "control pipe exists already: %s", pipe_path);
    exit(EXIT_FAILURE);
  }
  int ctrl_fd = open(pipe_path, O_RDONLY | O_NDELAY);
  free(pipe_path);
  assert(ctrl_fd);
  return command_loop(ctrl_fd);
}


