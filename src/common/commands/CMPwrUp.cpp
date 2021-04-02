/* 
  ApolloSMDevice functionality:
  https://github.com/apollo-lhc/ApolloSM_plugin/blob/develop/src/ApolloSM_device/ApolloSM_device.cc#L229

  - ApolloSMDevice::CMPowerUp calls ApolloSM::PowerUpCM, which returns bool
*/

#include "swatch/apolloherd/commands/CMPwrUp.hpp"
#include "swatch/apolloherd/ApolloDevice.hpp"
#include <BUTool/CommandReturn.hh>

namespace swatch {
namespace apolloherd {
namespace commands {

CMPwrUp::CMPwrUp(const std::string& aId, action::ActionableObject& aActionable)
  : Command(aId, aActionable, std::string("cmpwrup"))
  {
    registerParameter<std::string>("CM ID   ", " ");
    registerParameter<std::string>("wait (s)", " ");
  }

CMPwrUp::~CMPwrUp()
{
}

action::Command::State CMPwrUp::code(const core::ParameterSet& aParams)
{
  ApolloDeviceController& lController = getActionable<ApolloDevice>().getController();

  // gather command and arguments 
  std::vector<std::string> cmpwrup_args {"cmpwrup"};
  cmpwrup_args.push_back(aParams.get<std::string>("CM ID   "));
  cmpwrup_args.push_back(aParams.get<std::string>("wait (s)"));

  State lState;

  // perform command
  CommandReturn::status result = lController.EvaluateCommand(cmpwrup_args);

  // check the command result, send to HERD control console (BUTextIO functionality)
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
      break;
      /* 
         at this point, we know the command was successfully executed, but don't know if CM actually pwrd up 
         We'd need to capture the printf result from ApolloSM::PowerDownCM, which contains that information
         this functionality would be implemented through BUTextIO, and could be sent to the HERD control console via setResult
      */
      //setResult( result from BUTextIO stringstream )
  }

  return lState;
}


}   // commands
}   // apolloherd
}   // swatch