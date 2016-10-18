#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* malloc, free, rand */
#include <unistd.h>
#include <inttypes.h>

#include <libusb-1.0/libusb.h>

#include "libufe.h"
#include "libufe-def.h"

#define UFE_GET_CONTEXT(ctx) \
  do { \
    if (!(ctx)) \
      (ctx) = ufe_default_context; \
  } while(0)

bool is_bm_feb(libusb_device *dev) {

  struct libusb_device_descriptor desc;
  int status = libusb_get_device_descriptor(dev, &desc);
  if (status < 0) {
    fprintf(stderr, "!!! Failed to get device descriptor Error: %i\n", status);
    return false;
  }

  if (desc.idVendor == UFE_VENDOR_ID)
    return true;

  return false;
}

int ufe_init(struct ufe_context **context) {
  struct ufe_context *ctx;
  ctx = calloc(1, sizeof(*ctx));
  if (!ctx)
    return LIBUSB_ERROR_NO_MEM;

  ctx->timeout = 1000;
  ctx->debug   = 0;
  if (context)
    *context = ctx;

  ctx->usb_ctx = NULL;
  return libusb_init(&ctx->usb_ctx);
}

void ufe_set_debug(struct ufe_context *ctx, int libusb_level, int ufe_level) {
  libusb_set_debug(ctx->usb_ctx, libusb_level);
  ctx->debug = ufe_level;
}

void ufe_set_timeout(struct ufe_context *ctx, int t) {
  ctx->timeout = t;
}

size_t ufe_get_device_list(libusb_context *ctx, libusb_device ***feb_devs) {
  libusb_device **devs;
  size_t n_devs = libusb_get_device_list(ctx, &devs); //get the list of devices
  size_t n_febs = 0;

  if(n_devs < 0) {
    fprintf(stderr, "!!! Device Error\n"); //there was an error
    return 0;
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

void ufe_exit(struct ufe_context *ctx) {
  libusb_exit(ctx->usb_ctx);
  free(ctx);
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
                                        UFE_TIMEOUT);
  if(status != lenght) {
    fprintf(stderr, "!!! Error: get_version failed (%i, %2x)\n", status, *data);
    free(data);
    return UFE_INVALID_CMD_ANSWER_ERROR;
  } else
#ifdef UFE_DEBUG
    printf("### Debug: get_version (%i, 0x%4x)\n", status, *data);
#endif

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
                                        UFE_TIMEOUT);
  if(status != lenght) {
    fprintf(stderr, "!!! Error: get_buff_size failed (%i, %" PRId64 ")\n", status, *data);
    free(data);
    return UFE_INVALID_CMD_ANSWER_ERROR;
  } else
#ifdef UFE_DEBUG
    printf("### Debug: get_buff_size (%i, 0x%" PRIx64 "): \n", status, *data);
#endif

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
                                        UFE_TIMEOUT);
  if(status != lenght || data != value) {
    fprintf(stderr, "!!! Error: enable_led failed (%i, %i)\n", status, data);
    return UFE_INVALID_CMD_ANSWER_ERROR;
  } else
#ifdef UFE_DEBUG
    printf("### Debug: enable_led (%i, %i)\n", status, data);
#endif

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
                                        UFE_TIMEOUT);
  if(status != lenght || data != value) {
    fprintf(stderr, "!!! Error: ep2in_wrappup failed (%i, %i)\n", status, data);
    return UFE_INVALID_CMD_ANSWER_ERROR;
  } else
#ifdef UFE_DEBUG
    printf("### Debug: ep2in_wrappup (%i, %i)\n", status, data);
#endif

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
                                        UFE_TIMEOUT);
  if(status != lenght || data != value) {
    fprintf(stderr, "!!! Error: epxin_reset failed on EP %i (%i, %i)\n", ep_id, status, data);
    return UFE_INVALID_CMD_ANSWER_ERROR;
  } else
#ifdef UFE_DEBUG
    printf("### Debug: epxin_reset on EP %i (%i, %i)\n", ep_id, status, data);
#endif

  usleep(1);
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
    fprintf(stderr, "!!! Error: invalid End point id %i\n", ep);
    return UFE_INVALID_ARG_ERROR;
  }

  // Make bulk transfer.
  int actual;
  int status = libusb_bulk_transfer( ufe,
                                     ep_id,
                                     data,
                                     size,
                                     &actual,
                                     UFE_TIMEOUT);

  if(status == 0 && actual == size) {
    //we wrote successfully

#ifdef UFE_DEBUG

    // Print in a proper format. Only in debugging mode.
    switch (size) {
      case 1:
        printf("### Debug: command sent (%i, %i): 0x%x\n", status, actual, *((uint8_t *) data));
        break;
      case 2:
        printf("### Debug: command sent (%i, %i): 0x%x\n", status, actual, *((uint16_t *) data));
        break;
      default:
        printf("### Debug: command sent (%i, %i): 0x%x\n", status, actual, *((uint32_t *) data));
        break;
    }

#endif

    return 0;
  }

  fprintf(stderr, "!!! Error: answer not resieved (%i, %i)\n", status, actual);

  if (status < 0)
    return status;

  return UFE_IO_ERROR;
}

int ufe_user_get_sync( libusb_device_handle *ufe, int ep, int size, uint8_t *data) {

  // Prepare the End Point identifier.
  uint8_t ep_id;
  if (ep == 1) 
    ep_id = UFE_USB_EP1_IN | LIBUSB_ENDPOINT_IN;
  else if (ep == 2)
    ep_id = UFE_USB_EP2_IN | LIBUSB_ENDPOINT_IN;
  else {
    fprintf(stderr, "!!! Error: invalid End point id %i", ep);
    return UFE_INVALID_ARG_ERROR;
  }

  // Make bulk transfer.
  int actual;
  int status = libusb_bulk_transfer( ufe,
                                     ep_id,
                                     data,
                                     size,
                                     &actual,
                                     UFE_TIMEOUT);

  if(status == 0 && actual == size) {
    //we read successfully

#ifdef UFE_DEBUG

    // Print in a proper format. Only in debugging mode.
    switch (size) {
      case 1:
        printf("### Debug: answer resieved (%i, %i): 0x%x\n", status, actual, *((uint8_t *) data));
        break;
      case 2:
        printf("### Debug: answer resieved (%i, %i): 0x%x\n", status, actual, *((uint16_t *) data));
        break;
      default:
        printf("### Debug: answer resieved (%i, %i): 0x%x\n", status, actual, *((uint32_t *) data));
        break;
    }

#endif

    return 0;
  }

  fprintf(stderr, "!!! Error: answer not resieved (%i, %i)\n", status, actual);

  if (status < 0)
    return status;

  return UFE_IO_ERROR;
}

int ufe_send_command_req( libusb_device_handle *ufe,
                      int board_id,
                      int command_id,
                      int sub_cmd_id,
                      int argc,
                      uint16_t *argv,
                      int ep) {

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
    cmd[argc+1] |= (sub_cmd_id << UFE_SUBCMD_ID_SHIFT) & UFE_SUBCMD_ID_MASK;
//     printf("trailer: 0x%4x \n", cmd[xArg+1]);
  }

  // Send the command.
  int actual=0;
  uint8_t *cmd_8 = (uint8_t*) cmd;
  while (actual < size) {
    // Max size, allowed for a single transfer is 256.
    // If the command buffer is bigger, make multiple transfers.
    int tr_size = ( (size-actual) < 256 )? size-actual : 256;
//     int tr_size = 2;
    int status = ufe_user_set_sync(ufe, ep, tr_size, cmd_8);
    if (status < 0)
      return status;

//     int i;
//     for (i=0;i<tr_size;++i)
//       printf("%i: %4x \n", i, cmd_8[i]);

    cmd_8 += tr_size;
    actual += tr_size;
    usleep(10);
  }

  free(cmd);
  return 0;
}

int ufe_get_command_answer( libusb_device_handle *ufe,
                        int board_id,
                        int command_id,
                        int sub_cmd_id,
                        int argc,
                        uint16_t **argv,
                        int ep) {

  // Allocate memory for the answer according to the number of arguments.
  int size = (argc > 1)? ((argc+2)*4) : 4;
  uint32_t *answer = malloc(size);

  // Get the command answer.
  int actual=0;
  uint8_t *answer_8 = (uint8_t*) answer;

  while (actual < size) {
    // Max size, allowed for a single transfer is 256.
    // If the command buffer is bigger, make multiple transfers.
    int tr_size = ( (size-actual) < 256 )? size-actual : 256;
//     int tr_size = 4;
    int status = ufe_user_get_sync(ufe, ep, tr_size, answer_8);
    if (status < 0)
      return status;

//     int i;
//     for (i=0;i<tr_size;++i)
//       printf("%i: %4x \n", i, answer_8[i]);

    answer_8 += tr_size;
    actual   += tr_size;
    usleep(10);
  }

  // Check for firmware errors.
  if ( (*answer & UFE_CMD_ID_MASK) >>  UFE_CMD_ID_SHIFT == ERROR_CMD_ID ) {
    fprintf(stderr, "!!! Firmware Error (0x%4x)\n", *answer);
    free(answer);
    return UFE_INTERNAL_ERROR;
  }

  // Verify the consistency of the answer.
  // Check the header.
  if ( (*answer & UFE_DW_ID_MASK)    >>  UFE_DW_ID_SHIFT   != CMD_HEADER_ID   ||
       (*answer & UFE_BOARD_ID_MASK) >>  UFE_BOARD_ID_SHIFT != board_id ||
       (*answer & UFE_CMD_ID_MASK)   >>  UFE_CMD_ID_SHIFT   != command_id ) {
    fprintf(stderr, "!!! Error: inconsistent answer header (0x%4x)\n", *answer);
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
      fprintf(stderr, "!!! Error: inconsistent answer header (0x%4x)\n", *answer);
      free(answer);
      return UFE_INVALID_CMD_ANSWER_ERROR;
    }
  }

  // Check the number of arguments.
  if (argc > 1) {
    if ( (*answer & UFE_ARG_FR_NUM_MASK) != argc ) {
      fprintf(stderr, "!!! Error: inconsistent answer header (0x%4x)\n", *answer);
      free(answer);
      return UFE_INVALID_CMD_ANSWER_ERROR;
    }

    // Loop over the arguments.
    int i;
    for (i=0; i<argc; ++i) {
      // Check the consistency of the argument.
      if ( (answer[i+1] & UFE_DW_ID_MASK)       >> UFE_DW_ID_SHIFT       != CMD_ARG_ID ||
           (answer[i+1] & UEF_FRAME_INDEX_MASK) >> UEF_FRAME_INDEX_SHIFT != i ) {
        fprintf(stderr, "!!!  Error: inconsistent answer argument %i (0x%4x)\n",i , answer[i+1]);
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
      fprintf(stderr, "!!! Error: inconsistent answer trailer (0x%4x)\n", answer[argc+1]);
      free(answer);
      return UFE_INVALID_CMD_ANSWER_ERROR;
    }
  }

  free(answer);
  return 0;
}

int ufe_idle(libusb_device_handle *ufe, int board_id) {
#ifdef UFE_INFO
  printf("+++ Info: executing command IDLE (%i)\n", board_id);
#endif

  int command_id = IDLE_CMD_ID;
  int status = ufe_send_command_req( ufe,
                                     board_id,
                                     command_id,
                                     NO_SUB_CMD_ID,
                                     0,
                                     NULL,
                                     2);
  return status;
}

int ufe_firmware_version(libusb_device_handle *ufe, int board_id, int *data) {
#ifdef UFE_INFO
  printf("+++ Info: executing command FIRMWARE_VERSION (board %i)\n", board_id);
#endif

  int command_id = FIRMWARE_VERSION_CMD_ID;
  int status=0;
  uint16_t *data_16 = (uint16_t*) data;

  status = ufe_send_command_req( ufe,
                                 board_id,
                                 command_id,
                                 NO_SUB_CMD_ID,
                                 1,
                                 data_16,
                                 2);
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
                                   &data_16,
                                   2);
  usleep(1);
  return status;
}

int ufe_set_direct_param(libusb_device_handle *ufe, int board_id, uint16_t *data) {
#ifdef UFE_INFO
  printf("+++ Info: executing command SET_DIRECT_PARAM (board %i)\n", board_id);
#endif

  int command_id = SET_DIRECT_PARAM_CMD_ID;
  int status=0;

  status = ufe_send_command_req( ufe,
                                 board_id,
                                 command_id,
                                 NO_SUB_CMD_ID,
                                 1,
                                 data,
                                 2);
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
                                   &data,
                                   2);
  usleep(1);
  return status;
}

int ufe_read_status(libusb_device_handle *ufe, int board_id, uint16_t *data) {
#ifdef UFE_INFO
  printf("+++ Info: executing command READ_STATUS (board %i)\n", board_id);
#endif

  int command_id = READ_STATUS_CMD_ID;
  int status=0;

  status = ufe_send_command_req( ufe,
                                 board_id,
                                 command_id,
                                 NO_SUB_CMD_ID,
                                 0,
                                 NULL,
                                 2);
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
                                   &data,
                                   2);
  usleep(1);
  return status;
}

int ufe_set_config(libusb_device_handle *ufe, int board_id, int device, uint32_t *data) {
#ifdef UFE_INFO
  printf("+++ Info: executing command SET_CONFIG (board %i, device %i)\n", board_id, device);
#endif

  int command_id = SET_CONFIG_CMD_ID, argc = 72;
  int status=0;
  uint16_t *data_16 = (uint16_t*) data;

  status = ufe_send_command_req( ufe,
                                 board_id,
                                 command_id,
                                 device,
                                 argc,
                                 data_16,
                                 2);
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
                                   0,
                                   1,
                                   &data_16,
                                   2);
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
                                 &code,
                                 2);
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
                                   1,
                                   &data_16,
                                   2);
  usleep(1);

  return status;
}

int ufe_get_config(libusb_device_handle *ufe, int board_id, int device, uint32_t *data) {
#ifdef UFE_INFO
  printf("+++ Info: executing command GET_CONFIG (board %i, device %i)\n", board_id, device);
#endif

  int command_id = GET_CONFIG_CMD_ID, argc = 72;
  int status=0;
  uint16_t *data_16 = (uint16_t*) data;

  status = ufe_send_command_req( ufe,
                                 board_id,
                                 command_id,
                                 NO_SUB_CMD_ID,
                                 1,
                                 (uint16_t*) &device,
                                 2);

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
                                   &data_16,
                                   2);

  usleep(1);
  return status;
}

int ufe_apply_config(libusb_device_handle *ufe, int board_id, int device) {
#ifdef UFE_INFO
  printf("+++ Info: executing command APPLY_CONFIG (board %i, device %i)\n", board_id, device);
#endif
  int command_id = APPLY_CONFIG_CMD_ID, argc = 1;
  int status=0;
  uint16_t *data_16 = (uint16_t*) malloc(2);
  *data_16 = (1 << device);

  status = ufe_send_command_req( ufe,
                                 board_id,
                                 command_id,
                                 NO_SUB_CMD_ID,
                                 argc,
                                 data_16,
                                 2);
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
                                   &data_16,
                                   2);
  usleep(1);
  free(data_16);
  return status;
}

int ufe_data_readout(libusb_device_handle *ufe, int board_id, int start) {
#ifdef UFE_INFO
  printf("+++ Info: executing command DATA_READOUT (board %i, arg %i)\n", board_id, start);
#endif
  int command_id = DATA_READOUT_CMD_ID, argc = 1;
  int status=0;
  uint16_t *data_16 = (uint16_t*) malloc(2);
  *data_16 = start & 1;

  status = ufe_send_command_req( ufe,
                                 board_id,
                                 command_id,
                                 NO_SUB_CMD_ID,
                                 argc,
                                 data_16,
                                 2);

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
                                   &data_16,
                                   2);
  usleep(1);
  free(data_16);
  return status;
}
