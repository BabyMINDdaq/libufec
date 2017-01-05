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
 *  \returns True if this is a UFE device, else false.
 */
bool is_ufe(libusb_device *dev, int dummy_arg);


/** \brief Checks the type of the device.
 *  \param dev: A device handle.
 *  \returns True if this device is a Baby MIND FEB, else false.
 */
bool is_bm_feb(libusb_device *dev, int dummy_arg);


/** \brief Checks the type of the device.
 *  \param dev: A device handle.
 *  \param command_id: Command identifier (unique number).
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

/** The value to be given to the \param sub_cmd_id of to functions send_command_req and get_command_answer
 if the corresponding command has not Subcommand identifier */
#define NO_SUB_CMD_ID -1


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


int ufe_get_verbose();

int ufe_debug_print(const char *fmt, ...);

int ufe_info_print(const char *fmt, ...);

int ufe_warning_print(const char *fmt, ...);

int ufe_error_print(const char *fmt, ...);


#define CLASS_REQUEST        0x20

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


struct crc_context {
  uint32_t  polynomial_;
  uint8_t   size_;
  uint32_t  initRemainder_;
  uint32_t  finalXor_;
  bool      reflectDin_;
  bool      reflectCRC_;
  uint32_t  mask_;
  uint32_t *table_;
};
typedef struct crc_context crc_context;


enum crc_types {
  CRC_CCITT_11021,
  CRC_16_18005,
  CRC_21_21BF1F,
  CRC_16_1A2EB,
  CRC_32_104C11DB7
};


void ufe_crc_init( crc_context *this_crc,
                   uint32_t x_polynomial,
                   uint8_t  x_size,
                   uint32_t x_initRemainder,
                   uint32_t x_finalXor,
                   bool x_reflectDin,
                   bool x_reflectCRC);

uint32_t crc( crc_context *this_crc,
              uint8_t *message,
              ssize_t length);


/** 16-bits CRC optimized for HD4 @ 32751-bits = 2046 word16
 * (1A2EB = 0xD175 on https://users.ece.cmu.edu/~koopman/crc/index.html)
 * BABY-MIND Protocol GET/SET Config
 */
#define CRC_16_1A2EB_INIT(crc_ctx) \
ufe_crc_init(crc_ctx, 0xA2EB, 16, 0xFFFF, 0x0000, true, false);


/** 21-bits CRC optimized for HD4 @ 1048554-bits = 32767 word32
 * (21bf1f = 0x10df8f on https://users.ece.cmu.edu/~koopman/crc/index.html)
 *  BABY-MIND Readout TDM beacons
 */
#define CRC_21_21BF1F_INIT(crc_ctx) \
ufe_crc_init(crc_ctx, 0x21BF1F, 21, 0xFFFFFF, 0x000000, true, false);


/** Standard CCITT 16-bits CRC
 * (11021 = 0x8810 on https://users.ece.cmu.edu/~koopman/crc/index.html)
 */
#define CRC_CCITT_11021_INIT(crc_ctx) \
ufe_crc_init(crc_ctx, 0x1021, 16, 0xFFFF, 0x0000, false, false);


/** Standard 16-bits CRC.
 */
#define CRC_16_18005_INIT(crc_ctx) \
ufe_crc_init(crc_ctx, 0x8005, 16, 0x0000, 0x0000, true, true);


/** Standard 32-bits Ethernet CRC
 * (104C11DB7 = 0x82608edb on https://users.ece.cmu.edu/~koopman/crc/index.html)
 */
#define CRC_32_104C11DB7_INIT(crc_ctx) \
ufe_crc_init(crc_ctx, 0x04C11DB7, 32, 0xFFFFFFFF, 0xFFFFFFFF, true, true);


#ifdef __cplusplus
}
#endif

#endif
