#include <string.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>

#include "libufe.h"
#include "UFEConfigFrame.h"

#include "GetBackTrace.h"

using namespace std;

int main (int argc, char **argv) {

  SetErrorHdlr();

  string file_name("../../config/config-bitarray-asic0.txt");
  int board_id = 0;
  int device = 0;
  if (argc == 3) {
    stringstream ss;
    ss << string(argv[1]);
    ss >> device;
    file_name = string(argv[2]);
  }

  libusb_device_handle *dev_handle; //a device handle
  ufe_context *ctx = NULL; //a libusb session
  int status; //for return values

  status = ufe_init(&ctx); //initialize the library for the session we just declared
  if(status < 0) {
    cout << "Init Error " << status << endl; //there was an error
    return 1;
  }

  ufe_set_debug(ctx, 3, 3); //set verbosity level to 3

  libusb_device **febs;
  size_t n_bmfebs = ufe_get_device_list(ctx->usb_ctx, &febs);

  cout << "BM FEBs found: " << n_bmfebs << " \n";

  if (n_bmfebs > 0) {
    status = libusb_open(febs[0], &dev_handle);
    if(dev_handle == NULL) {
      cout << "Cannot open device.\n";
      return 1;
    } else
      cout << "Device Opened.\n";

    libusb_free_device_list(febs, 1); //free the lconf_filet, unref the devices in it

    UFEConfigBuffer cb(board_id, device);
    cb.load_config_from_text(file_name);

    uint32_t *data = new uint32_t[36];

    cout << "Configuring ...\n";
    status = ufe_set_config(dev_handle, board_id, device, cb.get_data());
    if (status < 0)
      return 1;

    status = ufe_get_config(dev_handle, board_id, device, data);
    if (status < 0)
      return 1;

    status = memcmp(cb.get_data(), data, cb.get_size());
    if (status != 0)
      return 1;

    status = ufe_apply_config(dev_handle, board_id, device);
    if (status < 0)
      return 1;


    status = ufe_data_readout(dev_handle, board_id, DR_START);
    if (status < 0)
      return 1;

    sleep(2);

    status = ufe_data_readout(dev_handle, board_id, DR_STOP);
    if (status < 0)
      return 1;

    libusb_close(dev_handle); //close the device we opened
    delete[] data;
  }

  ufe_exit(ctx); //needs to be called to end the
  cout << "Done.\n";
  return 0;
}

