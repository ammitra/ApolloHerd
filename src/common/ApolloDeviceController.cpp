#include "swatch/apolloherd/ApolloDeviceController.hpp"

/*
  wraps BUTool and ApolloSMDevice commands through direct calls to EvaluateCommand
  https://github.com/BU-Tools/BUTool/blob/3716389c20e8e8708c02eccf47d74222bf290cf8/include/BUTool/CommandList.hh#L171
*/

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

CommandReturn::status ApolloDeviceController::dev_cmd(std::string command_args)
{
  // split command_args into constituent parts
  // https://stackoverflow.com/a/20114104
  // "read SM_INFO.GIT_HASH*" -> "read","SM_INFO.GIT_HASH*"
  std::istringstream iss(command_args);
  std::vector<std::string> CommandArgs;
  std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(CommandArgs));
  return ptrSMDevice->EvaluateCommand(CommandArgs);
}

CommandReturn::status ApolloDeviceController::EvaluateCommand(std::vector<std::string> args)
{
  // command and arguments sent to BUTool EvaluateCommand function and parsed there
  return ptrSMDevice->EvaluateCommand(args);
}

// if we are just using EvaluateCommand, these specific commands are not necessary
// instead, just use the ApolloDeviceController::EvaluateCommand function to access the BUTool functionality directly
/*
CommandReturn::status ApolloDeviceController::read(std::vector<std::string> read_args)
{
  return ptrSMDevice->EvaluateCommand(read_args);
}

CommandReturn::status ApolloDeviceController::svfplayer(std::string SVF_XVC)
{
  std::istringstream iss(SVF_XVC);
  std::vector<std::string> SVFPlayerArgs {"svfplayer"};
  std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(SVFPlayerArgs));
  return ptrSMDevice->EvaluateCommand(SVFPlayerArgs);    
}

CommandReturn::status ApolloDeviceController::cmpwrup(std::string pwrup_args)
{
  std::istringstream iss(pwrup_args);
  std::vector<std::string> CMPWRUPArgs {"cmpwrup"};
  std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(CMPWRUPArgs));
  return ptrSMDevice->EvaluateCommand(CMPWRUPArgs);    
}

CommandReturn::status ApolloDeviceController::cmpwrdown(std::string pwrdown_args)
{
  std::istringstream iss(pwrdown_args);
  std::vector<std::string> CMPWRDOWNArgs {"cmpwrdown"};
  std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(CMPWRDOWNArgs));
  return ptrSMDevice->EvaluateCommand(CMPWRDOWNArgs);
}
*/

}   // apolloherd
}   // swatch