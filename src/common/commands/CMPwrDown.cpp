/* 
  ApolloSMDevice functionality:
  https://github.com/apollo-lhc/ApolloSM_plugin/blob/develop/src/ApolloSM_device/ApolloSM_device.cc#L255

  - ApolloSMDevice::CMPowerDown calls ApolloSM::PowerDownCM, which returns bool
*/

#include "swatch/apolloherd/commands/CMPwrDown.hpp"
#include "swatch/apolloherd/ApolloDevice.hpp"
#include <BUTool/CommandReturn.hh>

namespace swatch {
namespace apolloherd {
namespace commands {

CMPwrDown::CMPwrDown(const std::string& aId, action::ActionableObject& aActionable)
  : Command(aId, aActionable, std::string("cmpwrdown"))
  {
    registerParameter<std::string>("CM ID   ", " ");
    registerParameter<std::string>("wait (s)", " ");
  }

CMPwrDown::~CMPwrDown()
{
}

action::Command::State CMPwrDown::code(const core::ParameterSet& aParams)
{
  ApolloDeviceController& lController = getActionable<ApolloDevice>().getController();

  // gather command and arguments 
  std::vector<std::string> cmpwrdown_args {"cmpwrdown"};
  cmpwrdown_args.push_back(aParams.get<std::string>("CM ID   "));
  cmpwrdown_args.push_back(aParams.get<std::string>("wait (s)"));

  State lState;

  // perform command
  CommandReturn::status result = lController.EvaluateCommand(cmpwrdown_args);

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
         at this point, we know the command was successfully executed, but don't know if CM actually pwrd down 
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