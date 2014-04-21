AirVent
=======

a framework.

Running
=======
Simply start airvent by running ventd.

    ventd

Logging
=======
Ventd logs to syslog.

Controlling Instances
=====================

ventctl can be used to control an airvent instance. Any command defined in commands.h and commands.c can be called.
Separate airvent instances can be started using the --instance-name and -i options:

    ventd --instance-name=FOO
    ventctl -i FOO <command>

Plugins
=======

Plugins are shared objects.
Plugins should read on standard input and write on standard output.
The entry point of a plugin must take an argument count and an argument vector as parameters. A sample hello world plugin is included.
A plugin can be forked off using the spawn command:

Syntax:
    ventctl spawn <plugin>

Example:
    ventctl spawn ./plugins/hello.so 


