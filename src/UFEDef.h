#ifndef UFE_DEF_
#define UFE_DEF_ 1

#define UFE_VENDOR_ID        0x206b
#define CLASS_REQUEST           0x20

enum ufe_request_types {
  UFE_GET_VERSION_REQ = 0x20,
  UFE_LED_OFF_REQ = 0x22,
  UFE_EP2IN_WRAPPUP_REQ = 0x23,
  UFE_EPxIN_RESET_REQ = 0x24
};

enum ufe_error {
  UFE_ERROR_INVALID_CMD_ANSWER = -13,
};

enum ufe_masks {
  UFE_DW_ID_MASK       = 0xF0000000,
  UFE_BOARD_ID_MASK    = 0x0FE00000,
  UFE_CMD_ID_MASK      = 0x001F0000,
  UFE_SUBCMD_ID_MASK   = 0x0000F000,
  UFE_ARGUMENT_MASK    = 0x0000FFFF,
  UFE_ARG_FR_NUM_MASK  = 0x00000FFF,
  UEF_FRAME_INDEX_MASK = 0x0FFF0000
};

enum ufe_shifts {
  UFE_DW_ID_SHIFT       = 28,
  UFE_BOARD_ID_SHIFT    = 21,
  UFE_CMD_ID_SHIFT      = 16,
  UFE_SUBCMD_ID_SHIFT   = 12,
  UEF_FRAME_INDEX_SHIFT = 16
};

enum ufe_cmd_ids {
  CMD_HEADER_ID  = 0x8,
  CMD_ARG_ID     = 0x9,
  CMD_TRAILER_ID = 0xA
};

#endif
