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

#include "libufe.h"
#include "libufe-tools.h"

uint16_t data_16;
int board_id;

#define NOT_SET 0xFFFF

int set_param(libusb_device_handle *dev_handle) {

  return ufe_set_direct_param(dev_handle, board_id, &data_16);
}

void print_usage(char *argv) {
  fprintf(stderr, "\nUsage: %s [OPTIONS] \n\n", argv);
  fprintf(stderr, "    -b / --board-id     <int dec/hex>   ( Board Id )                  [ required ]\n");
  fprintf(stderr, "    -p / --param        <int dec/hex>   ( Param bit array value)      [ optional OR s ]\n");
  fprintf(stderr, "    -s / --stdin                        ( Param bit array from stdin) [ optional OR p ]\n");
  fprintf(stderr, "    -v / --verbose                      ( Print human readable)       [ optional ]\n\n");
}

int main (int argc, char **argv) {

  int board_id_arg = get_arg_val('b', "board-id"  , argc, argv);
  int param_arg    = get_arg_val('p', "param"     , argc, argv);
  int pipe_arg     = get_arg('s', "stdin"  , argc, argv);
  int v_arg        = get_arg('v', "verbose", argc, argv);

  if (board_id_arg == 0) {
    print_usage(argv[0]);
    return 1;
  }

  if (!( (param_arg == 0) ^ (pipe_arg == 0) )) {
    print_usage(argv[0]);
    return 1;
  }

  board_id = arg_as_int(argv[board_id_arg]);

  data_16 = NOT_SET;
  if ( param_arg != 0 ) {
    data_16 = arg_as_int(argv[param_arg]);
  }

  if (pipe_arg != 0) {
    char line[11];
    if (fgets(line, 11, stdin) != NULL)
      data_16 = arg_as_int(line);

  }

  if (data_16 == NOT_SET ) {
    print_usage(argv[0]);
    return 1;
  }

  if ( v_arg ) {
    printf("\nOn device 0x%x  board %i -> Setting direct params: 0x%x \n",BMFEB_PRODUCT_ID , board_id, data_16);
    dump_direct_params(data_16);
    printf("\n");
  }

//   ufe_context *ctx = NULL;
//   ufe_default_context(&ctx);
//   ctx->verbose_ = 3;

  int status = on_board_do(board_id, &set_param);

  return (status!=0)? 1 : 0;
}