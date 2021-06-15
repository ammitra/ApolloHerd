#ifndef __SWATCH_APOLLOHERD_APOLLODEVICECONTROLLER_HPP__
#define __SWATCH_APOLLOHERD_APOLLODEVICECONTROLLER_HPP__

#include <string>
#include <vector>
#include <algorithm>    // std::copy
#include <iterator>     // std::istream_iterator, std::back_inserter
#include <sstream>      // std::istringstream

#include <ApolloSM_device/ApolloSM_device.hh>

namespace swatch {
namespace apolloherd {

class ApolloDeviceController {
public:

  ApolloDeviceController(const std::string& aURI, const std::string& aAddrTable);

  ~ApolloDeviceController();

  // access any ApolloSMDevice command
  int dev_cmd(std::string command_args);

  // access ApolloSMDevice's AddOutputStream() method, inherited from BUTextIO via CommandListBase
  void AddStream(Level::level level, std::ostream* os);

private:
  BUTool::ApolloSMDevice *ptrSMDevice;
  std::vector<std::string> arg {"/opt/address_tables/connections.xml"};
};

}   // apolloherd
}   // swatch

#endif