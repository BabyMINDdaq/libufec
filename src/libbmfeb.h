#ifndef LIBBMFEB_H
#define LIBBMFEB_H 1

#include <stdbool.h>
#include <libusb-1.0/libusb.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t get_bmfeb_device_list(libusb_context *ctx, libusb_device ***feb_devs);

int get_version_req(libusb_device_handle *bmfeb, uint8_t *data);

int get_buff_size_req(libusb_device_handle *bmfeb, uint8_t *data);

int enable_led_req(libusb_device_handle *bmfeb, bool enable);

int ep2in_wrappup_req(libusb_device_handle *bmfeb, uint8_t *data);

int epxin_reset_req(libusb_device_handle *bmfeb, uint8_t *data);

int send_command_req( libusb_device_handle *bmfeb,
                      int board_id,
                      int command_id,
                      int argc,
                      uint32_t *argv);

int get_command_answer( libusb_device_handle *bmfeb,
                        int board_id,
                        int command_id,
                        int argc,
                        uint32_t **answer);

enum bmfeb_cmd_id {
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

enum bmfeb_set_dir_par_args {
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

enum bmfeb_read_status_args {
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

#ifdef __cplusplus
}
#endif

#endif