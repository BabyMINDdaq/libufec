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
#include <signal.h>

#include <zmq.h>

#include "libufe.h"
#include "libufe-core.h"
#include "libufe-tools.h"

void *context, *subscriber;

void intHandler(int dummy) {
  zmq_close (subscriber);
  zmq_ctx_destroy (context);

  exit(0);
}

int main (int argc, char **argv) {

  signal(SIGINT, intHandler);

//   int hist_arg  = get_arg_val('h', "hist-name",  argc, argv);
  int ip_arg    = get_arg_val('i', "ip-address", argc, argv);
  int local_arg = get_arg('l', "localhost",  argc, argv);

  if (ip_arg == 0 && local_arg == 0) {
    fprintf(stderr, "\nUsage: %s [OPTIONS] \n\n", argv[0]);
//     fprintf(stderr, "    -h / --host-name    <string> :  Message publisher name [ optional ]\n\n");
    fprintf(stderr, "    -i / --ip-address   <string> :  Message publisher ip     [ optional ]\n\n");
    fprintf(stderr, "    -l / --localhost   <string> :   Messages from localhost  [ optional ]\n\n");
    return 1;
  }

  char end_point[64] = "tcp://";
  if (ip_arg)
    strcat(end_point, argv[ip_arg]);
  else
    strcat(end_point, "localhost");

  strcat(end_point, ":6110");

//   printf("%s\n", end_point);

  void *context = zmq_ctx_new ();
  void *subscriber = zmq_socket (context, ZMQ_SUB);
  int rc = zmq_connect (subscriber, end_point);
  if (rc != 0)
    return 1;

  char *no_filter = NULL;
  rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, no_filter, 0);
  if (rc != 0)
    return 1;

  while (1) {
    char *message = s_recv(subscriber);
    printf("%s \n", message);
    free(message);
  }

  return 0;
}


