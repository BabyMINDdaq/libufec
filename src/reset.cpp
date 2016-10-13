#include <iostream>
#include <unistd.h>

#include "libufe.h"
#include "libufedef.h"

using namespace std;

int main (int argc, char **argv) {

  string file_name("../../config/config-bitarray-asic0.txt");
  if (argc == 2) {
    file_name = string(argv[1]);
  }

  libusb_device_handle *dev_handle; //a device handle
  libusb_context *ctx = NULL; //a libusb session
  int status; //for return values

  status = libusb_init(&ctx); //initialize the library for the session we just declared
  if(status < 0) {
    cout << "Init Error " << status << endl; //there was an error
    return 1;
  }

  libusb_set_debug(ctx, 3); //set verbosity level to 3

  libusb_device **febs;
  size_t n_bmfebs = ufe_get_device_list(ctx, &febs);

  cout << "BM FEBs found: " << n_bmfebs << " \n";

  if (n_bmfebs > 0) {
    status = libusb_open(febs[0], &dev_handle);
    if(dev_handle == NULL) {
      cout << "Cannot open device.\n";
      return 1;
    } else
      cout << "Device Opened.\n";

    libusb_free_device_list(febs, 1); //free the lconf_filet, unref the devices in it

    ufe_epxin_reset(dev_handle, 1);
    ufe_epxin_reset(dev_handle, 2);

    libusb_close(dev_handle); //close the device we opened
  }

  libusb_exit(ctx); //needs to be called to end the

  return 0;
}