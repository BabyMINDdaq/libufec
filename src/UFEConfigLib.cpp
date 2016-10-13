#include "UFEConfigLib.h"
#include "UFEError.h"

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
