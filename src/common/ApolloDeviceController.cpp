#include "swatch/apolloherd/ApolloDeviceController.hpp"

namespace swatch {
namespace apolloherd {

ApolloDeviceController::ApolloDeviceController(const std::string& aURI, const std::string& aAddrTable, const swatch::core::AbstractStub& aStub) :
  EMPDevice(aStub)
{
  ptrSMDevice = new BUTool::ApolloSMDevice(arg);
}

ApolloDeviceController::~ApolloDeviceController() 
{
  if (NULL != ptrSMDevice) {
      delete ptrSMDevice;
  }
}

void ApolloDeviceController::AddStream(Level::level level, std::ostream* os) {
  // calls ApolloSMDevice's AddOutputStrea() method inherited from BUTextIO through CommandListBase
  ptrSMDevice->AddOutputStream(level, os);
}

int ApolloDeviceController::ApolloAccess(std::string command_args)
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

int ApolloDeviceController::Program(std::string svfFile_XVCLabel)
{
  std::istringstream iss(svfFile_XVCLabel);
  // initialize vector of strings with command name
  std::vector<std::string> SVFPlayerArgs {std::string("svfplayer")};
  // add the SVF file and XVC label strings to vector
  std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(SVFPlayerArgs));
  // call the appropriate ApolloSMDevice command
  return ptrSMDevice->EvaluateCommand(SVFPlayerArgs);
}


}   // apolloherd
}   // swatch