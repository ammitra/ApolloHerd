/* 
  ApolloSMDevice::Read calls 
  BUTool::RegisterHelper::Read, which calls
  BUTool::RegisterHelper::ReadWithOffsetHelper, which calls
  IPBusRegHelper::RegReadRegister, which calls 
  IPBusIO::RegReadRegister

  In order to return the command result to the HERD control console, need BUTextIO functionality
*/

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
    registerParameter<std::string>("count", " ");
    registerParameter<std::string>("flags", " ");
  }

Read::~Read()
{
}

action::Command::State Read::code(const core::ParameterSet& aParams)
{
  ApolloDeviceController& lController = getActionable<ApolloDevice>().getController();

  State lState;

  // gather the command and arguments in order 
  std::vector<std::string> read_args {"read"};
  read_args.push_back(aParams.get<std::string>("address"));
  read_args.push_back(aParams.get<std::string>("count"));
  read_args.push_back(aParams.get<std::string>("flags"));

  // perform command
  CommandReturn::status result = lController.EvaluateCommand(read_args);

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
      /*
          here we send the values at the register/address back to the HERD control console
      */ 
      //setResult( result from BUTextIO stringstream )
      break;
  }

  return lState;
}

}   // commands
}   // apolloherd
}   // swatch