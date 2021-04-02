/*
  ApolloSMDevice::ReadFIFO calls 
  BUTool::RegisterHelper::ReadFIFO, which calls
  RegReadAddressFIFO/RegReadRegisterFIFO
    - idk where these commands are implemented, so this will be a naive implementation
      based on the given parameters

  In order to return the command result to the HERD control console, need BUTextIO functionality
*/

#include "swatch/apolloherd/commands/ReadFIFO.hpp"
#include "swatch/apolloherd/ApolloDevice.hpp"
#include <BUTool/CommandReturn.hh>

namespace swatch {
namespace apolloherd {
namespace commands {

ReadFIFO::ReadFIFO(const std::string& aId, action::ActionableObject& aActionable)
  : Command(aId, aActionable, std::string("readFIFO"))
  {
    registerParameter<std::string>("address", " ");
    registerParameter<std::string>("count", " ");
  }

ReadFIFO::~ReadFIFO()
{
}

action::Command::State ReadFIFO::code(const core::ParameterSet& aParams)
{
  ApolloDeviceController& lController = getActionable<ApolloDevice>().getController();

  State lState;

  std::vector<std::string> readfifo_args {"readFIFO"};
  readfifo_args.push_back(aParams.get<std::string>("address"));
  readfifo_args.push_back(aParams.get<std::string>("count"));

  CommandReturn::status result = lController.EvaluateCommand(readfifo_args);

  switch (result) {
    case 0:
      lState = kError;
      setStatusMsg("Command not found");
      break;
    case 2:
      lState = kError;
      setStatusMsg("Bad arguments");
      break;
    case 1:
      lState = kDone;
      /*
        here we send the values at the register/address back to the HERD control console
      */ 
      //setResult( result from BUTextIO stringstream )
      break;
  }

  return lState;
}

}   // commands
}   // apolloherd
}   // swatch