#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "libufe.h"
#include "libufe-tools.h"

#define SIZE  20

// uint32_t *conf_data;
int device_id, board_id, usb_id;
uint32_t conf_data[36], conf_data_back[36];


void print_usage(char *argv) {
  fprintf(stderr, "\nUsage: %s [OPTIONS] \n\n", argv);
  fprintf(stderr, "    -i / --product-id   <int dec/hex>   ( USB Product Id )  [ required ]\n");
  fprintf(stderr, "    -b / --board-id     <int dec/hex>   ( Board Id )        [ required ]\n");
  fprintf(stderr, "    -d / --device-id     <int dec/hex>   ( Board Id )       [ required ]\n");
  fprintf(stderr, "    -f / --config-file  <string>        ( Text file containing the config bit array values ) [ optional ]\n");
  fprintf(stderr, "    -s / --stdin                        ( Config bit array from stdin ) [ optional ]\n\n");
}

int config(libusb_device_handle *dev_handle) {
  int status = ufe_set_config(dev_handle, board_id, device_id, conf_data);
  if (status < 0)
    return 1;

  status = ufe_get_config(dev_handle, board_id, device_id, conf_data_back);
  if (status < 0)
    return 1;

  status = memcmp(conf_data, conf_data_back, sizeof(conf_data));
  if (status != 0) {
    fprintf(stderr, "!!! Error: On board %i, device %i - configuration mismatch.\n", board_id, device_id);
    return 1;
  }

  status = ufe_apply_config(dev_handle, board_id, device_id);
  return status;
}

int main (int argc, char **argv) {

  int usb_id_arg    = get_arg_val('i', "product-id"  , argc, argv);
  int board_id_arg  = get_arg_val('b', "board-id"    , argc, argv);
  int device_id_arg = get_arg_val('d', "device-id"   , argc, argv);
  int file_arg      = get_arg_val('f', "config-file" , argc, argv);
  int pipe_arg      = get_arg('s', "stdin"           , argc, argv);
  int status = 0;

  if (usb_id_arg == 0 || board_id_arg == 0 || device_id_arg == 0) {
    print_usage(argv[0]);
    return 1;
  }

  if (!( (file_arg == 0) ^ (pipe_arg == 0) )) {
    print_usage(argv[0]);
    return 1;
  }

  usb_id    = arg_as_int(argv[usb_id_arg]);
  board_id  = arg_as_int(argv[board_id_arg]);
  device_id = arg_as_int(argv[device_id_arg]);

  char stdin_buff[SIZE];

  FILE *conf_file;
  if (pipe_arg != 0)
    conf_file = stdin;
  else
    conf_file = fopen(argv[file_arg], "r");

  int i=0;
  while ( fgets(stdin_buff, SIZE, conf_file) != NULL ) {
      conf_data[i] = arg_as_int( stdin_buff );
//       printf("%i s: %s", i, stdin_buff);
//       printf("%i b: 0x%x\n", i, conf_data[i]);
      ++i;
  }

#ifdef UFE_DEBUG
    printf("Configuring ...\n");
#endif

  status = on_device_do(usb_id, &config);

  return (status!=0)? 1 : 0;
}


