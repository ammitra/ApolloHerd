#include "swatch/apolloherd/commands/Read.hpp"
#include "swatch/apolloherd/ApolloDevice.hpp"
#include <BUTool/CommandReturn.hh>

namespace swatch {
namespace apolloherd {
namespace commands {

Read::Read(const::std::string& aId, action::ActionableObject& aActionable)
  : Command(aId, aActionable, std::string("read"))
  {
    registerParameter<std::string>("address", " ");
  }

Read::~Read()
{
}

action::Command::State Read::code(const core::ParameterSet& aParams)
{
  ApolloDeviceController& lController = getActionable<ApolloDevice>().getController();

  State lState = kDone;

  if (lController.read(aParams.get<std::string>("address")) == CommandReturn::status::BAD_ARGS)
    throw core::RuntimeError("bad arguments");
  else
    lState = kDone;

  return lState;
}

}   // commands
}   // apolloherd
}   // swatch