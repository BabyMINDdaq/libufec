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

extern int usb_ep;

int main (int argc, char **argv) {

  int e_arg = get_arg_val('e', "end-point", argc, argv);

  if (e_arg == 0) {
    fprintf(stderr, "\nUsage: %s [OPTIONS] \n\n", argv[0]);
    fprintf(stderr, "    -e / --end-point    <int dec/hex> :  USB End Point Id [ required ]\n\n");
    return 1;
  }

  usb_ep = arg_as_int(argv[e_arg]);

  int status = ufe_on_all_boards_do(&usb_reset);
  if (status != 1)
    return 1;

  return 0;
}


