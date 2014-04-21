#include "commands.h"
#include "spawn.h"
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <dlfcn.h>
#include <argp.h>
#include <assert.h>
#include <string.h>

children_t processes;

void print_children(children_t *list) {
  printf("%p=[", list);
  for (int i = 0; i < list->count; i++) {
    printf(" %d", list->children[i].pid);
  }
  printf(" ]\n");
}

void insert_child(children_t *list, child_t child) {
  if(!list->children) {
    assert(list->count == 0);
    list->children = malloc(sizeof(child_t));
  } else
    list->children = realloc(list->children, (list->count+1)*sizeof(child_t));
  list->children[list->count] = child;
  list->count++;
}

// Comparator to sort list by pid
int child_compare( void const * lhs, void const * rhs ) {
  pid_t left  = ((child_t *) lhs)->pid;
  pid_t right = ((child_t *) rhs)->pid;

  if( left < right ) return -1;
  if( left > right ) return  1;

  return 0;  /* left == right */
}

child_t* lookup_child(children_t *list, pid_t pid) {
  qsort(list->children, list->count, sizeof(child_t), child_compare);
  child_t key;
  key.pid=pid;
  child_t *child = bsearch(&key, list->children, list->count, sizeof(child_t), child_compare);
  return child;
}

void remove_child(children_t *list, pid_t pid) {
  child_t *child = lookup_child(list, pid);
  memset(child, 0, sizeof(child_t));
  qsort(list->children, list->count, sizeof(child_t), child_compare );

  list->count--;
  child_t *new_list = malloc( sizeof(child_t)*(list->count) );
  new_list = memcpy(new_list, &(list->children[1]), sizeof(child_t)*(list->count) );
  free(list->children);
  list->children = new_list;
}

struct spawn_arguments {
  char *entry;
  int argc;
  char **argv;
};

int spawn_option_parse(int key, char *arg, struct argp_state *state) {
  struct spawn_arguments *arguments = state->input;
  switch(key) {
    case 'e':
      arguments->entry = arg;
      printf("entry: %s\n", arg);
      break;

    case ARGP_KEY_ARG:
      /* Make sure that argv is large enough to hold the arguments */
      if (!arguments->argv)
        arguments->argv = malloc((state->arg_num+1)*sizeof( char* ));
      else
        arguments->argv = realloc(arguments->argv, (state->arg_num+1)*sizeof( char* ));
      arguments->argv[state->arg_num] = arg;
      arguments->argc = state->arg_num;
      break;

    default:
      return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

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
    remove_child(&processes, pid);
    syslog(LOG_NOTICE, "%d exited and returned %d\n", pid, child_val);
  }
}

pid_t fork_process(int (*entry)(int argc, char **argv), int argc, char **argv, children_t list) {
  /* fork off a process to run the plugin */
  signal(SIGCHLD, sig_chld);
  pid_t pid = fork();
  if (pid==0){
    /* in child process */
    entry(argc,argv);
    // C99 exit here without calling the atexit handler
    _Exit(0);
  }
  else if (pid<0)
    syslog(LOG_ERR, "could not fork %s", argv[0]);
  else {
    /* in parent process */
    child_t process;
    process.pid=pid;
    process.argc=argc;
    process.argv=argv;
    insert_child(&processes, process);
    syslog(LOG_NOTICE, "forked %s as pid %d", argv[0], pid);
  }
  return pid;
}

command(spawn) {
  /* Option Parsing */
  struct spawn_arguments arguments;
  arguments.argc=0;
  arguments.entry="main";
  arguments.argv = NULL;
  static struct argp_option options[] = {
    {"entry", 'e', "SYMBOL", 0, "Use SYMBOL as entry point"},
    {0}
  };

  static struct argp argp = { options, spawn_option_parse, 0, 0 }; 
  if (argp_parse (&argp, argc+1, argv, ARGP_SILENT | ARGP_NO_ERRS, 0, &arguments)) {
    /*
     * If arguments couldn't be parsed, revert to defaults:
     *   arguments.argc = argc-1
     *   arguments.argv = argv[1:argc]
     *   arguments.entry = entry="main"
     */
    syslog(LOG_WARNING, "could not parse options, using defaults");
    arguments.argc = argc-1;
    arguments.argv = malloc(argc*sizeof( char* ));
    arguments.argv = memcpy(arguments.argv, &(argv[1]), argc*sizeof( char* ));
  }

  /* Open the object */
  char *object = arguments.argv[0];
  void *plugin = dlopen(object, RTLD_LAZY);
  if (!plugin)
    syslog(LOG_ERR, "Unable to load \'%s\'", object);
  else {
    void* initializer = dlsym(plugin, arguments.entry);
    if (initializer == NULL) {
      syslog(LOG_ERR, "Unable to extract initializer \'%s\' from \'%s\'", arguments.entry, object);
      } else {
        int (*plugin_main)(int argc, char **argv) = (int (*)(int argc, char **argv)) initializer;

        /* fork off a process to run the plugin */
        fork_process(plugin_main, argc, argv, processes);

        free(arguments.argv);
      }
  }
}
