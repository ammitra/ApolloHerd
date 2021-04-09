#include "swatch/apolloherd/commands/Write.hpp"
#include "swatch/apolloherd/ApolloDevice.hpp"
#include <BUTool/CommandReturn.hh>

namespace swatch {
namespace apolloherd {
namespace commands {

Write::Write(const std::string& aId, action::ActionableObject& aActionable)
  : Command(aId, aActionable, std::string("write"))
  {
    registerParameter<std::string>("address", " ");
    registerParameter<std::string>("data", " ");
    registerParameter<std::string>("count", " ");
  }

Write::~Write()
{
}

action::Command::State Write::code(const core::ParameterSet& aParams)
{
  ApolloDeviceController& lController = getActionable<ApolloDevice>().getController();

  State lState;

  std::vector<std::string> write_args {"write"};
  write_args.push_back(aParams.get<std::string>("address"));
  write_args.push_back(aParams.get<std::string>("data"));
  write_args.push_back(aParams.get<std::string>("count"));

  CommandReturn::status result = lController.EvaluateCommand(write_args);

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
