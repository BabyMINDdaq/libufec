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
 *  \file    libufe.h
 *  \brief   File containing an API for slow control and readout of the BabyMINDdaq
 *  detector front-end board.
 *  \author  Yordan Karadzhov
 *  \date    Oct 2016
 */

#ifndef LIBUFE_H
#define LIBUFE_H 1

#include <stdbool.h>
#include <libusb-1.0/libusb.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Baby Unige Front-end Board (UFE) USB vendor Id code. */
#define UFE_VENDOR_ID        0x206b

/** Baby MIND Front-end board USB product Id code. */
#define BMFEB_PRODUCT_ID     0xc00

/** Version Id of the Baby MIND Front-end board firmware supported by this library. */
#define BMFEB_FV             0x30

/** \brief Structure representing a libufec session. */
struct ufe_context {
  /** Verbosity level of the output
   * 0 - Errors only
   * 1 - Errors/Warnings only
   * 2 - Errors/Warnings/Info only
   * 3 - Errors/Warnings/Info/Debug
   * verbose_ < 0 - mute
   */
  int verbose_;

  /** Buffer readout timeout in (in millseconds) */
  unsigned int readout_timeout_;

  /** Size of the readout buffer. Determines the amount of data, transferred in one bulk transfer.*/
  unsigned int readout_buffer_size_;

  /** LIBUSB context */
  libusb_context* usb_ctx_;
};

/** ufe_context type */
typedef struct ufe_context ufe_context;


/** \brief Initialize a default context.
 *  \param ctx: Optional output location for context pointer. Only valid on return code 0.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_default_context(ufe_context **ctx);


/** \brief Initialize libufec. This function must be called before calling any other libufec function.
 *  \param ctx: Optional input/output location for context pointer. Only valid on return code 0.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_init(ufe_context **ctx);


/** \brief Set output message verbosity.
 *  \param ctx: The context to operate on.
 *  \param libusb_level: Verbosity level for libusb.
 *  \param ufe_level: Verbosity level for libufec.
 */
void ufe_set_verbose(ufe_context *ctx, int libusb_level, int ufe_level);


/** \brief Set buffer readout timeout.
 *  \param ctx: The context of the session.
 *  \param t: Buffer readout timeout in (in millseconds)
 */
void ufe_set_readout_timeout(ufe_context *ctx, int t);


/** \brief Gets a list of UFE devices currently attached to the system.
 *  \param ctx: The context of the session.
 *  \param feb_devs: Output location for a list of UFE devices.
 *  \returns The number of devices in the outputted list, or a libusb_error according to error
 *  encountered by the backend.
 */
size_t ufe_get_device_list(libusb_context *ctx, libusb_device ***feb_devs);


/** \brief Gets a list of the Baby MIND UFE devices currently attached to the system.
 *  \param ctx: The context of the session.
 *  \param feb_devs: output location for a list of UFE devices.
 *  \returns The number of devices in the outputted list, or a libusb_error according to error
 *  encountered by the backend.
 */
size_t ufe_get_bm_device_list(libusb_context *ctx, libusb_device ***feb_devs);


/** \brief Opens a UFE device.
 *  \param dev: Input location for a UFE devices.
 *  \param handle: Output location for the device handle.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_open(libusb_device *dev, libusb_device_handle **handle);


/** \brief Closes a UFE device.
 *  \param handle: Input location for the device handle to be closed.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
void ufe_close(libusb_device_handle *handle);


/** \brief Free a device list.
 *  \param list: The list to free.
 *  \param unref_devices: Whether to unref the devices in the list.
 */
void ufe_free_device_list(libusb_device **list, int unref_devices);

/** \brief Gets the session context.
 *  \returns Valid pointer if the context has been Initialized, else NULL.
 */
ufe_context* ufe_get_context();


/** \brief Deinitialize libufec. Should be called after closing all open devices and before your
 *  application terminates.
 *  \param ctx: The context to deinitializ.
 */
void ufe_exit(ufe_context *ctx);


/** \brief Checks if a board is reachable.
 *  \param ufe: A device handle.
 *  \param board_id: Identifier (unique number) of the board, addressed by this command.
 *  \returns True if the board is reachable, else false.
 */
bool ufe_ping(libusb_device_handle *ufe, uint8_t board_id);


/** \brief Gets the version Id codded in 2 bytes, one for the minor version and one for the
 *  major version.
 *  \param ufe: A device handle.
 *  \param data: Output location for the version Id data.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_get_version(libusb_device_handle *ufe, int *data);


/** \brief Gets the buffer size codded in 8 bytes for the DMA buffer size multiplier and the DMA
 *   count for each of the 4 threads.
 *  \param ufe: A device handle.
 *  \param data: Output location for the buffer size data.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_get_buff_size(libusb_device_handle *ufe, uint64_t *data);


/** \brief Turns ON/OFF the green LED on the board.
 *  \param ufe: A device handle.
 *  \param enable: True/False to turn ON/OFF.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_enable_led(libusb_device_handle *ufe, bool enable);


/** \brief The call of this function forces the DMA engine to allow USB EP2 to get the data from
 *  its buffer. This request is mandatory if the host want to get back some UART data where the
 *  number of bytes is lower than the DMA buffer e.g. for a short answer of the protocol.
 *  \param ufe: A device handle.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_ep2in_wrappup(libusb_device_handle *ufe);


/** \brief The call of this function forces the DMA and USB engines to reset the corresponding
 *  communication link (EP1IN or EP2IN). This might be helpful in case of USB stuck.
 *  \param ufe: A device handle.
 *  \param ep_id: 1 / 2 to reset EP1IN / EP2IN.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_epxin_reset(libusb_device_handle *ufe, int ep_id);


/** \brief To be defined.
 *  \param ufe: A device handle.
 *  \param board_id: Identifier (unique number) of the board, addressed by this command.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_idle(libusb_device_handle *ufe, int board_id);


/** \brief Starts the readout until a new command is sent.
 *  \param ufe: A device handle.
 *  \param board_id: Identifier (unique number) of the board, addressed by this command.
 *  \param data: Intput location for the command's argumant data.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_data_readout(libusb_device_handle *ufe, int board_id, uint16_t *data);


/** \brief Gets the firmware version.
 *  \param ufe: A device handle.
 *  \param board_id: Identifier (unique number) of the board, addressed by this command.
 *  \param data: Input/Output location for the command's argumant data.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_firmware_version(libusb_device_handle *ufe, int board_id, int *data);


/** \brief Set values of the direct parameters.
 *  \param ufe: A device handle.
 *  \param board_id: Identifier (unique number) of the board, addressed by this command.
 *  \param data: Intput location for the command's argumant data. In case of an error
 *  this is the output location of the error code.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_set_direct_param(libusb_device_handle *ufe, int board_id, uint16_t *data);


/** \brief Read the status parameters from the board.
 *  \param ufe: A device handle.
 *  \param board_id: Identifier (unique number) of the board, addressed by this command.
 *  \param data: Output location for the command's argumant data.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_read_status(libusb_device_handle *ufe, int board_id, uint16_t *data);


/** \brief Load the configuration.
 *  \param ufe: A device handle.
 *  \param board_id: Identifier (unique number) of the board, addressed by this command.
 *  \param device: Device Id (0/1/2 for the ASICs and 3 for the FPGA)
 *  \param data: Intput location for the command's argumant data. In case of an error
 *  this is the output location of the error code.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_set_config(libusb_device_handle *ufe, int board_id, int device, uint32_t *data);


/** \brief Get the loaded configuration.
 *  \param ufe: A device handle.
 *  \param board_id: Identifier (unique number) of the board, addressed by this command.
 *  \param device: Device Id (0/1/2 for the ASICs and 3 for the FPGA)
 *  \param data: Output location for the command's argumant data.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_get_config(libusb_device_handle *ufe, int board_id, int device, uint32_t *data);


/** \brief Apply the loaded configuration.
 *  \param ufe: A device handle.
 *  \param board_id: Identifier (unique number) of the board, addressed by this command.
 *  \param data: Intput location for the command's argumant data. In case of an error
 *  this is the output location of the error code.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_apply_config(libusb_device_handle *ufe, int board_id, uint16_t *data);


/** \brief Get data from the readout buffer.
 *  \param ufe: A device handle.
 *  \param data: Output location for data to be transferred.
 *  \param actual: Actual size of the transferred data.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_read_buffer(libusb_device_handle *ufe, uint8_t* data, int *actual);


/** List of the Command identifiers for the command requests and command answers */
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

/** List of the UFE_ERROR code returned on failure. */
enum ufe_error {
  UFE_INTERNAL_ERROR           = -13,
  UFE_IO_ERROR                 = -14,
  UFE_INVALID_CMD_ANSWER_ERROR = -15,
  UFE_INVALID_ARG_ERROR        = -16,
  UFE_NOT_FOUND_ERROR          = -17,
  UFE_FIRMWARE_ERROR           = -18
};


/** DATA_READOUT: List of the command argumants */
enum ufe_data_readout_cmd_args {
  DR_START = 0x0,
  DR_STOP  = 0x1,
  DR_GTEN  = 0x40,
  DR_AVE   = 0x80,
  DR_RDEN  = 0x400,
  DR_IGEN  = 0x800
};


/** SET_DIRECT_PARAM: List of the command argumants. */
enum ufe_set_dir_par_cmd_args {
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


/** READ_STATUS: List of the answer argumants. */
enum ufe_read_status_answ_args {
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


/** APPLY_CONFIG: List of the command argumants. */
enum ufe_apply_config_cmd_args {
  AC_CMD_ASIC0    = 0x1,
  AC_CMD_ASIC1    = 0x2,
  AC_CMD_ASIC2    = 0x4,
  AC_CMD_FPGA     = 0x8
};


/** APPLY_CONFIG: List of the answer argumants. */
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


/** \brief Gets the name of the command from the Command Identifier.
 *  \param command_id: Command identifier (unique number).
 *  \returns The name of the command as string.
 */
const char * ufe_get_command_name(int command_id);


/** \brief Prints the status of the board in a human-readable form.
 *  \param status: Board status coded in 16 bits.
 */
void ufe_dump_status(uint16_t status);


/** \brief Prints the direct parameters in a human-readable form.
 *  \param params: Direct parameters coded in 16 bits.
 */
void ufe_dump_direct_params(uint16_t params);


/** \brief Prints the readout parameters in a human-readable form.
 *  \param params: Readout parameters coded in 16 bits.
 */
void ufe_dump_readout_params(uint16_t params);


/** Type of the devices spellection function. */
typedef int (*ufe_user_func)(libusb_device_handle*);


/** Type of the user action function. */
typedef bool (*ufe_cond_func)(libusb_device*, int arg);


/** \brief Prepares a list of usb devices selected according a criteria provided by the user.
 *  \param ctx:  The context to operate on.
 *  \param cond_func: A function to be used for spellection of the devices, to be included in the list.
 *  \param arg: Argumant for the spellection function.
 *  \param feb_devs: Output location for the list of devices.
 *  \returns number of devices found.
 */
size_t ufe_get_custom_device_list( libusb_context *ctx,
                                   ufe_cond_func cond_func,
                                   int arg,
                                   libusb_device ***feb_devs);


/** \brief Executes an action specified by the user over a list of usb devices selected according a criteria
 *  provided by the user. New session is created in the beginning and closed at the end.
 *  \param cond_func: A function to be used for spellection of the devices, to be included in the list.
 *  \param arg: Argumant for the spellection function.
 *  \param user_func: A function defining the user action.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_on_device_do(ufe_cond_func cond_func, int arg, ufe_user_func user_func);


/** \brief Executes an action specified by the user over a particular Baby MIND FEB. New session is created
 *  in the beginning and closed at the end.
 *  \param board_id: Identifier (unique number) of the board, addressed by this command.
 *  \param user_func: A function defining the user action.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_on_board_do(int board_id, ufe_user_func user_func);


/** \brief Executes an action specified by the user over all Baby MIND FEB. New session is created in the
 *  beginning and closed at the end.
 *  \param user_func: A function defining the user action.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_on_all_boards_do(ufe_user_func user_func);


/** \brief Executes an action specified by the user over a list of usb devices selected according a criteria
 *  provided by the user. To be called in an existing (already open) usb session.
 *  \param cond_func: A function to be used for spellection of the devices, to be included in the list.
 *  \param arg: Argumant for the spellection function.
 *  \param user_func: A function defining the user action.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_in_session_on_device_do(ufe_cond_func cond_func, int arg, ufe_user_func user_func);


/** \brief Executes an action specified by the user over a particular Baby MIND FEB. To be called in an existing
 *  (already open) usb session.
 *  \param board_id: Identifier (unique number) of the board, addressed by this command.
 *  \param user_func: A function defining the user action.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_in_session_on_board_do(int board_id, ufe_user_func user_func);


/** \brief Executes an action specified by the user over all Baby MIND FEB. To be called in an existing
 *  (already open) usb session.
 *  \param user_func: A function defining the user action.
 *  \returns 0 on success, or a LIBUSB_ERROR / UFE_ERROR code on failure.
 */
int ufe_in_session_on_all_boards_do(ufe_user_func user_func);

#ifdef __cplusplus
}
#endif

#endif


