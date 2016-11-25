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
 *  \created  Oct 2016
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>

#include <libusb-1.0/libusb.h>

#include "libufe.h"
#include "libufe-core.h"
#include "libufe-tools.h"


ufe_context *ufe_context_handler = NULL;

extern crc_context crc16_context_handler;
extern crc_context crc21_context_handler;

int ufe_default_context(ufe_context **context) {
  ufe_context *ctx;
  ctx = calloc(1, sizeof(*ctx));
  if (!ctx)
    return LIBUSB_ERROR_NO_MEM;

  ctx->readout_timeout_ = 1000;
  ctx->verbose_ = 1;

  if (ufe_context_handler) {
    free(ufe_context_handler);
  }

  if (*context) {
    free(*context);
  }

  ufe_context_handler = ctx;
  *context = ctx;
  (*context)->usb_ctx_ = NULL;

  return 0;
}

int ufe_init(ufe_context **context) {
  if (*context == NULL && ufe_context_handler == NULL) {
    // No ufe_context exists. Create a default one.
    ufe_default_context(context);
  } else if (*context != NULL) {
    // The context handler is already set. Use it.
    ufe_context_handler = *context;
  } else {
    // The context is already set. Use it.
    *context = ufe_context_handler;
  }

  CRC_16_1A2EB_INIT(&crc16_context_handler);
  CRC_21_21BF1F_INIT(&crc21_context_handler);

  return libusb_init(&(*context)->usb_ctx_);
}

void ufe_set_verbose(ufe_context *ctx, int libusb_level, int ufe_level) {
  libusb_set_debug(ctx->usb_ctx_, libusb_level);
  ctx->verbose_ = ufe_level;
}

void ufe_set_readout_timeout(ufe_context *ctx, int t) {
  ctx->readout_timeout_ = t;
}

size_t ufe_get_device_list(libusb_context *ctx, libusb_device ***feb_devs) {
  int dummy_arg=0;
  return ufe_get_custom_device_list(ctx, &is_ufe, feb_devs, dummy_arg);
}

size_t ufe_get_bm_device_list(libusb_context *ctx, libusb_device ***feb_devs) {
  int dummy_arg=0;
  return ufe_get_custom_device_list(ctx, &is_bm_feb, feb_devs, dummy_arg);
}

int ufe_open(libusb_device *dev, libusb_device_handle **handle) {
  return libusb_open(dev, handle);
}

void ufe_close(libusb_device_handle *handle) {
  libusb_close(handle);
}

ufe_context* ufe_get_context() {
  return ufe_context_handler;
}

void ufe_exit(ufe_context *ctx) {
  libusb_exit(ctx->usb_ctx_);
  free(ctx);
}

bool ufe_ping(libusb_device_handle *dev_handle, uint8_t board_id) {
  uint16_t buff;

  int xVerbose = ufe_context_handler->verbose_;
  ufe_context_handler->verbose_ = -1;
  int status = ufe_read_status(dev_handle, board_id, &buff);
  ufe_context_handler->verbose_ = xVerbose;

  if (status != 0) {
    ufe_info_print("board %i is unreachable.", board_id);
    return false;
  }

  ufe_info_print("board %i found.", board_id);
  return true;
}

int ufe_get_version(libusb_device_handle *ufe, int *data) {

  uint16_t value = 0, lenght = 2;
  *data = 7;

  int status = libusb_control_transfer( ufe,
                                        CLASS_REQUEST | LIBUSB_ENDPOINT_IN,
                                        UFE_GET_VERSION_REQ,
                                        value,
                                        0,
                                        (uint8_t*) data,
                                        lenght,
                                        UFE_CMD_TIMEOUT);
  if(status != lenght) {
    ufe_error_print("get_version failed ( %i, %2x )", status, *data);
    free(data);
    return UFE_INVALID_CMD_ANSWER_ERROR;
  } else
    ufe_debug_print("get_version ( %i, 0x%4x )", status, *data);

  usleep(1);
  return 0;
}

int ufe_get_buff_size(libusb_device_handle *ufe, uint64_t *data) {

  uint16_t value = 0, lenght = 8;
  *data = 7;

  int status = libusb_control_transfer( ufe,
                                        CLASS_REQUEST | LIBUSB_ENDPOINT_IN,
                                        UFE_GET_BUF_SIZE,
                                        value,
                                        0,
                                        (uint8_t*) data,
                                        lenght,
                                        UFE_CMD_TIMEOUT);
  if(status != lenght) {
    ufe_error_print("get_buff_size failed ( %i, %" PRId64 " )", status, *data);
    free(data);
    return UFE_INVALID_CMD_ANSWER_ERROR;
  } else
    ufe_debug_print("get_buff_size ( %i, 0x%" PRIx64 " ): ", status, *data);


  usleep(1);
  return 0;
}

int ufe_enable_led(libusb_device_handle *ufe, bool enable) {

  uint16_t value = (uint16_t)(!enable), lenght = 1;
  uint8_t data = 7;

  int status = libusb_control_transfer( ufe,
                                        CLASS_REQUEST | LIBUSB_ENDPOINT_IN,
                                        UFE_LED_OFF_REQ,
                                        value,
                                        0,
                                        &data,
                                        lenght,
                                        UFE_CMD_TIMEOUT);
  if(status != lenght || data != value) {
    ufe_error_print("enable_led failed ( %i, %i )", status, data);
    return UFE_INVALID_CMD_ANSWER_ERROR;
  } else
    ufe_debug_print("enable_led ( %i, %i )", status, data);

  usleep(1);
  return 0;
}

int ufe_ep2in_wrappup(libusb_device_handle *ufe) {

  uint16_t value = 2, lenght = 1;
  uint8_t data = 7;

  int status = libusb_control_transfer( ufe,
                                        CLASS_REQUEST | LIBUSB_ENDPOINT_IN,
                                        UFE_EP2IN_WRAPPUP_REQ,
                                        value,
                                        0,
                                        &data,
                                        lenght,
                                        UFE_CMD_TIMEOUT);
  if(status != lenght || data != value) {
    ufe_error_print("ep2in_wrappup failed ( %i, %i )", status, data);
    return UFE_INVALID_CMD_ANSWER_ERROR;
  } else
    ufe_debug_print("ep2in_wrappup ( %i, %i )", status, data);

  usleep(1);
  return 0;
}

int ufe_epxin_reset(libusb_device_handle *ufe, int ep_id) {

  uint16_t value = ep_id, lenght = 1;
  uint8_t data = 7;

  int status = libusb_control_transfer( ufe,
                                        CLASS_REQUEST | LIBUSB_ENDPOINT_IN,
                                        UFE_EPxIN_RESET_REQ,
                                        value,
                                        0,
                                        &data,
                                        lenght,
                                        UFE_CMD_TIMEOUT);
  if(status != lenght || data != value) {
    ufe_error_print("epxin_reset failed on EP %i ( %i, %i )", ep_id, status, data);
    return UFE_INVALID_CMD_ANSWER_ERROR;
  } else
    ufe_debug_print("epxin_reset on EP %i ( %i, %i )", ep_id, status, data);

  usleep(1);
  return 0;
}


int ufe_read_buffer(libusb_device_handle *ufe, uint8_t* data, size_t size, int *actual) {
  // Prepare the End Point identifier.
  uint8_t ep_id = UFE_USB_EP1_IN | LIBUSB_ENDPOINT_IN;

  // Make bulk transfer.
  int status = libusb_bulk_transfer( ufe,
                                     ep_id,
                                     data,
                                     size,
                                     actual,
//                                      UFE_CMD_TIMEOUT);
                                     ufe_context_handler->readout_timeout_);

  ufe_debug_print("data resieved from EP 1 ( %i, %g KB): 0x%x", status, (*actual)/1024., *((uint32_t *) data));

  return status;
}


int ufe_idle(libusb_device_handle *ufe, int board_id) {
  ufe_info_print("executing command IDLE ( %i )", board_id);

  int command_id = IDLE_CMD_ID;
  int status = ufe_send_command_req( ufe,
                                     board_id,
                                     command_id,
                                     NO_SUB_CMD_ID,
                                     0,
                                     NULL);
  return status;
}

int ufe_firmware_version(libusb_device_handle *ufe, int board_id, int *data) {
  ufe_info_print("executing command FIRMWARE_VERSION ( board %i )", board_id);

  int command_id = FIRMWARE_VERSION_CMD_ID;
  int status=0;
  uint16_t *data_16 = (uint16_t*) data;

  status = ufe_send_command_req( ufe,
                                 board_id,
                                 command_id,
                                 NO_SUB_CMD_ID,
                                 1,
                                 data_16);
  if (status != 0)
    return status;

  usleep(1);
  status = ufe_ep2in_wrappup(ufe);

  if (status != 0)
    return status;

  usleep(1);
  status = ufe_get_command_answer( ufe,
                                   board_id,
                                   command_id,
                                   NO_SUB_CMD_ID,
                                   1,
                                   &data_16);
  usleep(1);
  return status;
}

int ufe_set_direct_param(libusb_device_handle *ufe, int board_id, uint16_t *data) {
  ufe_info_print("executing command SET_DIRECT_PARAM ( board %i )", board_id);

  int command_id = SET_DIRECT_PARAM_CMD_ID;
  int status=0;

  status = ufe_send_command_req( ufe,
                                 board_id,
                                 command_id,
                                 NO_SUB_CMD_ID,
                                 1,
                                 data);
  if (status != 0)
    return status;

  usleep(1);
  status = ufe_ep2in_wrappup(ufe);

  if (status != 0)
    return status;

  usleep(1);
  status = ufe_get_command_answer( ufe,
                                   board_id,
                                   command_id,
                                   NO_SUB_CMD_ID,
                                   0,
                                   &data);
  usleep(1);
  return status;
}

int ufe_read_status(libusb_device_handle *ufe, int board_id, uint16_t *data) {
  ufe_info_print("executing command READ_STATUS ( board %i )", board_id);

  int command_id = READ_STATUS_CMD_ID;
  int status=0;

  status = ufe_send_command_req( ufe,
                                 board_id,
                                 command_id,
                                 NO_SUB_CMD_ID,
                                 0,
                                 NULL);
  if (status != 0)
    return status;

  usleep(1);
  status = ufe_ep2in_wrappup(ufe);

  if (status != 0)
    return status;

  usleep(1);
  status = ufe_get_command_answer( ufe,
                                   board_id,
                                   command_id,
                                   NO_SUB_CMD_ID,
                                   1,
                                   &data);
  usleep(1);
  return status;
}

int ufe_set_config(libusb_device_handle *ufe, int board_id, int device, uint32_t *data) {
  ufe_info_print("executing command SET_CONFIG ( board %i, device %i )", board_id, device);

  int command_id = SET_CONFIG_CMD_ID, argc = 72;
  int status=0;
  uint16_t *data_16 = (uint16_t*) data;

// #ifdef UFE_DEBUG
//   printf("### Debug: sending configuration data\n");
//   int i;
//   for (i=0; i<argc; ++i)
//     printf("### Debug: %i 0x%x \n", i, data_16[i]);
// #endif

  status = ufe_send_command_req( ufe,
                                 board_id,
                                 command_id,
                                 device,
                                 argc,
                                 data_16);
  if (status != 0)
    return status;

  usleep(argc*100);
  status = ufe_ep2in_wrappup(ufe);

  if (status != 0)
    return status;

  usleep(1);
  status = ufe_get_command_answer( ufe,
                                   board_id,
                                   command_id,
                                   NO_SUB_CMD_ID,
                                   0,
                                   &data_16);

  if (status != 0)
    return status;

  usleep(1);
  // Now validate the configuration.
  uint16_t code = 0;
  switch (device) {
    case 0:
      code = UFE_SC_VALIDATE_D0; break;

    case 1:
      code = UFE_SC_VALIDATE_D1; break;

    case 2:
      code = UFE_SC_VALIDATE_D2; break;

    case 3:
      code = UFE_SC_VALIDATE_D3; break;
  }

  status = ufe_send_command_req( ufe,
                                 board_id,
                                 command_id,
                                 device+8,
                                 1,
                                 &code);
  if (status != 0)
    return status;

  usleep(argc*100);
  status = ufe_ep2in_wrappup(ufe);

  if (status != 0)
    return status;

  usleep(1);
  status = ufe_get_command_answer( ufe,
                                   board_id,
                                   command_id,
                                   NO_SUB_CMD_ID,
                                   0,
                                   &data_16);
  usleep(1);
  return status;
}

int ufe_get_config(libusb_device_handle *ufe, int board_id, int device, uint32_t *data) {
  ufe_info_print("executing command GET_CONFIG ( board %i, device %i )", board_id, device);

  int command_id = GET_CONFIG_CMD_ID, argc = 72;
  int status=0;
  uint16_t *data_16 = (uint16_t*) data;

  status = ufe_send_command_req( ufe,
                                 board_id,
                                 command_id,
                                 NO_SUB_CMD_ID,
                                 1,
                                 (uint16_t*) &device);

  if (status != 0)
    return status;

  usleep(argc*500);
  status = ufe_ep2in_wrappup(ufe);

  if (status != 0)
    return status;

  usleep(argc*100);
  status = ufe_get_command_answer( ufe,
                                   board_id,
                                   command_id,
                                   device,
                                   argc,
                                   &data_16);

// #ifdef UFE_DEBUG
//   printf("### Debug: configuration data received\n");
//   int i;
//   for (i=0; i<argc; ++i)
//     printf("### Debug: %i 0x%x \n", i, data_16[i]);
// #endif

  usleep(1);
  return status;
}

int ufe_apply_config(libusb_device_handle *ufe, int board_id, uint16_t* data) {
  ufe_info_print("executing command APPLY_CONFIG ( board %i, device %i )", board_id, *data);

  int command_id = APPLY_CONFIG_CMD_ID;
  int status=0;

  status = ufe_send_command_req( ufe,
                                 board_id,
                                 command_id,
                                 NO_SUB_CMD_ID,
                                 1,
                                 data);
  if (status != 0)
    return status;

  usleep(700);
  status = ufe_ep2in_wrappup(ufe);

  if (status != 0)
    return status;

  usleep(100);
  status = ufe_get_command_answer( ufe,
                                   board_id,
                                   command_id,
                                   NO_SUB_CMD_ID,
                                   0,
                                   &data);
  usleep(1);
  return status;
}

int ufe_data_readout(libusb_device_handle *ufe, int board_id, uint16_t *data) {
  ufe_info_print("executing command DATA_READOUT ( board %i, arg 0x%x )", board_id, *data);

  int command_id = DATA_READOUT_CMD_ID, argc = 1;
  int status=0;
  uint16_t *data_16 = (uint16_t*) malloc(2);

  status = ufe_send_command_req( ufe,
                                 board_id,
                                 command_id,
                                 NO_SUB_CMD_ID,
                                 argc,
                                 data);

  if (status != 0)
    return status;

  usleep(1);
  status = ufe_ep2in_wrappup(ufe);

  if (status != 0)
    return status;


  usleep(1);
  status = ufe_get_command_answer( ufe,
                                   board_id,
                                   command_id,
                                   NO_SUB_CMD_ID,
                                   argc,
                                   &data_16);
  usleep(1);
  free(data_16);
  return status;
}


const char * get_command_name(int command_id) {
  char *name = (char*) malloc(20);
  switch (command_id) {
    case DATA_READOUT_CMD_ID:
      return "DATA_READOUT";

    case FIRMWARE_VERSION_CMD_ID:
      return "FIRMWARE_VERSION";

    case SET_DIRECT_PARAM_CMD_ID:
      return "SET_DIRECT_PARAM";

    case READ_STATUS_CMD_ID:
      return "READ_STATUS";

    case SET_CONFIG_CMD_ID:
      return "SET_CONFIG";

    case GET_CONFIG_CMD_ID:
      return "GET_CONFIG";

    case APPLY_CONFIG_CMD_ID:
      return "APPLY_CONFIG";

    case ERROR_CMD_ID:
      return "ERROR";

    case IDLE_CMD_ID:
      return "IDLE_CMD_ID";

    default:
      return "UNKNOWN_CMD";
  }

  return name;
}

void dump_status(uint16_t status) {
  printf("GTEN: ....... %i\n", !!(status & RS_GTEN));
  printf("AVE: ........ %i\n", !!(status & RS_AVE));
  printf("L0F_ERR: .... %i\n", !!(status & RS_L0F_ERR));
  printf("L1F_ERR: .... %i\n", !!(status & RS_L1F_ERR));
  printf("L2F_ERR: .... %i\n", !!(status & RS_L2F_ERR));
  printf("MUX_ERR: .... %i\n", !!(status & RS_MUX_ERR));
  printf("L1_ADC_ERR: . %i\n", !!(status & RS_L1_ADC_ERR));
  printf("VW_ASIC0: ... %i\n", !!(status & RS_VW_ASIC0));
  printf("VW_ASIC1: ... %i\n", !!(status & RS_VW_ASIC1));
  printf("VW_ASIC2: ... %i\n", !!(status & RS_VW_ASIC2));
  printf("VW_FPGA: .... %i\n", !!(status & RS_VW_FPGA));
  printf("HVON: ....... %i\n", !!(status & RS_HVON));
  printf("IGEN: ....... %i\n", !!(status & RS_IGEN));
}

void dump_direct_params(uint16_t params) {
  printf("RSTA: ....... %i\n", !!(params & SDP_RSTA));
  printf("RTTA: ....... %i\n", !!(params & SDP_RTTA));
  printf("RL0F: ....... %i\n", !!(params & SDP_RL0F));
  printf("RL1F: ....... %i\n", !!(params & SDP_RL1F));
  printf("RL2F: ....... %i\n", !!(params & SDP_RL2F));
  printf("RADC: ....... %i\n", !!(params & SDP_RADC));
  printf("GTEN: ....... %i\n", !!(params & SDP_GTEN));
  printf("AVE: ........ %i\n", !!(params & SDP_AVE));
  printf("HVON: ....... %i\n", !!(params & SDP_HVON));
  printf("RDEN: ....... %i\n", !!(params & SDP_RDEN));
  printf("IGEN: ....... %i\n", !!(params & SDP_IGEN));
  printf("FCLR: ....... %i\n", !!(params & SDP_FCLR));
}

void dump_readout_params(uint16_t params) {
  if ( !!(params & DR_STOP) )
    printf("STOP: ....... true\n");
  else
    printf("START: ...... true\n");

  printf("GTEN: ....... %i\n", !!(params & DR_GTEN));
  printf("AVE:. ....... %i\n", !!(params & DR_AVE));
  printf("RDEN: ....... %i\n", !!(params & DR_RDEN));
  printf("IGEN: ....... %i\n", !!(params & DR_IGEN));
}
