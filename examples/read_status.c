#include <stdio.h>

#include "libufe.h"
#include "libufe-tools.h"

uint16_t data_16;
int board_id, usb_id;

int read_status(libusb_device_handle *dev_handle) {

  int status = ufe_read_status(dev_handle, board_id, &data_16);
  printf("0x%x\n", data_16);
  return status;
}

void print_usage(char *argv) {
  fprintf(stderr, "\nUsage: %s [OPTIONS] \n\n", argv);
  fprintf(stderr, "    -i / --product-id   <int dec/hex>   ( USB Product Id )  [ required ]\n");
  fprintf(stderr, "    -b / --board-id     <int dec/hex>   ( Board Id )        [ required ]\n");
}

int main (int argc, char **argv) {

  int usb_id_arg   = get_arg_val('i', "product-id", argc, argv);
  int board_id_arg = get_arg_val('b', "board-id"  , argc, argv);

  if (usb_id_arg == 0 || board_id_arg == 0) {
    print_usage(argv[0]);
    return 1;
  }

  usb_id   = arg_as_int(argv[usb_id_arg]);
  board_id = arg_as_int(argv[board_id_arg]);
  data_16 = 0;

#ifdef UFE_DEBUG
  printf("\nOn device 0x%x  board %i -> Reading status ... \n", usb_id, board_id);
#endif

  int status = on_device_do(usb_id, &read_status);

  return (status!=0)? 1 : 0;
}