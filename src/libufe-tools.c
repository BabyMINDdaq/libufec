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
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include"libufe-tools.h"

int arg_as_int(const char *arg) {
  int my_arg;
  if (arg[0] == '0' && arg[1] == 'x')
    my_arg = strtol(arg+2, NULL, 16);
  else
    my_arg = atoi(arg);

  return my_arg;
}


int get_arg_short(const char arg_name, int argc, char **argv) {
  int i;
  for (i = 1; i < argc; i++)
    if (argv[i][0] == '-')
      if ( arg_name == argv[i][1] && argv[i][2] == '\0' && argc > i) {
        return i;
      }

  return 0;
}

int get_arg_long(const char* arg_name, int argc, char **argv) {
  int i;
  for (i = 1; i < argc; i++)
    if (argv[i][0] == '-' && argv[i][1] == '-')
      if ( strcmp(argv[i]+2, arg_name) == 0 && argc > i) {
        return i;
      }

  return 0;
}

int get_arg(const char arg_short, const char* arg_long, int argc, char **argv) {
  int x_arg = get_arg_short(arg_short, argc, argv);
  if (x_arg == 0)
    x_arg = get_arg_long(arg_long, argc, argv);

  return x_arg;
}

int get_arg_val(const char arg_short, const char* arg_long, int argc, char **argv) {
  int x_arg = get_arg(arg_short, arg_long, argc, argv);
  if (x_arg != 0 && x_arg+1 < argc)
    return x_arg + 1;

  return 0;
}

int on_device_do(ufe_cond_func cond_func, ufe_user_func user_func, int arg) {

  libusb_device_handle *dev_handle; //a device handle
  ufe_context *ctx = NULL; //a libusb session
  int status; //for return values

  status = ufe_init(&ctx); //initialize the library for the session we just declared
  if(status < 0) {
    ufe_error_print("init Error. %i", status); //there was an error
    return 1;
  }

  libusb_device **febs;
//   size_t n_febs = ufe_get_device_list(ctx->usb_ctx_, &febs);
  size_t n_febs = ufe_get_custom_device_list(ctx->usb_ctx_, cond_func, &febs, arg);

  if (n_febs == 0) {
    ufe_error_print("no UFE board found.");
    ufe_exit(ctx);
    return 1;
  }

  ufe_info_print("UFE boards found: %zu", n_febs);

  int i;
  for (i=0; i<n_febs; ++i) {
    status = libusb_open(febs[i], &dev_handle);
    if(dev_handle == NULL) {
      ufe_error_print("cannot open device.");
      return 1;
    }

    ufe_info_print("device opened.");
    status = (*user_func)(dev_handle);
    libusb_close(dev_handle);
    ufe_info_print("device closed.");

    if (status != 0) {
      libusb_free_device_list(febs, 1);
      ufe_exit(ctx);
      return status;
    }
  }

  libusb_free_device_list(febs, 1); //free/unref the selected devices.
  ufe_exit(ctx);
  return status;
}

int on_board_do(int board_id, ufe_user_func user_func) {
  return on_device_do(&is_bm_feb_with_id, user_func, board_id);
}

int on_all_boards_do(ufe_user_func user_func) {
  int dummy_arg=0;
  return on_device_do(&is_bm_feb, user_func, dummy_arg);
}

int ufe_open_fifo() {
  int i=0, status = 0;
  while ( (status = mkfifo(FIFO_PATH, 0666)) == -1) {
    unlink(FIFO_PATH);
    if (++i > 5) {
      ufe_error_print("Cannot make /etc/ufe_fifo.");
      return -1;
    }
  }

  int fifo;
  i = 0;
  while ((fifo = open(FIFO_PATH, O_WRONLY)) == -1) {
    usleep(1000);
    if (++i > 5) {
      ufe_error_print("Cannot open /etc/ufe_fifo for writting.");
      return fifo;
    }
  }

  uint32_t handshake = 0xabcdef;
  ssize_t actual = write(fifo, &handshake, sizeof(uint32_t));
  if (actual != sizeof(uint32_t)) {
    ufe_error_print("when writting handshake data to FIFO (%zd).", actual);
    return -1;
  }

  ufe_debug_print("handshake sent.");

  return fifo;
}

int ufe_close_fifo(int fifo) {
  usleep(1000);
//   sleep(1);
  uint32_t goodbye = 0xfedcba;
  ssize_t actual = write(fifo, &goodbye, sizeof(uint32_t));
  if (actual != sizeof(uint32_t)) {
    ufe_error_print("when writting goodbye data to FIFO (%zd).", actual);
    return -1;
  }

  usleep(1000);
  unlink(FIFO_PATH);
  return 0;
}

