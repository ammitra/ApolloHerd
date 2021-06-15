#include "swatch/apolloherd/commands/ApolloAccess.hpp"
#include "swatch/apolloherd/ApolloDevice.hpp"

#include <BUTool/CommandReturn.hh>

namespace swatch {
namespace apolloherd {
namespace commands {

ApolloAccess::ApolloAccess(const std::string& aId, action::ActionableObject& aActionable)
  : Command(aId, aActionable, std::string("apollo command accessed"))
  {
    registerParameter<std::string>("command_and_arguments", " ");
  }

ApolloAccess::~ApolloAccess()
{
}

// implementing method, accessing pointer to ApolloSMDevice instance
action::Command::State ApolloAccess::code(const core::ParameterSet& aParams)
{
  ApolloDeviceController& lController = getActionable<ApolloDevice>().getController();

  // initialize a return value object
  State lState;
  // gather command and arguments in a string to pass to dev_cmd
  std::string params = aParams.get<std::string>("command_and_arguments");
  // initialize a stringstream to capture command result
  std::ostringstream oss;
  // add this stream (via ApolloSMDevice functionality inherited from BUTextIO)
  lController.AddStream(Level::INFO, &oss);
  // perform the command, have output written to oss
  int result = lController.dev_cmd(params);

  // compare the command result
  if (result == CommandReturn::status::NOT_FOUND)
    throw core::RuntimeError("command not found");
  else if (result == CommandReturn::status::BAD_ARGS)
    throw core::RuntimeError("bad arguments");
  else
    lState = kDone;

  // send the command result to the herd control console
  setResult(oss.str());

  return lState;
}

}   // commands
}   // apolloherd
}   // swatch