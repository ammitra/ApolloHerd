#ifndef __SWATCH_APOLLOHERD_APOLLOCMFPGA_HPP__
#define __SWATCH_APOLLOHERD_APOLLOCMFPGA_HPP__

#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>

#include "emp/swatch/EMPDevice.hpp"
#include <ApolloSM_device/ApolloSM_device.hh>

namespace swatch {
namespace apolloherd {

class ApolloCMFPGA : public emp::swatch::EMPDevice {
public:
  ApolloCMFPGA(const swatch::core::AbstractStub& aStub);
  virtual ~ApolloCMFPGA();
  // use to call any relevant CM commands
  int ApolloAccess(std::string command_args);
  void AddStream(Level::level level, std::ostream* os);
  FPGA getFPGA();
private:
  void retrieveMetricValues() override; // implement but don't define

  // determine which FPGA this class represents
  // might need to change order depending on which FPGA has which CM ID
  enum class FPGA {
    KINTEX = 0,
    VIRTEX = 1
  };

  FPGA mFPGA;

  // for calling svfplayer, other CM control functions
  BUTool::ApolloSMDevice *SMDevice;
  std::vector<std::string> arg {"/opt/address_tables/connections.xml"};

};

} // apolloherd
} // swatch

#endif //__SWATCH_APOLLOHERD_APOLLOCMFPGA_HPP__