#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* malloc, free, rand */
#include <libusb-1.0/libusb.h>

#include "libbmfeb.h"

#include "BMFEBDef.h"

#define BM_VENDOR_ID 0x206b

bool is_bm_feb(libusb_device *dev) {

  struct libusb_device_descriptor desc;
  int status = libusb_get_device_descriptor(dev, &desc);
  if (status < 0) {
    printf("Failed to get device descriptor Error: %i \n", status);
    return false;
  }

  if (desc.idVendor == BM_VENDOR_ID)
    return true;

  return false;
}

size_t get_bmfeb_device_list(libusb_context *ctx, libusb_device ***feb_devs) {
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


int get_version_req(libusb_device_handle *bmfeb, uint8_t *data) {
/** TODO */

  return 0;
}

int get_buff_size_req(libusb_device_handle *bmfeb, uint8_t *data) {
/** TODO */

  return 0;
}

int enable_led_req(libusb_device_handle *bmfeb, bool enable) {

  uint16_t value = (uint16_t)(!enable);
  uint8_t *data = (uint8_t*) malloc(1);
  *data = 7;

  int status = libusb_control_transfer( bmfeb,
                                        CLASS_REQUEST | LIBUSB_ENDPOINT_IN,
                                        BMFEB_LED_OFF_REQ,
                                        value,
                                        0,
                                        data,
                                        1,
                                        1000);
  if(status != 1 && *data == 7)
    printf("Control transfer failed Error: %i, %i\n", status, *data);

  return status;
}

int ep2in_wrappup_req(libusb_device_handle *bmfeb, uint8_t *data) {

  uint16_t value = 2;
  uint8_t *data_ = (uint8_t*) malloc(1);
  *data_ = 7;

  int status = libusb_control_transfer( bmfeb,
                                        CLASS_REQUEST | LIBUSB_ENDPOINT_IN,
                                        BMFEB_EP2IN_WRAPPUP_REQ,
                                        value,
                                        0,
                                        data_,
                                        1,
                                        1000);
  if(status != 1 && *data_ == 7)
    printf("Control transfer failed Error: %i, %i\n", status, *data_);

  return status;
}

int epxin_reset_req(libusb_device_handle *bmfeb, uint8_t *data) {
/** TODO */

  return 0;
}

int send_command_req( libusb_device_handle *bmfeb,
                      int board_id,
                      int command_id,
                      int argc,
                      uint32_t *argv) {

  uint32_t *cmd = (uint32_t*) malloc(4);

  *cmd  = CMD_REQ_HEADER_ID;
  *cmd |= (board_id << BMFEB_BOARD_ID_SHIFT) & BMFEB_BOARD_ID_MASK;
  *cmd |= (command_id << BMFEB_CMD_ID_SHIFT) & BMFEB_CMD_ID_MASK;

  if (argc == 1)
    *cmd |= argv[0] & BMFEB_ARGUMENT_MASK;

  printf("command: %4x \n", *cmd);

  int size = (argc > 1)? argc*4+8 : 4;
  int actual;
  int status = libusb_bulk_transfer( bmfeb,
                                     (0x2 | LIBUSB_ENDPOINT_OUT),
                                     (uint8_t*) cmd,
                                     size,
                                     &actual,
                                     1000);

  if(status == 0 && actual == size) //we wrote the 4 bytes successfully
    printf("Command sent\n");
  else {
    printf("Command not sent Error (%i, %i)\n", status, actual);
    return status;
  }

  return 0;
}

int get_command_answer( libusb_device_handle *bmfeb,
                        int board_id,
                        int command_id,
                        int argc,
                        uint32_t **answer) {

  int size = (argc > 1)? argc*4+8 : 4;
  int actual;
  int status = libusb_bulk_transfer( bmfeb,
                                     (0x82 | LIBUSB_ENDPOINT_IN),
                                     (uint8_t*)*answer,
                                     size,
                                     &actual,
                                     1000);

  if(status == 0 && actual == size) //we wrote the 4 bytes successfully
    printf("Answer resieved: %4x \n", **answer);
  else {
    printf("Answer not resieved Error (%i, %i)\n", status, actual);
    return status;
  }

  if ( (*answer[0] & BMFEB_DW_ID_MASK) >> BMFEB_DW_ID_SHIFT != CMD_HEADER_ID  ||
       (*answer[0] & BMFEB_BOARD_ID_MASK) >> BMFEB_BOARD_ID_SHIFT != board_id ||
       (*answer[0] & BMFEB_CMD_ID_MASK) >> BMFEB_CMD_ID_SHIFT != command_id ) {
    printf("Inconsistent command header\n");
    return BMFEB_ERROR_INVALID_CMD_ANSWER;
  }

  **answer &= BMFEB_ARGUMENT_MASK;

  if (argc > 1) {
    int i;
    for (i=1; i<argc+1; ++i) {
      if ( (*answer[i] & BMFEB_DW_ID_MASK) >> BMFEB_DW_ID_SHIFT != CMD_ARG_ID) {
        printf("Inconsistent command argument\n");
        return BMFEB_ERROR_INVALID_CMD_ANSWER;
      }

      *answer[i] &= (~BMFEB_DW_ID_MASK);
    }

    if ( (*answer[argc+1] & BMFEB_DW_ID_MASK) >> BMFEB_DW_ID_SHIFT != CMD_TRAILER_ID  ||
         (*answer[argc+1] & BMFEB_BOARD_ID_MASK) >> BMFEB_BOARD_ID_SHIFT != board_id ||
         (*answer[argc+1] & BMFEB_CMD_ID_MASK) >> BMFEB_CMD_ID_SHIFT != command_id ) {
      printf("Inconsistent command trailer\n");
      return BMFEB_ERROR_INVALID_CMD_ANSWER;
    }
  }

  return 0;
}

