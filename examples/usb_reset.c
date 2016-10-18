#include <stdio.h>

#include "libufe.h"
#include "libufe-tools.h"

int usb_reset(libusb_device_handle *dev_handle) {

  int status = ufe_epxin_reset(dev_handle, 1);
  if (status != 0)
    return status;

  status = ufe_epxin_reset(dev_handle, 2);
  return status;
}

int main (int argc, char **argv) {

  int x_arg = get_arg_val('i', "product-id", argc, argv);
  if (x_arg == 0) {
    fprintf(stderr, "\nUsage: %s [OPTIONS] \n\n", argv[0]);
    fprintf(stderr, "    -i / --product-id   <int dec/hex> :  USB Product Id [ required ]\n\n");
    return 1;
  }

  int usb_product_id = arg_as_int(argv[x_arg]);

  int status = on_device_do(usb_product_id, &usb_reset);
  if (status != 1)
    return 1;

  return 0;
}


