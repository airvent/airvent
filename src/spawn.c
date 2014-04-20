#include "commands.h"
#include <stdio.h>
#include <syslog.h>
#include <dlfcn.h>

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
        plugin_main(argc-1, &(argv[1]));
      }
    printf("spawning:\n");
  }
}
