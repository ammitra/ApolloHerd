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

  std::string CMID = aParams.get<std::string>("CM ID   ");
  std::string WAIT = aParams.get<std::string>("wait (s)");

  std::string CMID_WAIT;
  CMID_WAIT = CMID + ' ' + WAIT;

  State lState = kDone;

  if (lController.cmpwrup(CMID_WAIT) == CommandReturn::status::BAD_ARGS)
    throw core::RuntimeError("bad arguments");
  else
    lState = kDone;

  return lState;
}


}   // commands
}   // apolloherd
}   // swatch