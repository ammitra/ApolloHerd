#include "swatch/apolloherd/ApolloDeviceController.hpp"

namespace swatch {
namespace apolloherd {

ApolloDeviceController::ApolloDeviceController(const std::string& aURI, const std::string& aAddrTable)
{
  ptrSMDevice = new BUTool::ApolloSMDevice(arg);
}

ApolloDeviceController::~ApolloDeviceController() 
{
  if (NULL != ptrSMDevice) {
      delete ptrSMDevice;
  }
}

// I think for these, it would be better just to pass vector<string>, vector<uint64_t>
// since that is what ApolloSMDevice takes, then let EvaluateCommand parse it. Would be much simpler

int ApolloDeviceController::dev_cmd(std::string command_args)
{
  // split command_args into constituent parts
  // https://stackoverflow.com/a/20114104
  std::istringstream iss(command_args);
  std::vector<std::string> CommandArgs;
  std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(CommandArgs));
  return ptrSMDevice->EvaluateCommand(CommandArgs);
}

int ApolloDeviceController::read(std::string args)
{
  std::istringstream iss(args);
  std::vector<std::string> ReadArgs {"read"};
  std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(ReadArgs));
  return ptrSMDevice->EvaluateCommand(ReadArgs);
}

int ApolloDeviceController::svfplayer(std::string SVF_XVC)
{
  std::istringstream iss(SVF_XVC);
  std::vector<std::string> SVFPlayerArgs {"svfplayer"};
  std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(SVFPlayerArgs));
  return ptrSMDevice->EvaluateCommand(SVFPlayerArgs);    
}

int ApolloDeviceController::cmpwrup(std::string pwrup_args)
{
  std::istringstream iss(pwrup_args);
  std::vector<std::string> CMPWRUPArgs {"cmpwrup"};
  std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(CMPWRUPArgs));
  return ptrSMDevice->EvaluateCommand(CMPWRUPArgs);    
}

int ApolloDeviceController::cmpwrdown(std::string pwrdown_args)
{
  std::istringstream iss(pwrdown_args);
  std::vector<std::string> CMPWRDOWNArgs {"cmpwrdown"};
  std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(CMPWRDOWNArgs));
  return ptrSMDevice->EvaluateCommand(CMPWRDOWNArgs);
}

}   // apolloherd
}   // swatch