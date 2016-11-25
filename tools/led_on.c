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

int led_on(libusb_device_handle *dev_handle) {

  return ufe_enable_led(dev_handle, 1);
}

int led_off(libusb_device_handle *dev_handle) {

  return ufe_enable_led(dev_handle, 0);
}

void print_usage(char *argv) {
  fprintf(stderr, "\nUsage: %s [OPTION] ARG \n\n", argv);
  fprintf(stderr, "    ARG                 < 1 / 0 >       ( Turn On / Off )   [ required ]\n\n");
}

int main (int argc, char **argv) {

  int turn_on = 1, status = 0;

  if (argc != 2) {
    print_usage(argv[0]);
    return 1;
  }

  if (strcmp(argv[1], "0") == 0)
    turn_on = 0;

  if (turn_on)
    status = on_all_boards_do(&led_on);
  else
    status = on_all_boards_do(&led_off);

  return (status!=0)? 1 : 0;
}