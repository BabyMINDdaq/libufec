#include <stdio.h>

#include "libufe.h"
#include "libufe-tools.h"

int led_on(libusb_device_handle *dev_handle) {

  return ufe_enable_led(dev_handle, 1);
}

int led_off(libusb_device_handle *dev_handle) {

  return ufe_enable_led(dev_handle, 0);
}

void print_usage(char *argv) {
  fprintf(stderr, "\nUsage: %s [OPTION] ARG \n\n", argv);
  fprintf(stderr, "    -i / --product-id   <int dec/hex>   ( USB Product Id )  [ required ]\n");
  fprintf(stderr, "    ARG                 < 0 / 1 >       ( Turn On / Off )   [ required ]\n");
}

int main (int argc, char **argv) {

  int turn_on = 1, status = 0;

  int x_arg = get_arg_val('i', "product-id", argc, argv);
  if (x_arg == 0 || argc != 4) {
    print_usage(argv[0]);
    return 1;
  }

  int usb_product_id = arg_as_int(argv[x_arg]);

  if (argc == 2) {
    if (strcmp(argv[3], "0") == 0)
      turn_on = 0;
  }

  if (turn_on)
    status = on_device_do(usb_product_id, &led_on);
  else
    status = on_device_do(usb_product_id, &led_off);

  return (status!=0)? 1 : 0;
}