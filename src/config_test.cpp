#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>

#include "libufe.h"

#include "UFEDef.h"

using namespace std;

int main (int argc, char **argv) {

  string file_name("../../config/config-bitarray-asic0.txt");
  if (argc == 2) {
    file_name = string(argv[1]);
  }

  libusb_device_handle *dev_handle; //a device handle
  libusb_context *ctx = NULL; //a libusb session
  int status; //for return values

  status = libusb_init(&ctx); //initialize the library for the session we just declared
  if(status < 0) {
    cout << "Init Error " << status << endl; //there was an error
    return 1;
  }

  libusb_set_debug(ctx, 3); //set verbosity level to 3

  libusb_device **febs;
  size_t n_bmfebs = get_ufe_device_list(ctx, &febs);

  cout << "BM FEBs found: " << n_bmfebs << " \n";

  if (n_bmfebs > 0) {
    status = libusb_open(febs[0], &dev_handle);
    if(dev_handle == NULL) {
      cout << "Cannot open device.\n";
      return 1;
    } else
      cout << "Device Opened.\n";

    libusb_free_device_list(febs, 1); //free the lconf_filet, unref the devices in it

    uint16_t *data = new uint16_t[72];
    uint32_t *data_32 = (uint32_t*) data;

    string line;
    stringstream ss;
    ss << hex;
    ifstream conf_file(file_name);

    if (conf_file) {
      while ( getline (conf_file, line) ) {
        ss << line;
        ss >> *data_32;
        data_32 ++;
      }
      conf_file.close();
    } else
      cout << "Cannot open file " << file_name << endl;

    int board_id = 0, command_id;

    command_id = SET_CONFIG_CMD_ID;
    send_command_req( dev_handle,
                      board_id,
                      command_id,
                      0,
                      72,
                      data);

    usleep(100);
    ep2in_wrappup_req(dev_handle, NULL);
    usleep(100);

    get_command_answer( dev_handle,
                        board_id,
                        command_id,
//                         0,
                        1,
                        &data);

    cout << "STATUS: " << hex << *data << dec << endl;

    libusb_close(dev_handle); //close the device we opened

  }

  libusb_exit(ctx); //needs to be called to end the

  return 0;
}

