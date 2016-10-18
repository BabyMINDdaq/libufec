#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include"libufe.h"
#include"libufe-tools.h"

int arg_as_int(char *arg) {
  int my_arg;
  if (arg[0] == '0' && arg[1] == 'x')
    my_arg = strtol(arg+2, NULL, 16);
  else
    my_arg = atoi(arg);

  return my_arg;
}


int get_arg_short(char arg_name, int argc, char **argv) {
  int i;
  for (i = 1; i < argc; i++)
    if (argv[i][0] == '-')
      if ( arg_name == argv[i][1] && argv[i][2] == '\0' && argc > i) {
//         printf("arg: -%s\n", argv[i]);
        return i;
      }

  return 0;
}

int get_arg_long(char* arg_name, int argc, char **argv) {
  int i;
  for (i = 1; i < argc; i++)
    if (argv[i][0] == '-' && argv[i][1] == '-')
      if ( strcmp(argv[i]+2, arg_name) == 0 && argc > i) {
//         printf("arg: -%s\n", argv[i]);
        return i;
      }

  return 0;
}

int get_arg(char arg_short, char* arg_long, int argc, char **argv) {
  int x_arg = get_arg_short(arg_short, argc, argv);
  if (x_arg == 0)
    x_arg = get_arg_long(arg_long, argc, argv);

//   printf("arg %i\n", x_arg);
  return x_arg;
}

int get_arg_val(char arg_short, char* arg_long, int argc, char **argv) {
  int x_arg = get_arg(arg_short, arg_long, argc, argv);
  if (x_arg != 0 && x_arg+1 < argc)
    return x_arg + 1;

  return 0;
}

int on_device_do(int id, userfunc user_func) {

  libusb_device_handle *dev_handle; //a device handle
  libusb_context *ctx = NULL; //a libusb session
  int status; //for return values

  status = libusb_init(&ctx); //initialize the library for the session we just declared
  if(status < 0) {
    fprintf(stderr, "!!! Error: init Error. %i\n", status); //there was an error
    return 1;
  }

  libusb_device **febs;
  size_t n_febs = ufe_get_device_list(ctx, &febs);

  if (n_febs == 0) {
    fprintf(stderr, "!!! Error: no UFE boards found. %i\n", status); //there was an error
    libusb_exit(ctx);
    return 1;
  }

#ifdef UFE_DEBUG
  printf("### Debug: UFE boards found: %zu\n", n_febs);
#endif

  int i;
  for (i=0; i<n_febs; ++i) {
    struct libusb_device_descriptor desc;
    int r = libusb_get_device_descriptor(febs[i], &desc);
    if (r < 0) {
        fprintf(stderr, "!!! Error: failed to get device descriptor.\n");
        libusb_exit(ctx);
        return 1;
    }

    if (id != desc.idProduct)
      continue;

    status = libusb_open(febs[i], &dev_handle);
    if(dev_handle == NULL) {
      fprintf(stderr, "!!! Error: cannot open device.\n");
      return 1;
    } else {
#ifdef UFE_DEBUG
      printf("### Debug: device opened.\n");
#endif

      libusb_free_device_list(febs, 1); //free the lconf_filet, unref the devices in it

      status = (*user_func)(dev_handle);

      libusb_close(dev_handle);

#ifdef UFE_DEBUG
      printf("### Debug: devicec closed.\n");
#endif

      libusb_exit(ctx);
      return status;
    }
  }

  fprintf(stderr, "!!! Error: device 0x%x not found.\n", id); //there was an error
  libusb_exit(ctx);
  return UFE_NOT_FOUND_ERROR;
}


