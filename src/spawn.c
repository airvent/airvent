#include "commands.h"
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <dlfcn.h>

void sig_chld(int signo) {
  int status, child_val;
  pid_t pid = waitpid(-1, &status, WNOHANG) ;
  /* Wait for any child without blocking */
  if (pid < 0) {
    syslog(LOG_ERR, "waitpid failed.");
    return;
  }
  if (WIFEXITED(status)) {
    child_val = WEXITSTATUS(status); /* get child's exit status */
    syslog(LOG_NOTICE, "%d exited and returned %d\n", pid, child_val);
  }
}


command(spawn) {
  void *plugin = dlopen(argv[1], RTLD_LAZY);
  if (!plugin)
    syslog(LOG_ERR, "Unable to load \'%s\'", argv[1]);
  else {
    void* initializer = dlsym(plugin,(argc==2)?argv[2]:"main");
    if (initializer == NULL) {
      syslog(LOG_ERR, "Unable to extract initializer \'%s\' from \'%s\'", (argc==2)?argv[2]:"main", argv[1]);
      } else {
        int (*plugin_main)(int argc, char **argv);
        plugin_main = (int (*)(int argc, char **argv)) initializer;
        signal(SIGCHLD, sig_chld);
        pid_t pid = fork();
        if (pid==0){ 
          plugin_main(argc-1, &(argv[1]));
          // C99 exit here without calling the atexit handler
          syslog(LOG_NOTICE, "terminated.");
          _Exit(0); 
        }
        else if (pid<0)
          syslog(LOG_ERR, "could not fork %s", argv[1]);
        else
          printf("forked %s as pid %d\n", argv[1], pid);
          syslog(LOG_NOTICE, "forked %s as pid %d", argv[1], pid);
      }
  }
}
