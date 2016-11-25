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
 *  \created  Nov 2016
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "libufe.h"
#include "libufe-tools.h"

#define SIZE_STDIN     20
#define SIZE_CONFBUFF  36

int device_id, board_id;
uint32_t conf_data[SIZE_CONFBUFF], conf_data_back[SIZE_CONFBUFF];
char stdin_buff[SIZE_STDIN];
FILE *conf_file;

void print_usage(char *argv) {
  fprintf(stderr, "\nUsage: %s [OPTIONS] \n\n", argv);
  fprintf(stderr, "    -b / --board-id      <int dec/hex>   ( Board Id )                             [ required ]\n");
  fprintf(stderr, "    -a / --asics                         ( Configure the 3 asics )                [ optional OR f/d ]\n");
  fprintf(stderr, "    -f / --fpga                          ( Configure the fpga )                   [ optional OR a/d ]\n");
  fprintf(stderr, "    -d / --all-devices                   ( Configure all devices )                [ optional OR a/f ]\n");
  fprintf(stderr, "    -c / --config-file   <string>        ( Text file containing the config bits ) [ optional OR s ]\n");
  fprintf(stderr, "    -s / --stdin                         ( Config bit array from stdin )          [ optional OR c ]\n\n");
}

int load_config(libusb_device_handle *dev_handle, int size) {
  int i;
  for (i=0; i<size;++i) {
    if ( fgets(stdin_buff, SIZE_STDIN, conf_file) == NULL )
      break;

    conf_data[i] = arg_as_int( stdin_buff );
  }

  int status = ufe_set_config(dev_handle, board_id, device_id, conf_data);
  if (status < 0)
    return 1;

  status = ufe_get_config(dev_handle, board_id, device_id, conf_data_back);
  if (status < 0)
    return 1;

//   for(i=0;i<size;++i) {
//     if (conf_data[i]!=conf_data_back[i]) printf("* ");
//     printf("0x%x 0x%x \n", conf_data[i], conf_data_back[i]);
//   }

  status = memcmp(conf_data, conf_data_back, sizeof(conf_data));
  if (status != 0) {
    fprintf(stderr, "\n!!! Error: On board %i, device %i - configuration mismatch.\n\n", board_id, device_id);
    return 1;
  }

  return 0;
}
int config_fpga(libusb_device_handle *dev_handle) {

  device_id = 3;
  int status = load_config(dev_handle, SIZE_CONFBUFF);
  if (status != 0)
    return 1;

  uint16_t arg = 0x8;
  status = ufe_apply_config(dev_handle, board_id, &arg);
  return status;
}

int config_asics(libusb_device_handle *dev_handle) {
  int status = 0;
  for (device_id=0; device_id<3; ++device_id) {
    status = load_config(dev_handle, SIZE_CONFBUFF);
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

int main (int argc, char **argv) {

  int board_id_arg  = get_arg_val('b', "board-id"    , argc, argv);
  int file_arg      = get_arg_val('c', "config-file" , argc, argv);
  int asics_arg         = get_arg('a', "asics"       , argc, argv);
  int fpga_arg          = get_arg('f', "fpga"        , argc, argv);
  int all_devices_arg   = get_arg('d', "all-devices" , argc, argv);
  int pipe_arg          = get_arg('s', "stdin"       , argc, argv);

  if (board_id_arg == 0) {
    print_usage(argv[0]);
    return 1;
  }

  if (asics_arg == 0 && fpga_arg == 0 && all_devices_arg == 0) {
    print_usage(argv[0]);
    return 1;
  }

  if (!( (file_arg == 0) ^ (pipe_arg == 0) )) {
    print_usage(argv[0]);
    return 1;
  }

  board_id  = arg_as_int(argv[board_id_arg]);

  if (pipe_arg != 0)
    conf_file = stdin;
  else {
    conf_file = fopen(argv[file_arg], "r");
    if(!conf_file) {
      fprintf(stderr, "\n!!! Error: can not open file %s \n\n", argv[file_arg]);
      return 1;
    }
  }

  int status = 0;
  if ( all_devices_arg != 0 ||
       (fpga_arg != 0 && asics_arg != 0) ) {
    status = on_board_do(board_id, &config_all);
    if (status!=0)
      return 1;

  } else if (fpga_arg != 0) {
    status = on_board_do(board_id, &config_fpga);
    if (status!=0)
      return 1;
  } else {
    status = on_board_do(board_id, &config_asics);
    if (status!=0)
      return 1;
  }

  return 0;
}


