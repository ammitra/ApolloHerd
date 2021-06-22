#ifndef __SWATCH_APOLLOHERD_APOLLODEVICECONTROLLER_HPP__
#define __SWATCH_APOLLOHERD_APOLLODEVICECONTROLLER_HPP__

#include <string>
#include <vector>
#include <algorithm>    // std::copy
#include <iterator>     // std::istream_iterator, std::back_inserter
#include <sstream>      // std::istringstream

#include "emp/swatch/EMPDevice.hpp"

#include <ApolloSM_device/ApolloSM_device.hh>

namespace swatch {
namespace apolloherd {

class ApolloDeviceController : public emp::swatch::EMPDevice {
public:

  ApolloDeviceController(const std::string& aURI, const std::string& aAddrTable, const swatch::core::AbstractStub& aStub);

  ~ApolloDeviceController();

  int ApolloAccess(std::string command_args);

  int Program()

  // access ApolloSMDevice's AddOutputStream() method, inherited from BUTextIO via CommandListBase
  void AddStream(Level::level level, std::ostream* os);

private:
  BUTool::ApolloSMDevice *ptrSMDevice;
  std::vector<std::string> arg {"/opt/address_tables/connections.xml"};
};

}   // apolloherd
}   // swatch

#endif