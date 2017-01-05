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

#define SIZE_CONFBUFF  36

extern int board_id;
extern uint32_t conf_data_back[SIZE_CONFBUFF];

int get_config_fpga(libusb_device_handle *dev_handle) {
  int status = ufe_get_config(dev_handle, board_id, 3, conf_data_back);
  if (status < 0)
    return 1;

  int i;
  for(i=0;i<SIZE_CONFBUFF;++i) {
    printf("0x%x\n", conf_data_back[i]);
  }

  return 0;
}

int get_config_asics(libusb_device_handle *dev_handle) {
  int status = 0;
  int device_id;
  for (device_id=0; device_id<3; ++device_id) {
    status = ufe_get_config(dev_handle, board_id, device_id, conf_data_back);
    if (status < 0)
      return 1;

    int i;
    for(i=0;i<SIZE_CONFBUFF;++i) {
      printf("0x%x\n", conf_data_back[i]);
    }
  }

  return 0;
}

int get_config_all(libusb_device_handle *dev_handle) {
  int status = get_config_asics(dev_handle);
  if (status != 0)
    return 1;

  status = get_config_fpga(dev_handle);
  return status;
}


void print_usage(char *argv) {
  fprintf(stderr, "\nUsage: %s [OPTIONS] \n\n", argv);
  fprintf(stderr, "    -b / --board-id      <int dec/hex>   ( Board Id )                             [ required ]\n");
  fprintf(stderr, "    -a / --asics                         ( Configure the 3 asics )                [ optional OR f/d ]\n");
  fprintf(stderr, "    -f / --fpga                          ( Configure the fpga )                   [ optional OR a/d ]\n");
  fprintf(stderr, "    -d / --all-devices                   ( Configure all devices )                [ optional OR a/f ]\n");
}


int main (int argc, char **argv) {

  int board_id_arg  = get_arg_val('b', "board-id"    , argc, argv);
  int asics_arg         = get_arg('a', "asics"       , argc, argv);
  int fpga_arg          = get_arg('f', "fpga"        , argc, argv);
  int all_devices_arg   = get_arg('d', "all-devices" , argc, argv);

  if (board_id_arg == 0) {
    print_usage(argv[0]);
    return 1;
  }

  if (asics_arg == 0 && fpga_arg == 0 && all_devices_arg == 0) {
    print_usage(argv[0]);
    return 1;
  }

  board_id  = arg_as_int(argv[board_id_arg]);
  int status = 0;

  ufe_context *ctx = NULL;
  ufe_default_context(&ctx);
  ctx->verbose_ = 3;

  if ( all_devices_arg != 0 ||
       (fpga_arg != 0 && asics_arg != 0) ) {
    status = ufe_on_board_do(board_id, &get_config_all);
    if (status!=0)
      return 1;

  } else if (fpga_arg != 0) {
    status = ufe_on_board_do(board_id, &get_config_fpga);
    if (status!=0)
      return 1;
  } else {
    status = ufe_on_board_do(board_id, &get_config_asics);
    if (status!=0)
      return 1;
  }

  return 0;
}


