/* 
  ApolloSMDevice::ReadString calls
  IPBusRegHelper::ReadString

  In order to return the command result to the HERD control console, need BUTextIO functionality
*/

#include "swatch/apolloherd/commands/ReadString.hpp"
#include "swatch/apolloherd/ApolloDevice.hpp"
#include <BUTool/CommandReturn.hh>

namespace swatch {
namespace apolloherd {
namespace commands {

ReadString::ReadString(const std::string& aId, action::ActionableObject& aActionable)
  : Command(aId, aActionable, std::string("readstring"))
  {
    registerParameter<std::string>("register", " ");
  }

ReadString::~ReadString()
{
}

action::Command::State ReadString::code(const core::ParameterSet& aParams)
{
  ApolloDeviceController& lController = getActionable<ApolloDevice>().getController();

  State lState;

  std::vector<std::string> readstring_args {"readstring"};
  readoffset_args.push_back(aParams.get<std::string>("register"));

  CommandReturn::status result = lController.EvaluateCommand(readstring_args);

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

}
}
}