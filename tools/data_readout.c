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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>

#include "libufe.h"
#include "libufe-tools.h"

int board_id, time_s, data_fifo=-1;
uint16_t data_16;
char file_name[30];
uint8_t *data_out;

#define NOT_SET   0xFFFF

void* get_data(void *dev) {
  libusb_device_handle *dev_handle = (libusb_device_handle*) dev;
  FILE *bFile = fopen (file_name, "wb");

  int actual_read=0;
  while (ufe_read_buffer(dev_handle, data_out, &actual_read) == 0) {
    if (actual_read == 0)
      break;

    fwrite(data_out , sizeof(uint8_t), actual_read, bFile);
  }

  fclose (bFile);

  return NULL;
}

void* get_data_to_fifo(void *dev) {
  libusb_device_handle *dev_handle = (libusb_device_handle*) dev;
  int actual_read=0, status;
  while ((status = ufe_read_buffer(dev_handle, data_out, &actual_read)) == 0) {
    if (actual_read == 0)
      break;

    ssize_t actual_wtite = write(data_fifo, data_out, actual_read);
    if (actual_read != actual_wtite) {
      fprintf(stderr, "\n!!! Error writting data to FIFO (%i!=%zd).\n\n", actual_read, actual_wtite);
      break;
    }
  }

  ufe_close_fifo(data_fifo);
  return NULL;
}

int readout(libusb_device_handle *dev_handle) {
  data_16 &= 0xfffe;
  int status = ufe_data_readout(dev_handle, board_id, &data_16);

  void* (*job_ptr) (void*);
  if (data_fifo == -1)
    job_ptr = &get_data;
  else
    job_ptr = &get_data_to_fifo;

  /* Create a second thread which does the readout. */
  pthread_t readout_thread;
  if(pthread_create(&readout_thread, NULL, job_ptr, (void*)dev_handle)) {
    fprintf(stderr, "\n!!! Error creating readout thread.\n\n");
    return 1;
  }

  sleep(time_s);
  data_16 |= 0x1;
  status = ufe_data_readout(dev_handle, board_id, &data_16);

  /* Wait for the readout thread to finish */
  if(pthread_join(readout_thread, NULL)) {
    fprintf(stderr, "Error joining thread\n");
    return 1;
  }

  return status;
}

void print_usage(char *argv) {
  fprintf(stderr, "\nUsage: %s [OPTION] ARG \n\n", argv);
  fprintf(stderr, "    -b / --board-id     <int dec/hex>   ( Board Id )                  [ required ]\n");
  fprintf(stderr, "    -o / --output-file  <string>        ( Name of the output file)    [ optional OR f ]\n");
  fprintf(stderr, "    -f / --fifo-output                  ( Output data to FIFO file)   [ optional OR o ]\n");
  fprintf(stderr, "    -t / --time         <int dec/hex>   ( Duration in seconds )       [ optional / Default 10 s ]\n");
  fprintf(stderr, "    -v / --verbose                      ( Print human readable)       [ optional ]\n");
  fprintf(stderr, "    -p / --param        <int dec/hex>   ( Param bit array value)      [ optional OR s ]\n");
  fprintf(stderr, "    -s / --stdin                        ( Param bit array from stdin) [ optional OR p ]\n");
}

int main (int argc, char **argv) {

  int board_id_arg = get_arg_val('b', "board-id"    , argc, argv);
  int out_file_arg = get_arg_val('o', "output-file" , argc, argv);
  int fifo_arg         = get_arg('f', "fifo-output" , argc, argv);
  int time_arg     = get_arg_val('t', "time"        , argc, argv);
  int param_arg    = get_arg_val('p', "param"       , argc, argv);
  int pipe_arg         = get_arg('s', "stdin"       , argc, argv);
  int v_arg            = get_arg('v', "verbose"     , argc, argv);

  if (board_id_arg == 0) {
    print_usage(argv[0]);
    return 1;
  }

  board_id = arg_as_int(argv[board_id_arg]);

  time_s = 10;
  if (time_arg != 0)
    time_s = arg_as_int(argv[time_arg]);

  if (!( (param_arg == 0)^(pipe_arg == 0) )) {
    print_usage(argv[0]);
    return 1;
  }

  if (!( (fifo_arg == 0)^(out_file_arg == 0) )) {
    print_usage(argv[0]);
    return 1;
  }

  data_16 = NOT_SET;
  if ( param_arg != 0 ) {
    data_16 = arg_as_int(argv[param_arg]);
  }

  if (pipe_arg != 0) {
    char line[11];
    if (fgets(line, 11, stdin) != NULL)
      data_16 = arg_as_int(line);
    else {
      print_usage(argv[0]);
      return 1;
    }
  }

  if (data_16 == NOT_SET ) {
    print_usage(argv[0]);
    return 1;
  }

  if ( v_arg ) {
    printf("\nOn device 0x%x  board %i -> Setting readout params: 0x%x \n", BMFEB_PRODUCT_ID,
                                                                            board_id,
                                                                            data_16);
    ufe_dump_readout_params(data_16);
    printf("\n");
  }

  if (out_file_arg != 0)
    strcat(file_name, argv[out_file_arg]);
  else {
    data_fifo = ufe_open_fifo();
    if (data_fifo == -1)
      return 1;
  }

  ufe_context *ctx = NULL;
  ufe_default_context(&ctx);
//   ctx->readout_buffer_size_ = 1024*64;
//   ctx->verbose_ = 3;
  data_out = (uint8_t*) malloc(ctx->readout_buffer_size_);

  int status = ufe_on_board_do(board_id, &readout);

  free(data_out);
  return (status)? 0 : 1;
}

