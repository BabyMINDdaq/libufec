/** This is an example application written in C++
 */

#include <iostream>
#include <unistd.h>

#include "libufe.h"

using namespace std;

int main(int argc, char** argv) {

  bool led = false;
  if (argc == 2) {
    led = true;
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

    libusb_free_device_list(febs, 1); //free the list, unref the devices in it

    status = ufe_enable_led(dev_handle, led);
    if (status < 0)
      return 1;

    int data;
//     uint32_t data_32 = 0;
    uint16_t data_16 = 0;
    int board_id = 0;
    uint64_t data_l = 0;
    status = ufe_get_version(dev_handle, &data);
    if (status < 0)
      return 1;

    status = ufe_get_buff_size(dev_handle, &data_l);
    if (status < 0)
      return 1;

    data = 0;
    status = ufe_firmware_version(dev_handle, board_id, &data);
    if (status < 0)
      return 1;

    cout << "FV: " << hex << data << dec << endl;

    // Turn HV On
    data = SDP_HVON;
    status = ufe_set_direct_param(dev_handle, board_id, &data_16);
    if (status < 0)
      return 1;

    status = ufe_read_status(dev_handle, board_id, &data_16);
     if (status < 0)
      return 1;

    int hv = (data & RS_HVON)? 1:0;
    cout << "HV On: " << hv << endl;

    libusb_close(dev_handle); //close the device we opened
  }

  libusb_exit(ctx); //needs to be called to end the
  return 0;
}

