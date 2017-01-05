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
#include <string.h>

#include "libufe.h"
#include "libufe-tools.h"

int board_id;

int ping(libusb_device_handle *dev_handle) {
  return ufe_enable_led(dev_handle, 1);
}


void print_usage(char *argv) {
  fprintf(stderr, "\nUsage: %s [OPTION] ARG \n\n", argv);
  fprintf(stderr, "    -b / --board-id     <int dec/hex>   ( Board Id )  [ required ]\n");
}

int main (int argc, char **argv) {

  int board_id_arg = get_arg_val('b', "board-id"  , argc, argv);

if (board_id_arg == 0) {
    print_usage(argv[0]);
    return 1;
  }

  board_id = arg_as_int(argv[board_id_arg]);

  int status = ufe_on_all_boards_do(&ping);

  return (status!=0)? 1 : 0;
}