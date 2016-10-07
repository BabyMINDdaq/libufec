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
  size_t n_bmfebs = get_ufe_device_list(ctx, &febs);

  cout << "BM FEBs found: " << n_bmfebs << " \n";

  if (n_bmfebs > 0) {
    status = libusb_open(febs[0], &dev_handle);
    if(dev_handle == NULL) {
      cout << "Cannot open device.\n";
      return 1;
    } else
      cout << "Device Opened.\n";

    libusb_free_device_list(febs, 1); //free the list, unref the devices in it

    enable_led_req(dev_handle, led);
    usleep(1);

    uint16_t *data = new uint16_t;
    int board_id = 0, command_id;

    command_id = FIRMWARE_VERSION_CMD_ID;
    *data = 0;
    send_command_req( dev_handle,
                      board_id,
                      command_id,
                      NO_SUB_CMD_ID,
                      1,
                      data);

    usleep(1);
    ep2in_wrappup_req(dev_handle, NULL);
    usleep(1);

    get_command_answer( dev_handle,
                        board_id,
                        command_id,
                        1,
                        &data);

    cout << "FV: " << hex << *data << dec << endl;
    usleep(1);

    // Turn HV On
    command_id = SET_DIRECT_PARAM_CMD_ID;
    *data = SDP_HVON;
    send_command_req( dev_handle,
                      board_id,
                      command_id,
                      NO_SUB_CMD_ID,
                      1,
                      data);

    usleep(1);
    ep2in_wrappup_req(dev_handle, NULL);
    usleep(1);

    get_command_answer( dev_handle,
                        board_id,
                        command_id,
                        1,
                        &data);

    cout << "STATUS: " << hex << *data << dec << endl;
    usleep(1);

    command_id = READ_STATUS_CMD_ID;
    send_command_req( dev_handle,
                      board_id,
                      command_id,
                      NO_SUB_CMD_ID,
                      0,
                      NULL);

    usleep(1);
    ep2in_wrappup_req(dev_handle, NULL);
    usleep(1);

    get_command_answer( dev_handle,
                        board_id,
                        command_id,
                        1,
                        &data);

    int hv = (*data & RS_HVON)? 1:0;
    cout << "HV On: " << hv << endl;

    libusb_close(dev_handle); //close the device we opened

  }

  libusb_exit(ctx); //needs to be called to end the
  return 0;
}

