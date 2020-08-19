#ifndef __SWATCH_DUMMY_APOLLODEVICECONTROLLER_HPP__
#define __SWATCH_DUMMY_APOLLODEVICECONTROLLER_HPP__

#include <string>
#include <vector>
#include <ApolloSM_device/ApolloSM_device.hh>

#include <algorithm>    // std::copy
#include <iterator>     // std::istream_iterator, std::back_inserter
#include <sstream>      // std::istringstream

namespace swatch {
namespace dummy {

class ApolloDeviceController {
    public:
    ApolloDeviceController(const std::string& aURI, const std::string& aAddrTable);
    ~ApolloDeviceController();

   int dev_cmd(std::string command_args)
   {
      // taking the command_args string and separating it 
      // into its constituent command and arguments
      // with help from: https://stackoverflow.com/a/20114104

      std::istringstream iss(command_args);
      std::vector<std::string> CommandArgs;
      std::copy(std::istream_iterator<std::string>(iss),
                std::istream_iterator<std::string>(),
                std::back_inserter(CommandArgs));
      
      return ptrSMDevice->EvaluateCommand(CommandArgs);
   }

   int read(std::string args)
   {
       std::istringstream iss(args);
       std::vector<std::string> ReadArgs {"read"};          // create vector of strings and initialize with read command
       std::copy(std::istream_iterator<std::string>(iss),
                 std::istream_iterator<std::string>(),
                 std::back_inserter(ReadArgs));

       return ptrSMDevice->EvaluateCommand(ReadArgs);
   }

   int svfplayer(std::string SVF_XVC)
   {
       std::istringstream iss(SVF_XVC);
       std::vector<std::string> SVFPlayerArgs {"svfplayer"};
       std::copy(std::istream_iterator<std::string>(iss),
                 std::istream_iterator<std::string>(),
                 std::back_inserter(SVFPlayerArgs));

       return ptrSMDevice->EvaluateCommand(SVFPlayerArgs);
   }

   int cmpwrup(std::string pwrup_args)
   {
       std::istringstream iss(pwrup_args);
       std::vector<std::string> CMPWRUPArgs {"cmpwrup"};
       std::copy(std::istream_iterator<std::string>(iss),
                 std::istream_iterator<std::string>(),
                 std::back_inserter(CMPWRUPArgs));

       return ptrSMDevice->EvaluateCommand(CMPWRUPArgs);
   }

    int cmpwrdown(std::string pwrdown_args)
    {
       std::istringstream iss(pwrdown_args);
       std::vector<std::string> CMPWRDOWNArgs {"cmpwrdown"};
       std::copy(std::istream_iterator<std::string>(iss),
                 std::istream_iterator<std::string>(),
                 std::back_inserter(CMPWRDOWNArgs));

       return ptrSMDevice->EvaluateCommand(CMPWRDOWNArgs);
    }

    private:
    BUTool::ApolloSMDevice *ptrSMDevice;
    std::vector<std::string> arg {"/opt/address_tables/address_apollo.xml"};
};

}   // dummy
}   // swatch

#endif      // __SWATCH_DUMMY_APOLLODEVICECONTROLLER_HPP__
