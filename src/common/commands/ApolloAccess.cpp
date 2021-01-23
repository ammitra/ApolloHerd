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

  State lState = kDone;

  if (lController.dev_cmd(aParams.get<std::string>("command_and_arguments")) == CommandReturn::status::NOT_FOUND)
    throw core::RuntimeError("command not found");
  else if (lController.dev_cmd(aParams.get<std::string>("command_and_arguments")) == CommandReturn::status::BAD_ARGS)
    throw core::RuntimeError("bad arguments");
  else
    lState = kDone;

  return lState;
}

}   // commands
}   // apolloherd
}   // swatch