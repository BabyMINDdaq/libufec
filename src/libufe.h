#ifndef LIBUFE_H
#define LIBUFE_H 1

#include <stdbool.h>
#include <stdint.h>

#include <libusb-1.0/libusb.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ufe_context {
  int debug;
  unsigned int timeout;

  libusb_context* usb_ctx;
};

int ufe_init(struct ufe_context **ctx);

void ufe_set_debug(struct ufe_context *ctx, int libusb_level, int ufe_level);

void ufe_set_timeout(struct ufe_context *ctx, int t);

size_t ufe_get_device_list(libusb_context *ctx, libusb_device ***feb_devs);

void ufe_exit(struct ufe_context *ctx);

int ufe_get_version(libusb_device_handle *ufe, int *data);

int ufe_get_buff_size(libusb_device_handle *ufe, uint64_t *data);

int ufe_enable_led(libusb_device_handle *ufe, bool enable);

int ufe_ep2in_wrappup(libusb_device_handle *ufe);

int ufe_epxin_reset(libusb_device_handle *ufe, int ep_id);

int ufe_idle(libusb_device_handle *ufe, int board_id);

int ufe_data_readout(libusb_device_handle *ufe, int board_id, int start);

int ufe_firmware_version(libusb_device_handle *ufe, int board_id, int *data);

int ufe_set_direct_param(libusb_device_handle *ufe, int board_id, uint16_t *data);

int ufe_read_status(libusb_device_handle *ufe, int board_id, uint16_t *data);

int ufe_set_config(libusb_device_handle *ufe, int board_id, int device, uint32_t *data);

int ufe_get_config(libusb_device_handle *ufe, int board_id, int device, uint32_t *data);

int ufe_apply_config(libusb_device_handle *ufe, int board_id, int device);

int send_command_req( libusb_device_handle *ufe,
                      int board_id,
                      int command_id,
                      int sub_cmd_id,
                      int argc,
                      uint16_t *argv,
                      int ep);

int get_command_answer( libusb_device_handle *ufe,
                        int board_id,
                        int command_id,
                        int sub_cmd_id,
                        int argc,
                        uint16_t **argv,
                        int ep);

int ufe_user_set_sync( libusb_device_handle *ufe, int ep, int size, uint8_t *data);

int ufe_user_get_sync( libusb_device_handle *ufe, int ep, int size, uint8_t *data);

enum ufe_cmd_id {
  DATA_READOUT_CMD_ID     = 0x0,
  FIRMWARE_VERSION_CMD_ID = 0x01,
  SET_DIRECT_PARAM_CMD_ID = 0x02,
  READ_STATUS_CMD_ID      = 0x03,
  SET_CONFIG_CMD_ID       = 0x04,
  GET_CONFIG_CMD_ID       = 0x05,
  APPLY_CONFIG_CMD_ID     = 0x06,
  ERROR_CMD_ID            = 0x1E,
  IDLE_CMD_ID             = 0x1F
};

enum ufe_error {
  UFE_INTERNAL_ERROR           = -13,
  UFE_IO_ERROR                 = -14,
  UFE_INVALID_CMD_ANSWER_ERROR = -15,
  UFE_INVALID_ARG_ERROR        = -16,
  UFE_NOT_FOUND_ERROR          = -17
};

#define NO_SUB_CMD_ID -1

#define UFE_TIMEOUT 1000

enum ufe_data_readout_args {
  DR_START = 0x0,
  DR_STOP  = 0x1

};
enum ufe_set_dir_par_args {
  SDP_RSSR = 0x1,
  SDP_RSTA = 0x2,
  SDP_RTTA = 0x4,
  SDP_RL0F = 0x8,
  SDP_RL1F = 0x10,
  SDP_RL2F = 0x20,
  SDP_RADC = 0x40,
  SDP_GTEN = 0x80,
  SDP_AVE  = 0x100,
  SDP_HVON = 0x200,
  SDP_RDEN = 0x400,
  SDP_IGEN = 0x800,
  SDP_FCLR = 0x8000
};

enum ufe_read_status_args {
  RS_GTEN       = 0x1,
  RS_AVE        = 0x2,
  RS_L0F_ERR    = 0x4,
  RS_L1F_ERR    = 0x8,
  RS_L2F_ERR    = 0x10,
  RS_MUX_ERR    = 0x20,
  RS_L1_ADC_ERR = 0x40,
  RS_VW_ASIC0   = 0x80,
  RS_VW_ASIC1   = 0x100,
  RS_VW_ASIC2   = 0x200,
  RS_VW_FPGA    = 0x400,
  RS_HVON       = 0x800,
  RS_IGEN       = 0x1000
};

enum ufe_apply_config_cmd_args {
  AC_CMD_ASIC0    = 0x1,
  AC_CMD_ASIC1    = 0x2,
  AC_CMD_ASIC2    = 0x4,
  AC_CMD_FPGA     = 0x8
};

enum ufe_apply_config_answ_args {
  AC_ANSW_WWID  = 0x1,
  AC_ANSW_WCID  = 0x2,
  AC_ANSW_WSCID = 0x4,
  AC_ANSW_WFIDX = 0x8,
  AC_ANSW_ASIC0 = 0x10,
  AC_ANSW_ASIC1 = 0x20,
  AC_ANSW_ASIC2 = 0x40,
  AC_ANSW_FPGA  = 0x80,
  AC_ANSW_WVAL  = 0x2000
};

#ifdef __cplusplus
}
#endif

#endif