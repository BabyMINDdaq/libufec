#ifndef UFE_CONFIGFRAME_H
#define UFE_CONFIGFRAME_H 1

#include "UFEConfigLib.h"

class UFEConfigBuffer {
public:
  UFEConfigBuffer() = delete;
  UFEConfigBuffer(int b, int d)
  : board_id_(b), device_id_(d), buffer_(new uint32_t[size_]) {}

  virtual ~UFEConfigBuffer() {delete[] buffer_;}

  uint32_t* get_data() { return buffer_;}
  unsigned int get_size() {return size_;}
  void load_config_from_text(std::string file);

private:
  static const unsigned int size_ = 36;
  int board_id_;
  int device_id_;
  uint32_t* buffer_;
};

class UFEConfigFrame {
public:
  UFEConfigFrame() {};
  UFEConfigFrame(Json::Value c);

  virtual ~UFEConfigFrame() {}

  void load_frame(Json::Value c);

private:

  void get_board_config(const Json::Value &c);
  void get_ASICS_config(const Json::Value &c);
  void get_FPGA_config(const Json::Value &c);

  std::string Name_;
  double Version_;
  int DeviceMemorySize_;
  double MinFpgaVersion_;
  std::vector<HardwareVId> HardwareVIds_;
  std::vector<FirmwareVId> FirmwareVIds_;
  Board Board_;
};

#endif

