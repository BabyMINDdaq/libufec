#include <stdio.h>

#include "libufe.h"
#include "libufe-tools.h"

uint16_t data_16;
int usb_id, board_id;

#define NOT_SET 0xFFFF

int set_param(libusb_device_handle *dev_handle) {

  return ufe_set_direct_param(dev_handle, board_id, &data_16);
}

void print_usage(char *argv) {
  fprintf(stderr, "\nUsage: %s [OPTIONS] \n\n", argv);
  fprintf(stderr, "    -i / --product-id   <int dec/hex>   ( USB Product Id )  [ required ]\n");
  fprintf(stderr, "    -b / --board-id     <int dec/hex>   ( Board Id )        [ required ]\n");
  fprintf(stderr, "    -p / --param        <int dec/hex>   ( Param bit array value)      [ optional ]\n");
  fprintf(stderr, "    -s / --stdin                        ( Param bit array from stdin) [ optional ]\n\n");
}

int main (int argc, char **argv) {

  int usb_id_arg   = get_arg_val('i', "product-id", argc, argv);
  int board_id_arg = get_arg_val('b', "board-id"  , argc, argv);
  int param_arg    = get_arg_val('p', "param"     , argc, argv);
  int pipe_arg     = get_arg('s', "stdin"     , argc, argv);

  if (usb_id_arg == 0 || board_id_arg == 0) {
    print_usage(argv[0]);
    return 1;
  }

  if (!( (param_arg == 0) ^ (pipe_arg == 0) )) {
    print_usage(argv[0]);
    return 1;
  }

  usb_id   = arg_as_int(argv[usb_id_arg]);
  board_id = arg_as_int(argv[board_id_arg]);
  data_16 = NOT_SET;

  if ( param_arg != 0 ) {
    data_16 = arg_as_int(argv[param_arg]);
  }

  if (pipe_arg != 0) {
    char line[11];
    if (fgets(line, 11, stdin) != NULL)
      data_16 = arg_as_int(line);

  }

  if (data_16 == NOT_SET ) {
    print_usage(argv[0]);
    return 1;
  }

#ifdef UFE_DEBUG
  printf("\nOn device 0x%x  board %i -> Setting params: 0x%x \n", usb_id, board_id, data_16);
#endif

  int status = on_device_do(usb_id, &set_param);

  return (status!=0)? 1 : 0;
}