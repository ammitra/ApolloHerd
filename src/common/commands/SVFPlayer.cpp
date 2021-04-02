/* 
  ApolloSMDevice::svfplayer calls ApolloSM::svfplayer
  https://github.com/apollo-lhc/ApolloSM_plugin/blob/develop/src/ApolloSM/ApolloSM_svf.cc#L4
*/

#include "swatch/apolloherd/commands/SVFPlayer.hpp"
#include "swatch/apolloherd/ApolloDevice.hpp"
#include <BUTool/CommandReturn.hh>

namespace swatch {
namespace apolloherd {
namespace commands{

SVFPlayer::SVFPlayer(const std::string& aId, action::ActionableObject& aActionable)
  : Command(aId, aActionable, std::string("svfplayer"))
  {
    registerParameter<std::string>("SVF file  ", " ");
    registerParameter<std::string>("XVC device", " ");
  }

SVFPlayer::~SVFPlayer()
{
}

action::Command::State SVFPlayer::code(const core::ParameterSet& aParams)
{
  ApolloDeviceController& lController = getActionable<ApolloDevice>().getController();

  std::vector<std::string> svfplayer_args {"svfplayer"};
  svfplayer_args.push_back(aParams.get<std::string>("SVF file  "));
  svfplayer_args.push_back(aParams.get<std::string>("XVC device"));

  State lState;

  // perform command
  CommandReturn::status result = lController.EvaluateCommand(svfplayer_args);

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
      //setResult( result from BUTextIO stringstream )
      break;
  }

  return lState;
}

}   // commands
}   // apolloherd
}   // swatch