/*
  ApolloSMDevice::ReadOffset calls
  BUTool::RegisterHelper::ReadOffset, which calls
  BUTool::RegisterHelper::ReadWithOffsetHelper, which calls
  IPBusRegHelper::RegReadRegister, which calls 
  IPBusIO::RegReadRegister

  again, BUTextIO functionality will be needed
*/

#include "swatch/apolloherd/commands/ReadOffset.hpp"
#include "swatch/apolloherd/ApolloDevice.hpp"
#include <BUTool/CommandReturn.hh>

namespace swatch {
namespace apolloherd {
namespace commands {

ReadOffset::ReadOffset(const std::string& aId, action::ActionableObject& aActionable)
  : Command(aId, aActionable, std::string("readoffset"))
  {
    registerParameter<std::string>("address", " ");
    registerParameter<std::string>("offset", " ");
    registerParameter<std::string>("count", " ");
  }

ReadOffset::~ReadOffset()
{
}

action::Command::State ReadOffset::code(const core::ParameterSet& aParams)
{
  ApolloDeviceController& lController = getActionable<ApolloDevice>().getController();

  State lState;

  std::vector<std::string> readoffset_args {"readoffset"};
  readoffset_args.push_back(aParams.get<std::string>("address"));
  readoffset_args.push_back(aParams.get<std::string>("offset"));
  readoffset_args.push_back(aParams.get<std::string>("count"));

  CommandReturn::status result = lController.EvaluateCommand(readoffset_args);

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
