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


#ifndef LIBUFE_TOOLS_H
#define LIBUFE_TOOLS_H 1

#include <libusb-1.0/libusb.h>

#include "libufe.h"
#include "libufe-core.h"

#ifdef __cplusplus
extern "C" {
#endif

int arg_as_int(const char *arg);

int get_arg_short(const char arg_name, int argc, char **argv);

int get_arg_long(const char* arg_name, int argc, char **argv);

int get_arg(const char arg_short, const char* arg_long, int argc, char **argv);

int get_arg_val(const char arg_short, const char* arg_long, int argc, char **argv);


#define   FIFO_PATH "/tmp/ufe_fifo"
int ufe_open_fifo();

int ufe_close_fifo(int fifo);

// Config
int load_config(libusb_device_handle *dev_handle, int board, int device, uint32_t *conf_data, int size);

int config_fpga(libusb_device_handle *dev_handle);

int config_asics(libusb_device_handle *dev_handle);

int config_all(libusb_device_handle *dev_handle);

// Led ON
int led_on(libusb_device_handle *dev_handle);

int led_off(libusb_device_handle *dev_handle);

// USB reset
int usb_reset(libusb_device_handle *dev_handle);

// Read status
int read_status(libusb_device_handle *dev_handle);

// Set parameter
int set_param(libusb_device_handle *dev_handle);


#ifdef __cplusplus
}
#endif

#endif

