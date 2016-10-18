#ifndef LIBUFE_TOOLS_H
#define LIBUFE_TOOLS_H 1

#include <string.h>
#include <stdlib.h>

int arg_as_int(char *arg);

int get_arg_short(char arg_name, int argc, char **argv);

int get_arg_long(char* arg_name, int argc, char **argv);

int get_arg(char arg_short, char* arg_long, int argc, char **argv);

int get_arg_val(char arg_short, char* arg_long, int argc, char **argv);

typedef int (*userfunc)(libusb_device_handle*);

int on_device_do(int id, userfunc user_func);

#endif

