#include<iostream>
#include<sstream>
#include<fstream>

#include "UFEConfigFrame.h"
#include "UFEError.h"

using namespace std;

void UFEConfigBuffer::load_config_from_text(std::string file) {
    uint32_t *data_32 = buffer_;

    string line;
    stringstream ss;
    ss << hex;
    ifstream conf_file(file);

    if (conf_file) {
      while ( getline (conf_file, line) ) {
        ss << line;
        ss >> *data_32;
//         cout << hex << *data_32 << dec << endl;
        data_32 ++;
      }
      conf_file.close();
    } else
      cout << "**  Error: cannot open file " << file << endl;
}

UFEConfigFrame::UFEConfigFrame(Json::Value c) {
  load_frame(c);
}

void UFEConfigFrame::load_frame(Json::Value conf) {

  try {
    SET_MEMBER_STRING(this, conf, Name, 1)
    SET_MEMBER_DOUBLE_ASSTRING(this, conf, Version, 1)
    SET_MEMBER_DOUBLE_ASSTRING(this, conf, MinFpgaVersion, 1)
    SET_MEMBER_INT(this, conf, DeviceMemorySize, 1)

    if (conf.isMember("FirmwareVIds"))
      for (auto const& xFV : conf["FirmwareVIds"]) {
//         cout << xFV << endl;
        FirmwareVId fv;
        fv << xFV;
        FirmwareVIds_.push_back(fv);
      }

    if (conf.isMember("HardwareVIds"))
      for (auto const& xHV : conf["HardwareVIds"]) {
//         cout << xFV << endl;
        HardwareVId hv;
        hv << xHV;
        HardwareVIds_.push_back(hv);
      }

    if (conf.isMember("Board")) {
      this->get_board_config(conf["Board"]);
    }

    if (conf.isMember("Children")) {
      for (auto const& children : conf["Children"]) {
        if (children["Name"].asString() == "FPGA")
          this->get_FPGA_config(children);
        else if (children["Name"].asString() == "ASICS")
          this->get_ASICS_config(children);
      }
    }
//   for (auto const& id : conf["Board"].getMemberNames()) {
//     std::cout << id << std::endl;
//   }
  } catch (UFEError &e) {
    cerr << e.GetDescription() << endl;
    cerr << e.GetLocation() << endl;
  }
}

void UFEConfigFrame::get_board_config(const Json::Value &conf) {
  if (conf.isMember("DirectParameters"))
    if (conf["DirectParameters"].isMember("Variables"))
      for (auto const& dp_var : conf["DirectParameters"]["Variables"]) {
//      cout << dp_var << endl;
        Variable v;
        v << dp_var;

        MemoryLayout ml;
        ml << dp_var["MemoryLayout"];
//        cout << ml.Index_ << endl;
        v.MemoryLayout_ = ml;
        Board_.DirectParameters_.Variables_.push_back(v);
      }

  if (conf.isMember("DataReadoutParameters"))
    if (conf["DataReadoutParameters"].isMember("Variables"))
      for (auto const& drp_var : conf["DataReadoutParameters"]["Variables"]) {
//         cout << drp_var << endl;
        Variable v;
        v << drp_var;

        MemoryLayout ml;
        ml << drp_var["MemoryLayout"];
//        cout << ml.Index_ << endl;
        v.MemoryLayout_ = ml;
        Board_.DataReadoutParameters_.Variables_.push_back(v);
      }

  if (conf.isMember("StatusParameters"))
    if (conf["StatusParameters"].isMember("Variables"))
      for (auto const& sp_var : conf["StatusParameters"]["Variables"]) {
//         cout << sp_var << endl;
        Variable v;
        v << sp_var;

        MemoryLayout ml;
        ml << sp_var["MemoryLayout"];
//         cout << ml.Index_ << endl;
        v.MemoryLayout_ = ml;
        Board_.StatusParameters_.Variables_.push_back(v);
      }
}

void UFEConfigFrame::get_ASICS_config(const Json::Value &conf) {
  cout << conf << endl;
}

void UFEConfigFrame::get_FPGA_config(const Json::Value &conf) {
//   cout << conf << endl;
}
