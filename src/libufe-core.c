/** This file is part of BabyMINDdaq software package. This software
 * package is designed for internal use for the Baby MIND detector
 * collaboration and is tailored for this use primarily.
 *
 * BabyMINDdaq is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BabyMINDdaq is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BabyMINDdaq.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  \author   Yordan Karadzhov <Yordan.Karadzhov \at cern.ch>
 *            University of Geneva
 *
 *  \created  Nov 2016
 */


#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>

#include "libufe.h"
#include "libufe-core.h"

bool is_ufe(libusb_device *dev, int dummy_arg) {
  struct libusb_device_descriptor desc;
  int status = libusb_get_device_descriptor(dev, &desc);
  if (status < 0) {
    ufe_error_print("failed to get device descriptor; status: %i", status);
    return false;
  }

  if (desc.idVendor == UFE_VENDOR_ID)
    return true;

  return false;
}

bool is_bm_feb(libusb_device *dev, int dummy_arg) {
  struct libusb_device_descriptor desc;
  int status = libusb_get_device_descriptor(dev, &desc);
  if (status < 0) {
    ufe_error_print("failed to get device descriptor; status: %i", status);
    return false;
  }

  if (desc.idVendor == UFE_VENDOR_ID && desc.idProduct == BMFEB_PRODUCT_ID)
    return true;

  return false;
}

bool is_bm_feb_with_id(libusb_device *dev, int board_id) {
  struct libusb_device_descriptor desc;
  libusb_device_handle *dev_handle;
  int status = libusb_get_device_descriptor(dev, &desc);
  if (status < 0) {
    ufe_error_print("failed to get device descriptor; status: %i", status);
    return false;
  }

  if ( desc.idVendor  != UFE_VENDOR_ID ||
       desc.idProduct != BMFEB_PRODUCT_ID ) {
    return false;
  }

  status = libusb_open(dev, &dev_handle);
  if(dev_handle == NULL) {
    ufe_error_print("cannot open device.");
    return false;
  }

  if (ufe_ping(dev_handle, board_id)) {
    libusb_close(dev_handle);
    return true;
  }

  libusb_close(dev_handle);
  return false;
}

size_t ufe_get_custom_device_list(libusb_context *ctx, ufe_cond_func cond, libusb_device ***feb_devs, int arg) {
  libusb_device **devs;
  ssize_t n_devs = libusb_get_device_list(ctx, &devs); //get the list of devices
  size_t n_febs = 0;

  if(n_devs < 0) {
    ufe_error_print("Device Error."); //there was an error
    return 0;
  }

  uint8_t* dev_id = (uint8_t*) calloc(n_devs, sizeof(uint8_t));
  int i_dev, i_feb = 0;
  for(i_dev = 0; i_dev < n_devs; i_dev++) {
    if ( (*cond)(devs[i_dev], arg) ) {
      dev_id[n_febs++] = i_dev;
    }
  }

  libusb_device **febs_found = (libusb_device**) calloc(n_febs, sizeof(struct libusb_device*));

  for(i_dev = 0; i_dev < n_febs; i_dev++) {
    int x_dev = dev_id[i_dev];
    febs_found[i_feb++] = libusb_ref_device(devs[x_dev]);
  }

  free(dev_id);

  *feb_devs = febs_found;
  libusb_free_device_list(devs, 1); //free the list, unref the devices in it
  return n_febs;
}

crc_context crc16_context_handler;
crc_context crc21_context_handler;
extern ufe_context *ufe_context_handler;

int ufe_get_verbose() {
  if (!ufe_context_handler)
    return 3;

  return ufe_context_handler->verbose_;
}

int ufe_send_command_req( libusb_device_handle *ufe,
                      int board_id,
                      int command_id,
                      int sub_cmd_id,
                      int argc,
                      uint16_t *argv) {

  // Allocate memory for the command according to the number of arguments.
  int size = (argc > 1)? (argc+2)*4 : 4;
  uint32_t *cmd = (uint32_t*) malloc(size);

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

//   printf("command: 0x%4x \n", *cmd);

  if (argc > 1) {
    int xArg = 0;
    // Set the data words of the arguments.
    while ( xArg < argc) {
      cmd[xArg+1] = argv[xArg];
      cmd[xArg+1] |= (CMD_ARG_ID << UFE_DW_ID_SHIFT);
      cmd[xArg+1] |= (xArg << UEF_FRAME_INDEX_SHIFT) & UEF_FRAME_INDEX_MASK;
//       printf("arg: 0x%4x \n", cmd[xArg+1]);
      ++xArg;
    }

    // Set the trailer.
    cmd[argc+1] = (CMD_TRAILER_ID << UFE_DW_ID_SHIFT);
    cmd[argc+1] |= (board_id << UFE_BOARD_ID_SHIFT) & UFE_BOARD_ID_MASK;
    cmd[argc+1] |= (command_id << UFE_CMD_ID_SHIFT) & UFE_CMD_ID_MASK;

    // Set CRC16
    uint16_t cmd_crc = crc(&crc16_context_handler, (uint8_t*) argv, argc*2);
//     printf("command crc: 0x%x\n", cmd_crc);
    cmd[argc+1] |= cmd_crc;
//     printf("trailer: 0x%4x \n", cmd[xArg+1]);
  }

  // Send the command.
  int status = ufe_user_set_sync(ufe, 2, size, (uint8_t*) cmd);
  if (status < 0) {
    const char* cmd_name = get_command_name(command_id);
    ufe_error_print("error during command %s ( board %i )", cmd_name, board_id);
    return status;
    }

  free(cmd);
  return 0;
}

int ufe_get_command_answer( libusb_device_handle *ufe,
                        int board_id,
                        int command_id,
                        int sub_cmd_id,
                        int argc,
                        uint16_t **argv) {

  // Allocate memory for the answer according to the number of arguments.
  int size = (argc > 1)? ((argc+2)*4) : 4;
  uint32_t *answer = (uint32_t*) malloc(size);

  // Get the command answer.
  int status = ufe_user_get_sync(ufe, 2, size, (uint8_t*) answer);
  if (status < 0) {
    const char* cmd_name = get_command_name(command_id);
    ufe_error_print("error during command %s ( board %i )", cmd_name, board_id);
    free(answer);
    return status;
  }

  // Check for firmware errors.
  if ( (*answer & UFE_CMD_ID_MASK) >>  UFE_CMD_ID_SHIFT == ERROR_CMD_ID ) {
    const char* cmd_name = get_command_name(command_id);
    ufe_error_print("Firmware Error ( 0x%4x ) after command %s.", *answer, cmd_name);
    **argv = *answer & UFE_ARGUMENT_MASK;
    free(answer);
    return UFE_INTERNAL_ERROR;
  }

  // Verify the consistency of the answer.
  // Check the header.
  if ( (*answer & UFE_DW_ID_MASK)    >>  UFE_DW_ID_SHIFT   != CMD_HEADER_ID   ||
       (*answer & UFE_BOARD_ID_MASK) >>  UFE_BOARD_ID_SHIFT != board_id ||
       (*answer & UFE_CMD_ID_MASK)   >>  UFE_CMD_ID_SHIFT   != command_id ) {
    const char* cmd_name = get_command_name(command_id);
    ufe_error_print("inconsistent answer header ( 0x%4x ) after command %s.",
             *answer, cmd_name);
    free(answer);
    return UFE_INVALID_CMD_ANSWER_ERROR;
  }

  // If the argument is only one, retrieve the value.
  if  (argc == 1) {
    **argv = *answer & UFE_ARGUMENT_MASK;
  }

  // Check the header subcommand Id.
  if (sub_cmd_id >= 0) {
    if ( (*answer & UFE_SUBCMD_ID_MASK) >> UFE_SUBCMD_ID_SHIFT != sub_cmd_id ) {
      const char* cmd_name = get_command_name(command_id);
      ufe_error_print("inconsistent answer header ( 0x%4x ) after command %s.",
               *answer, cmd_name);
      free(answer);
      return UFE_INVALID_CMD_ANSWER_ERROR;
    }
  }

  // Check the number of arguments.
  if (argc > 1) {
    if ( (*answer & UFE_ARG_FR_NUM_MASK) != argc ) {
      const char* cmd_name = get_command_name(command_id);
      ufe_error_print("inconsistent answer header ( 0x%4x ) after command %s.",
               *answer, cmd_name);
      free(answer);
      return UFE_INVALID_CMD_ANSWER_ERROR;
    }

    // Loop over the arguments.
    int i;
    for (i=0; i<argc; ++i) {
      // Check the consistency of the argument.
      if ( (answer[i+1] & UFE_DW_ID_MASK)       >> UFE_DW_ID_SHIFT       != CMD_ARG_ID ||
           (answer[i+1] & UEF_FRAME_INDEX_MASK) >> UEF_FRAME_INDEX_SHIFT != i ) {
        const char* cmd_name = get_command_name(command_id);
        ufe_error_print("inconsistent answer argument %i ( 0x%4x ) after command %s.",
                 i, answer[i+1], cmd_name);
        free(answer);
        return UFE_INVALID_CMD_ANSWER_ERROR;
      }

      // Retrieve the value.
      (*argv)[i] = (~UFE_DW_ID_MASK) & answer[i+1];
    }

    // Check the trailer
    if ( (answer[argc+1] & UFE_DW_ID_MASK)    >>  UFE_DW_ID_SHIFT    != CMD_TRAILER_ID  ||
         (answer[argc+1] & UFE_BOARD_ID_MASK) >>  UFE_BOARD_ID_SHIFT != board_id ||
         (answer[argc+1] & UFE_CMD_ID_MASK)   >>  UFE_CMD_ID_SHIFT   != command_id ) {
      const char* cmd_name = get_command_name(command_id);
      ufe_error_print("inconsistent answer trailer ( 0x%4x ) after command %s.",
               answer[argc+1], cmd_name);
      free(answer);
      return UFE_INVALID_CMD_ANSWER_ERROR;
    }

    // Check the CRC16
    uint16_t answer_crc = crc(&crc16_context_handler, (uint8_t*) *argv, argc*2);
//     printf("answer crc: 0x%x \n", answer_crc);
    if (answer_crc != (answer[argc+1] & crc16_context_handler.mask_)) {
      const char* cmd_name = get_command_name(command_id);
      ufe_error_print("CRC16 mismatch after command %s.", cmd_name);
      free(answer);
      return UFE_INVALID_CMD_ANSWER_ERROR;
    }
  }

  free(answer);
  return 0;
}

int ufe_user_set_sync( libusb_device_handle *ufe, int ep, int size, uint8_t *data) {

  // Prepare the End Point identifier.
  uint8_t ep_id;
  if (ep == 1) 
    ep_id = UFE_USB_EP1_OUT | LIBUSB_ENDPOINT_OUT;
  else if (ep == 2)
    ep_id = UFE_USB_EP2_OUT | LIBUSB_ENDPOINT_OUT;
  else {
    ufe_error_print("invalid End point id %i", ep);
    return UFE_INVALID_ARG_ERROR;
  }

  // Make bulk transfer.
  int actual_tot=0;
  uint8_t *data_tmp = data;
  while (actual_tot < size) {
    // Max size, allowed for a single transfer is 256.
    // If the command buffer is bigger, make multiple transfers.
    int tr_size = ( (size-actual_tot) < 256 )? size-actual_tot : 256;
    int actual;
    int status = libusb_bulk_transfer( ufe,
                                     ep_id,
                                     data_tmp,
                                     tr_size,
                                     &actual,
                                     UFE_CMD_TIMEOUT);

    if(status != 0 || actual != tr_size) {
      ufe_error_print("data not sent to EP %i (0x%x)  status: %i, size: %i", ep, ep_id, status, actual);

      if (status < 0)
        return status;

      return UFE_IO_ERROR;
    } else {
      //we wrote successfully

#ifdef UFE_DEBUG

    // Print in a proper format. Only in debugging mode.
      switch (size) {
        case 1:
          ufe_debug_print("data sent to EP %i ( %i, %i ): 0x%x", ep, status, actual, *((uint8_t *) data));
          break;
        case 2:
          ufe_debug_print("data sent to EP %i ( %i, %i ): 0x%x", ep, status, actual, *((uint16_t *) data));
          break;
        default:
          ufe_debug_print("data sent to EP %i ( %i, %i ): 0x%x", ep, status, actual, *((uint32_t *) data));
          break;
      }

#endif

      data_tmp += tr_size;
      actual_tot += tr_size;
      usleep(10);
    }
  }

  return 0;
}

int ufe_user_get_sync( libusb_device_handle *ufe, int ep, int size, uint8_t *data) {

  // Prepare the End Point identifier.
  uint8_t ep_id;
  if (ep == 1) 
    ep_id = UFE_USB_EP1_IN | LIBUSB_ENDPOINT_IN;
  else if (ep == 2)
    ep_id = UFE_USB_EP2_IN | LIBUSB_ENDPOINT_IN;
  else {
    ufe_error_print("invalid End point id %i", ep);
    return UFE_INVALID_ARG_ERROR;
  }

  // Make bulk transfer.
  int actual;
  int status = libusb_bulk_transfer( ufe,
                                     ep_id,
                                     data,
                                     size,
                                     &actual,
                                     UFE_CMD_TIMEOUT);

  if(status == 0 && actual == size) {
    //we read successfully

#ifdef UFE_DEBUG

    // Print in a proper format. Only in debugging mode.
    switch (size) {
      case 1:
        ufe_debug_print("data resieved from EP %i ( %i, %i ): 0x%x", ep, status, actual, *((uint8_t *) data));
        break;
      case 2:
        ufe_debug_print("data resieved from EP %i ( %i, %i ): 0x%x", ep, status, actual, *((uint16_t *) data));
        break;
      default:
        ufe_debug_print("data resieved from EP %i ( %i, %i ): 0x%x", ep, status, actual, *((uint32_t *) data));
        break;
    }

#endif

    return 0;
  }

  ufe_error_print("data not resieved from EP %i (0x%x)  status: %i, size: %i", ep, ep_id, status, actual);

  if (status < 0)
    return status;

  return UFE_IO_ERROR;
}


void ufe_build_crc_table(crc_context *this_crc) {

  uint32_t l_remainder;
  uint32_t l_topBit = (uint32_t)(1<<(this_crc->size_ - 1));

  // Compute the remainder of each possible dividend.
  int l_dividend;
  for (l_dividend = 0; l_dividend < 256; ++l_dividend) {
    // Start with the dividend followed by zeros.
    l_remainder = (uint32_t)(l_dividend << (this_crc->size_ - 8));

    // Perform modulo-2 division, a bit at a time.
    uint8_t l_bit;
    for (l_bit = 8; l_bit > 0; --l_bit) {
      //Try to divide the current data bit.
      if ((l_remainder & l_topBit) > 0)
        l_remainder = (l_remainder << 1) ^ this_crc->polynomial_;
      else
        l_remainder = (l_remainder << 1);
    }

    // Store the result into the table.
    this_crc->table_[l_dividend] = l_remainder & this_crc->mask_;
  }
}


void ufe_crc_init( crc_context *this_crc,
                   uint32_t x_polynomial,
                   uint8_t  x_size,
                   uint32_t x_initRemainder,
                   uint32_t x_finalXor,
                   bool x_reflectDin,
                   bool x_reflectCRC) {

  this_crc->polynomial_    = x_polynomial;
  this_crc->size_          = x_size;
  this_crc->initRemainder_ = x_initRemainder;
  this_crc->finalXor_      = x_finalXor;
  this_crc->reflectDin_    = x_reflectDin;
  this_crc->reflectCRC_    = x_reflectCRC;

  this_crc->mask_ = (this_crc->size_ == 32) ? 0xFFFFFFFF : (uint32_t)((1 << this_crc->size_) - 1);
  this_crc->table_ = (uint32_t*) calloc(256, sizeof(uint32_t));
  ufe_build_crc_table(this_crc);
}


uint32_t reflect(uint32_t x_val, uint8_t x_nbBits) {
  uint32_t l_reflection = 0;

  // Reflect the data about the center bit.
  uint8_t  l_bit;
  for (l_bit = 0; l_bit < x_nbBits; ++l_bit) {
    // If the LSB bit is set, set the reflection of it.
    if ((x_val & 0x01)==0x01)
      l_reflection |= (uint32_t)(1 << ((x_nbBits - 1) - l_bit));

    x_val = (x_val >> 1);
  }

  return (l_reflection);
}

uint32_t crc( crc_context *this_crc,
              uint8_t *message,
              ssize_t length) {
  uint8_t l_data;
  uint32_t l_remainder = this_crc->initRemainder_;

  // Divide the message by the polynomial, a uint8_t at a time.
  int l_i;
  for (l_i = 0; l_i < length; ++l_i) {
    // reflect DIN ?
    uint8_t l_din = (this_crc->reflectDin_) ? (uint8_t)(reflect(message[l_i], 8)): message[l_i];

    l_data = (uint8_t)(l_din ^ (l_remainder >> (this_crc->size_ - 8)));

    l_remainder = this_crc->table_[l_data] ^ (l_remainder << 8);
  }

  uint32_t l_crc = (l_remainder & this_crc->mask_) ^ this_crc->finalXor_;

  if (this_crc->reflectCRC_)
    l_crc = reflect(l_crc, this_crc->size_);

  return l_crc;
}



