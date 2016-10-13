/** This is an example application written in C
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "libufe.h"


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
    printf("LibUSB Init Error: %i \n", status); //there was an error
    return 1;
  }

  libusb_set_debug(ctx, 3); //set verbosity level to 3

  libusb_device **febs;
  size_t n_bmfebs = ufe_get_device_list(ctx, &febs);

  printf("BM FEBs found: %zu \n", n_bmfebs);

  if (n_bmfebs > 0) {
    status = libusb_open(febs[0], &dev_handle);
    if(dev_handle == NULL) {
      printf("Cannot open device\n");
      return 1;
    } else
      printf("Device Opened");

    libusb_free_device_list(febs, 1); //free the list, unref the devices in it

    enable_led_req(dev_handle, led);
    usleep(1);

    uint16_t *data = (uint16_t*) malloc(2);
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
    ep2in_wrappup_req(dev_handle);
    usleep(1);

    get_command_answer( dev_handle,
                        board_id,
                        command_id,
                        NO_SUB_CMD_ID,
                        1,
                        &data);

    printf("FV:  %4x \n", *data);
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
    ep2in_wrappup_req(dev_handle);
    usleep(1);

    get_command_answer( dev_handle,
                        board_id,
                        command_id,
                        NO_SUB_CMD_ID,
                        1,
                        &data);

    printf("Status:  %4x \n", *data);
    usleep(1);

    command_id = READ_STATUS_CMD_ID;
    send_command_req( dev_handle,
                      board_id,
                      command_id,
                      NO_SUB_CMD_ID,
                      0,
                      NULL);

    usleep(1);
    ep2in_wrappup_req(dev_handle);
    usleep(1);

    get_command_answer( dev_handle,
                        board_id,
                        command_id,
                        NO_SUB_CMD_ID,
                        1,
                        &data);

    int hv = (*data & RS_HVON)? 1:0;
    printf("HV On: %i \n", hv);

    libusb_close(dev_handle); //close the device we opened

  }

  libusb_exit(ctx); //needs to be called to end the
  return 0;
}

