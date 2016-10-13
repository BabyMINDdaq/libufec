#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include<json/json.h>
#include <unistd.h>

#include "libufe.h"
#include "UFEConfigFrame.h"

using namespace std;

int main (int argc, char **argv) {

  string config_file("../../config/config-desc.json");
  if (argc == 2) {
    config_file = string(argv[1]);
  }

  std::cout  << "\nLoading file " << config_file << "\n";
  stringstream buffer_in;
  ifstream config_doc(config_file);
  buffer_in << config_doc.rdbuf();

  Json::Value  conf;
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(buffer_in, conf);

  if ( !parsingSuccessful ) {
    // report to the user the failure and their locations in the document.
    std::cout << "Failed to parse configuration\n"
              << reader.getFormattedErrorMessages();
    return 1;
  }

//   for (auto const& id : conf.getMemberNames()) {
//     std::cout << id << std::endl;
//   }

  UFEConfigFrame c;
  c.load(conf);

  return 0;
}

