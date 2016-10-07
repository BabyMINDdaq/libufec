#include<sstream>

#include "UFEConfigFrame.h"
#include "UFEError.h"

#define THROW_ERROR(location, json_doc, key) { \
  std::string message; \
  if (json_doc.isMember("Name")) { \
    message += "When reading "; \
    message += json_doc["Name"].asString();} \
  message += " Compulsory member >>"; \
  message += #key; \
  message += "<< is missing in the json configuration!"; \
  throw UFEError(message, #location, UFEError::FATAL);}

#define SET_MEMBER_BOOL(obj, json_doc, key, compulsory) \
{ if (json_doc.isMember(#key)) obj->key##_ = json_doc[#key].asBool(); \
  else if (compulsory) THROW_ERROR(SET_MEMBER_DOUBLE_ASSTRING, json_doc, key)}

#define SET_MEMBER_INT(obj, json_doc, key, compulsory) \
{ if (json_doc.isMember(#key)) obj->key##_ = json_doc[#key].asInt(); \
  else if (compulsory) THROW_ERROR(SET_MEMBER_INT, json_doc, key)}

#define SET_MEMBER_DOUBLE(obj, json_doc, key, compulsory) \
{ if (json_doc.isMember(#key)) obj->key##_ = json_doc[#key].asDouble(); \
  else if (compulsory) THROW_ERROR(SET_MEMBER_DOUBLE, json_doc, key) }

#define SET_MEMBER_STRING(obj, json_doc, key, compulsory) \
{ if (json_doc.isMember(#key)) obj->key##_ = json_doc[#key].asString(); \
  else if (compulsory) THROW_ERROR(SET_MEMBER_STRING, json_doc, key) }

#define SET_MEMBER_DOUBLE_ASSTRING(obj, json_doc, key, compulsory) \
{ if (json_doc.isMember(#key)) { \
  std::stringstream ss; ss << json_doc[#key].asString(); ss >> obj->key##_; } \
  else if (compulsory) THROW_ERROR(SET_MEMBER_DOUBLE_ASSTRING, json_doc, key) }

using namespace std;

void HardwareVId::operator << (const Json::Value &fv_json) {
  SET_MEMBER_INT(this, fv_json, Id, 1)
  SET_MEMBER_STRING(this, fv_json, ShortName, 1)
  SET_MEMBER_STRING(this, fv_json, FriendlyName, 1)
}

void FirmwareVId::operator << (const Json::Value &fv_json) {
  SET_MEMBER_INT(this, fv_json, MajorId, 1)
  SET_MEMBER_INT(this, fv_json, MinorId, 1)
  SET_MEMBER_STRING(this, fv_json, ShortName, 1)
  SET_MEMBER_STRING(this, fv_json, FriendlyName, 1)
}

void Variable::operator << (const Json::Value &v_json) {
  string type = v_json["$type"].asString();
  if (type != "UnigeFrontEnd.Config.Variable") {
    stringstream ss;
    ss << v_json << endl;
    ss << "*** This is not a Variable!";
    throw UFEError( ss.str(),
                    "void Variable::operator << (const Json::Value &v_json)" ,
                    UFEError::FATAL);
  }

  SET_MEMBER_INT(this, v_json, Default, 1)
  SET_MEMBER_INT(this, v_json, Min, 1)
  SET_MEMBER_INT(this, v_json, Max, 1)
  SET_MEMBER_INT(this, v_json, BitSize, 1)
  SET_MEMBER_STRING(this, v_json, Name, 1)
  SET_MEMBER_STRING(this, v_json, Type, 1)
}

void MemoryLayout::operator << (const Json::Value &ml_json) {
  SET_MEMBER_INT(this, ml_json, Index, 1)
  SET_MEMBER_INT(this, ml_json, Increment, 0)
  SET_MEMBER_BOOL(this, ml_json, MsbFirst, 0)
  SET_MEMBER_BOOL(this, ml_json, Absolute, 0)
}

UFEConfigFrame::UFEConfigFrame(Json::Value c) {
  load(c);
}

void UFEConfigFrame::load(Json::Value conf) {

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
