#ifndef __SWATCH_APOLLOHERD_APOLLODEVICECONTROLLER_HPP__
#define __SWATCH_APOLLOHERD_APOLLODEVICECONTROLLER_HPP__

#include <string>
#include <vector>
#include <ApolloSM_device/ApolloSM_device.hh>

#include <algorithm>    // std::copy
#include <iterator>     // std::istream_iterator, std::back_inserter
#include <sstream>      // std::istringstream

namespace swatch {
namespace apolloherd {

class ApolloDeviceController {
public:

  ApolloDeviceController(const std::string& aURI, const std::string& aAddrTable);

  ~ApolloDeviceController();

  int dev_cmd(std::string command_args);

  // access ApolloSMDevice pointer's AddStream() method, which calls AddOutputStream from BUTool::RegisterHelper's BUTextIO pointer
  void AddStream(Level::level level, std::ostream* os);

  int read(std::string args);

  int svfplayer(std::string SVF_XVC);

  int cmpwrup(std::string pwrup_args);

  int cmpwrdown(std::string pwrdown_args);

private:
  BUTool::ApolloSMDevice *ptrSMDevice;
  std::vector<std::string> arg {"/opt/address_tables/connections.xml"};
};

}   // apolloherd
}   // swatch

#endif