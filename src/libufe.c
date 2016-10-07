#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* malloc, free, rand */
#include <libusb-1.0/libusb.h>

#include "libufe.h"

#include "UFEDef.h"

bool is_bm_feb(libusb_device *dev) {

  struct libusb_device_descriptor desc;
  int status = libusb_get_device_descriptor(dev, &desc);
  if (status < 0) {
    printf("Failed to get device descriptor Error: %i \n", status);
    return false;
  }

  if (desc.idVendor == UFE_VENDOR_ID)
    return true;

  return false;
}

size_t get_ufe_device_list(libusb_context *ctx, libusb_device ***feb_devs) {
  libusb_device **devs;
  size_t n_devs = libusb_get_device_list(ctx, &devs); //get the list of devices
  size_t n_febs = 0;

  if(n_devs < 0) {
    printf("Device Error \n"); //there was an error
  }

  int i_dev;
  for(i_dev = 0; i_dev < n_devs; i_dev++) {
    if ( is_bm_feb(devs[i_dev]) ) {
      ++n_febs;
    }
  }

  *feb_devs = (libusb_device **) calloc(n_febs, sizeof(libusb_device*));

  int i_feb = 0;
  i_dev = 0;
  while (i_feb<n_febs) {
    if ( is_bm_feb(devs[i_dev]) ) {
      *feb_devs[i_feb] = devs[i_dev];
      libusb_ref_device(devs[i_dev]);
      ++i_feb;
    }
    ++i_dev;
  }

  libusb_free_device_list(devs, 1); //free the list, unref the devices in it
  return n_febs;
}


int get_version_req(libusb_device_handle *ufe, uint8_t *data) {
/** TODO */

  return 0;
}

int get_buff_size_req(libusb_device_handle *ufe, uint8_t *data) {
/** TODO */

  return 0;
}

int enable_led_req(libusb_device_handle *ufe, bool enable) {

  uint16_t value = (uint16_t)(!enable);
  uint8_t *data = (uint8_t*) malloc(1);
  *data = 7;

  int status = libusb_control_transfer( ufe,
                                        CLASS_REQUEST | LIBUSB_ENDPOINT_IN,
                                        UFE_LED_OFF_REQ,
                                        value,
                                        0,
                                        data,
                                        1,
                                        1000);
  if(status != 1 && *data == 7)
    printf("Control transfer failed Error: %i, %i\n", status, *data);

  return status;
}

int ep2in_wrappup_req(libusb_device_handle *ufe, uint8_t *data) {

  uint16_t value = 2;
  uint8_t *data_ = (uint8_t*) malloc(1);
  *data_ = 7;

  int status = libusb_control_transfer( ufe,
                                        CLASS_REQUEST | LIBUSB_ENDPOINT_IN,
                                        UFE_EP2IN_WRAPPUP_REQ,
                                        value,
                                        0,
                                        data_,
                                        1,
                                        1000);
  if(status != 1 && *data_ == 7)
    printf("Control transfer failed Error: %i, %i\n", status, *data_);

  return status;
}

int epxin_reset_req(libusb_device_handle *ufe, uint8_t *data) {
/** TODO */

  return 0;
}

int send_command_req( libusb_device_handle *ufe,
                      int board_id,
                      int command_id,
                      int sub_cmd_id,
                      int argc,
                      uint16_t *argv) {

  // Allocate memory for the command according to the number of arguments.
  int size = (argc > 1)? (argc+2)*4 : 4;
  uint32_t *cmd = malloc(size);

  // Set the Header of the command.
  *cmd  = (CMD_HEADER_ID << UFE_DW_ID_SHIFT);;
  *cmd |= (board_id << UFE_BOARD_ID_SHIFT) & UFE_BOARD_ID_MASK;
  *cmd |= (command_id << UFE_CMD_ID_SHIFT) & UFE_CMD_ID_MASK;

  // If the command has only one argument add it to the header of the command.
  if (argc == 1) {
    *cmd |= argv[0] & UFE_ARGUMENT_MASK;
  }

  // If the command has moltiple arguments add the number of arguments to the header of the command.
  if (argc > 1) {
    *cmd |= argc & UFE_ARG_FR_NUM_MASK;
  }

  // Add the Sub Command Id if any.
  if (sub_cmd_id > 0) {
    *cmd |= (sub_cmd_id << UFE_SUBCMD_ID_SHIFT) & UFE_SUBCMD_ID_MASK;
  }

  printf("command: 0x%4x \n", *cmd);

  if (argc > 1) {
    int xArg = 0;
    // Set the data words of the arguments.
    while ( xArg < argc) {
      cmd[xArg+1] = argv[xArg];
      cmd[xArg+1] |= (CMD_ARG_ID << UFE_DW_ID_SHIFT);
      cmd[xArg+1] |= (xArg << UEF_FRAME_INDEX_SHIFT) & UEF_FRAME_INDEX_MASK;
      printf("arg: 0x%4x \n", cmd[xArg+1]);
      ++xArg;
    }

    // Set the trailer.
    cmd[argc+1] = (CMD_TRAILER_ID << UFE_DW_ID_SHIFT);
    cmd[argc+1] |= (board_id << UFE_BOARD_ID_SHIFT) & UFE_BOARD_ID_MASK;
    cmd[argc+1] |= (command_id << UFE_CMD_ID_SHIFT) & UFE_CMD_ID_MASK;
    printf("trailer: 0x%4x \n", cmd[xArg+1]);
  }

  int actual;
  int status = libusb_bulk_transfer( ufe,
                                     (0x2 | LIBUSB_ENDPOINT_OUT),
                                     (uint8_t*) cmd,
                                     size,
                                     &actual,
                                     1000);

  if(status == 0 && actual == size) //we wrote the 4 bytes successfully
    printf("Command sent (%i, %i)\n", status, actual);
  else {
    printf("Command not sent Error (%i, %i)\n", status, actual);
    return status;
  }

  free(cmd);
  return 0;
}

int get_command_answer( libusb_device_handle *ufe,
                        int board_id,
                        int command_id,
                        int argc,
                        uint16_t **argv) {

  // Allocate memory for the answer according to the number of arguments.
  int size = (argc > 1)? (argc+2*4) : 4;
  uint32_t *answer = malloc(size);

  int actual;
  int status = libusb_bulk_transfer( ufe,
                                     (0x82 | LIBUSB_ENDPOINT_IN),
                                     (uint8_t*) answer,
                                     size,
                                     &actual,
                                     1000);

  if(status == 0 && actual == size) //we wrote the 4 bytes successfully
    printf("Answer resieved (%i, %i): %4x\n", status, actual, *answer);
  else {
    printf("Answer not resieved (%i, %i): Error\n", status, actual);
    return status;
  }

  if ( (answer[0] & UFE_DW_ID_MASK) >> UFE_DW_ID_SHIFT != CMD_HEADER_ID  ||
       (answer[0] & UFE_BOARD_ID_MASK) >> UFE_BOARD_ID_SHIFT != board_id ||
       (answer[0] & UFE_CMD_ID_MASK) >> UFE_CMD_ID_SHIFT != command_id ) {
    printf("Inconsistent command header\n");
    return UFE_ERROR_INVALID_CMD_ANSWER;
  }

  **argv = *answer & UFE_ARGUMENT_MASK;

  if (argc > 1) {
    int i;
    for (i=1; i<argc+1; ++i) {
      if ( (answer[i] & UFE_DW_ID_MASK) >> UFE_DW_ID_SHIFT != CMD_ARG_ID) {
        printf("Inconsistent command argument\n");
        return UFE_ERROR_INVALID_CMD_ANSWER;
      }

      answer[i] &= (~UFE_DW_ID_MASK);
    }

    if ( (answer[argc+1] & UFE_DW_ID_MASK) >> UFE_DW_ID_SHIFT != CMD_TRAILER_ID  ||
         (answer[argc+1] & UFE_BOARD_ID_MASK) >> UFE_BOARD_ID_SHIFT != board_id ||
         (answer[argc+1] & UFE_CMD_ID_MASK) >> UFE_CMD_ID_SHIFT != command_id ) {
      printf("Inconsistent command trailer\n");
      return UFE_ERROR_INVALID_CMD_ANSWER;
    }
  }

  free(answer);
  return 0;
}

