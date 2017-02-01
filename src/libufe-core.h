/*
 * This file is part of BabyMINDdaq software package. This software
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
 * along with BabyMINDdaq. If not, see <http://www.gnu.org/licenses/>.
 */


/**
 *  \file    libufe-core.h
 *  \brief   File containing some core functions used by the API.
 *  \author  Yordan Karadzhov
 *  \date    Oct 2016
 */

#ifndef UFE_CORE_H
#define UFE_CORE_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdarg.h>

#include <libusb-1.0/libusb.h>


/** Default timeout (in millseconds) during the command exchange communication. */
#define UFE_CMD_TIMEOUT 1000


/** \brief Checks the type of the device.
 *  \param dev: A device handle.
 *  \param dummy_arg: Not used.
 *  \returns True if this is a UFE device, else false.
 */
bool is_ufe(libusb_device *dev, int dummy_arg);


/** \brief Checks the type of the device.
 *  \param dev: A device handle.
 *  \param dummy_arg: Not used.
 *  \returns True if this device is a Baby MIND FEB, else false.
 */
bool is_bm_feb(libusb_device *dev, int dummy_arg);


/** \brief Checks the type of the device.
 *  \param dev: A device handle.
 *  \param board_id: Board identifier (unique number), addressed by this command.
 *  \returns True if a Baby MIND FEB with this Id is connected to this device, else false.
 */
bool is_bm_feb_with_id(libusb_device *dev, int board_id);


/** \brief Send a command.
 *  \param ufe: A device handle.
 *  \param board_id: Board identifier (unique number), addressed by this command.
 *  \param command_id: Command identifier (unique number).
 *  \param sub_cmd_id: Subcommand identifier (unique number).
 *  \param argc: Number of argumants.
 *  \param argv: Intput location for the command's argumants data.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_send_command_req( libusb_device_handle *ufe,
                          int board_id,
                          int command_id,
                          int sub_cmd_id,
                          int argc,
                          uint16_t *argv);


/** \brief Get the answer of a command.
 *  \param ufe: A device handle.
 *  \param board_id: Board identifier (unique number), addressed by this command.
 *  \param command_id: Command identifier (unique number).
 *  \param sub_cmd_id: Subcommand identifier (unique number).
 *  \param argc: Number of argumants.
 *  \param argv: Intput location for the command's argumants data.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_get_command_answer( libusb_device_handle *ufe,
                            int board_id,
                            int command_id,
                            int sub_cmd_id,
                            int argc,
                            uint16_t **argv);

/** The value to be given to the parameter sub_cmd_id of the functions send_command_req and get_command_answer
 if the corresponding command has not Subcommand identifier. */
#define NO_SUB_CMD_ID -1

/** . */
#define CLASS_REQUEST 0x20


/** \brief Send data to the device.
 *  \param ufe: A device handle.
 *  \param ep: 1 / 2 for EP1OUT / EP2OUT
 *  \param size: Size of the data to be transferred.
 *  \param data: Intput location for data to be transferred.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_user_set_sync( libusb_device_handle *ufe, int ep, int size, uint8_t *data);


/** \brief Get data from the device.
 *  \param ufe: A device handle.
 *  \param ep: 1 / 2 for EP1IN / EP2IN.
 *  \param size: Size of the data to be transferred.
 *  \param data: Output location for data to be transferred.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_user_get_sync( libusb_device_handle *ufe, int ep, int size, uint8_t *data);


/** \brief Print a degging message.
 *  \param fmt: Formated string (the message).
 *  \returns The number of characters that are printed.
 */
int ufe_debug_print(const char *fmt, ...);


/** \brief Print an info message.
 *  \param fmt: Formated string (the message).
 *  \returns The number of characters that are printed.
 */
int ufe_info_print(const char *fmt, ...);


/** \brief Print a warning message.
 *  \param fmt: Formated string (the message).
 *  \returns The number of characters that are printed.
 */
int ufe_warning_print(const char *fmt, ...);


/** \brief Print an error message.
 *  \param fmt: Formated string (the message).
 *  \returns The number of characters that are printed.
 */
int ufe_error_print(const char *fmt, ...);

#ifdef ZMQ_ENABLE

/** \brief Convert C string to 0MQ string and send to socket
 *  \param socket: Intput location for the socket.
 *  \param message: Intput location for the message.
 */
int s_send(void *socket, char *message);

/** \brief Receive 0MQ string from socket and convert into C string
 *  Caller must free returned string.
 *  \param socket: Intput location for the socket.
 *  \return NULL if the context is being terminated.
 */
char* s_recv(void *socket);

#endif // ZMQ_ENABLE

enum ufe_request_types {
  UFE_GET_VERSION_REQ   = 0x20,
  UFE_GET_BUF_SIZE      = 0x21,
  UFE_LED_OFF_REQ       = 0x22,
  UFE_EP2IN_WRAPPUP_REQ = 0x23,
  UFE_EPxIN_RESET_REQ   = 0x24
};

enum ufe_masks {
  UFE_DW_ID_MASK        = 0xF0000000,
  UFE_BOARD_ID_MASK     = 0x0FE00000,
  UFE_CMD_ID_MASK       = 0x001F0000,
  UFE_SUBCMD_ID_MASK    = 0x0000F000,
  UFE_ARGUMENT_MASK     = 0x0000FFFF,
  UFE_ARG_FR_NUM_MASK   = 0x00000FFF,
  UEF_FRAME_INDEX_MASK  = 0x0FFF0000
};

enum ufe_shifts {
  UFE_DW_ID_SHIFT       = 28,
  UFE_BOARD_ID_SHIFT    = 21,
  UFE_CMD_ID_SHIFT      = 16,
  UFE_SUBCMD_ID_SHIFT   = 12,
  UEF_FRAME_INDEX_SHIFT = 16
};

enum ufe_cmd_ids {
  CMD_HEADER_ID         = 0x8,
  CMD_ARG_ID            = 0x9,
  CMD_TRAILER_ID        = 0xA
};

enum ufe_ep_addr {
  UFE_USB_EP1_OUT       = 0x01,
  UFE_USB_EP1_IN        = 0x81,
  UFE_USB_EP2_OUT       = 0x02,
  UFE_USB_EP2_IN        = 0x82
};

enum ufe_setcongig_validate_codes {
  UFE_SC_VALIDATE_D0    = 0xFA5,
  UFE_SC_VALIDATE_D1    = 0xED8,
  UFE_SC_VALIDATE_D2    = 0x9CD,
  UFE_SC_VALIDATE_D3    = 0x76A
};


/** \brief Structure representing a CRC calculation algorithm. */
struct crc_context {
  /** .. */
  uint32_t  polynomial_;

  /** .. */
  uint8_t   size_;

  /** .. */
  uint32_t  initRemainder_;

  /** .. */
  uint32_t  finalXor_;

  /** .. */
  bool      reflectDin_;

  /** .. */
  bool      reflectCRC_;

  /** .. */
  uint32_t  mask_;

  /** .. */
  uint32_t *table_;
};

/** crc_context type */
typedef struct crc_context crc_context;

/** CRC calculation types. */
enum crc_types {
  /** .. */
  CRC_CCITT_11021,

  /** Standard 16-bits CRC. */
  CRC_16_18005,

  /** 21-bits CRC. BABY-MIND Readout TDM beacons. */
  CRC_21_21BF1F,

  /** 16-bits CRC optimized for HD4. BABY-MIND Protocol GET/SET Config. */
  CRC_16_1A2EB,

  /** Standard 32-bits Ethernet CRC. */
  CRC_32_104C11DB7
};


/** \brief Initialize the Cyclic Redundancy Check (CRC) calculation algorithm. This function
 *  must be called once in the beginning.
 *  \param this_crc: Input/output location for the CRC calculation algorithm context pointer.
 *  \param x_polynomial: .
 *  \param x_size: .
 *  \param x_initRemainder: .
 *  \param x_finalXor: .
 *  \param x_reflectDin: .
 *  \param x_reflectCRC: .
 */
void ufe_crc_init( crc_context *this_crc,
                   uint32_t x_polynomial,
                   uint8_t  x_size,
                   uint32_t x_initRemainder,
                   uint32_t x_finalXor,
                   bool x_reflectDin,
                   bool x_reflectCRC);


/** \brief Calculates the Cyclic Redundancy Check (CRC).
 *  \param this_crc: Input location for the CRC calculation algorithm context pointer.
 *  \param message: Input location for the data block.
 *  \param length: The size of the data block.
 */
uint32_t crc( crc_context *this_crc,
              uint8_t *message,
              ssize_t length);


/** \brief 16-bits CRC optimized for HD4 @ 32751-bits = 2046 word16(1A2EB = 0xD175)
 *  BABY-MIND Protocol GET/SET Config.
 *  \see https://users.ece.cmu.edu/~koopman/crc/index.html
 */
#define CRC_16_1A2EB_INIT(crc_ctx) \
ufe_crc_init(crc_ctx, 0xA2EB, 16, 0xFFFF, 0x0000, true, false);


/** \brief 21-bits CRC optimized for HD4 @ 1048554-bits = 32767 word32 (21bf1f = 0x10df8f)
 *  BABY-MIND Readout TDM beacons.
 *  \see https://users.ece.cmu.edu/~koopman/crc/index.html
 */
#define CRC_21_21BF1F_INIT(crc_ctx) \
ufe_crc_init(crc_ctx, 0x21BF1F, 21, 0xFFFFFF, 0x000000, true, false);


/** \brief Standard CCITT 16-bits CRC (11021 = 0x8810)
 * \see https://users.ece.cmu.edu/~koopman/crc/index.html
 */
#define CRC_CCITT_11021_INIT(crc_ctx) \
ufe_crc_init(crc_ctx, 0x1021, 16, 0xFFFF, 0x0000, false, false);


/** \brief Standard 16-bits CRC.
 */
#define CRC_16_18005_INIT(crc_ctx) \
ufe_crc_init(crc_ctx, 0x8005, 16, 0x0000, 0x0000, true, true);


/** \brief Standard 32-bits Ethernet CRC (104C11DB7 = 0x82608edb)
 *  \see on https://users.ece.cmu.edu/~koopman/crc/index.html
 */
#define CRC_32_104C11DB7_INIT(crc_ctx) \
ufe_crc_init(crc_ctx, 0x04C11DB7, 32, 0xFFFFFFFF, 0xFFFFFFFF, true, true);

#ifdef __cplusplus
}
#endif

#endif
