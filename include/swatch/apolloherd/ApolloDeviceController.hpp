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

  // access any command by passing the command and arguments in one string
  CommandReturn::status dev_cmd(std::vector<std::string>);

  // pass a vector of strings directly to EvaluateCommand (similar to above)
  CommandReturn::status EvaluateCommand(std::vector<std::string>);

  /*
  CommandReturn::status read(std::vector<std::string>);

  CommandReturn::status svfplayer(std::vector<std::string>);

  CommandReturn::status cmpwrup(std::vector<std::string>);

  CommandReturn::status cmpwrdown(std::vector<std::string>);
  */

private:
  BUTool::ApolloSMDevice *ptrSMDevice;
  std::vector<std::string> arg {"/opt/address_tables/connections.xml"};
};

}   // apolloherd
}   // swatch

#endif