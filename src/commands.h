#pragma once
#include "ventd.h"
#define command(name) void name (int argc, char **argv)
#define COUNT_COMMANDS int command_count=sizeof(commands)/sizeof(stringcase_t)

extern stringcase_t commands[];
extern int command_count;

// Prototype for a notfound command
command(notfound);

/* Define commands here */
command(run);
command(spawn);
command(quit);

