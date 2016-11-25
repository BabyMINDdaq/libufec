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

typedef int (*ufe_user_func)(libusb_device_handle*);

int on_device_do(ufe_cond_func cond_func, ufe_user_func user_func, int arg);

int on_board_do(int board_id, ufe_user_func user_func);

int on_all_boards_do(ufe_user_func user_func);

#define   FIFO_PATH "/tmp/ufe_fifo"
int ufe_open_fifo();

int ufe_close_fifo(int fifo);


#ifdef __cplusplus
}
#endif

#endif

