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

extern ufe_context *ufe_context_handler;

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


#define SIZE_STDIN     20
#define SIZE_CONFBUFF  36

int device_id, board_id;
uint32_t conf_buffer[SIZE_CONFBUFF], conf_data_back[SIZE_CONFBUFF];
char stdin_buff[SIZE_STDIN];
FILE *conf_file;


int load_config(libusb_device_handle *dev_handle, int board, int device, uint32_t *conf_data, int size) {

  int status = ufe_set_config(dev_handle, board, device, conf_data);
  if (status < 0)
    return 1;

  status = ufe_get_config(dev_handle, board, device, conf_data_back);
  if (status < 0)
    return 1;

//   int i;
//   for(i=0;i<size;++i) {
//     if (conf_data[i]!=conf_data_back[i]) printf("* ");
//     printf("0x%x 0x%x \n", conf_data[i], conf_data_back[i]);
//   }

  status = memcmp(conf_data, conf_data_back, size*4);
  if (status != 0) {
    fprintf(stderr, "\n!!! Error: On board %i, device %i - configuration mismatch.\n\n", board_id, device_id);
    return 1;
  }

  return 0;
}

void get_conf_data() {
  int i;
  for (i=0; i<SIZE_CONFBUFF;++i) {
    if ( fgets(stdin_buff, SIZE_STDIN, conf_file) == NULL )
      break;

    conf_buffer[i] = arg_as_int( stdin_buff );
  }
}

int config_fpga(libusb_device_handle *dev_handle) {
  device_id = 3;
  get_conf_data();
  int status = load_config(dev_handle, board_id, device_id, conf_buffer, SIZE_CONFBUFF);
  if (status != 0)
    return 1;

  uint16_t arg = 0x8;
  status = ufe_apply_config(dev_handle, board_id, &arg);
  return status;
}

int config_asics(libusb_device_handle *dev_handle) {
  int status = 0;
  for (device_id=0; device_id<3; ++device_id) {
    get_conf_data();
    status = load_config(dev_handle, board_id, device_id, conf_buffer, SIZE_CONFBUFF);
    if (status != 0)
      return status;
  }

  uint16_t arg = 0x7;
  status = ufe_apply_config(dev_handle, board_id, &arg);
  return status;
}

int config_all(libusb_device_handle *dev_handle) {
  int status = config_asics(dev_handle);
  if (status == 0)
    status = config_fpga(dev_handle);

  return status;
}

int led_on(libusb_device_handle *dev_handle) {

  return ufe_enable_led(dev_handle, 1);
}

int led_off(libusb_device_handle *dev_handle) {

  return ufe_enable_led(dev_handle, 0);
}

int usb_ep =-1;
int usb_reset(libusb_device_handle *dev_handle) {
  return ufe_epxin_reset(dev_handle, usb_ep);
}

uint16_t data_16;
int read_status(libusb_device_handle *dev_handle) {
  int status = ufe_read_status(dev_handle, board_id, &data_16);
//   printf("0x%x\n", data_16);
  return status;
}

int set_param(libusb_device_handle *dev_handle) {
  return ufe_set_direct_param(dev_handle, board_id, &data_16);
}

