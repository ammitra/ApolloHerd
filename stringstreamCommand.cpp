/*
    This is meant as an illustration for how ApolloSMDevice command calls should look when using the new BUTextIO functionality
    Specifically, this example is for the Read command, which should return the values at each address being read

    ** The current code should be rewritten to adhere to this format after the BUTextIO functionality is done **
*/

#include "swatch/apolloherd/commands/Read.hpp"
#include "swatch/apolloherd/ApolloDevice.hpp"
#include <BUTool/CommandReturn.hh>

namespace swatch {
namespace apolloherd {
namespace commands {

Read::Read(const::std::string& aId, action::ActionableObject& aActionable)
  : Command(aId, aActionable, std::string())
  {
    registerParameter<std::string>("address", " ");
  }

Read::~Read()
{
}

action::Command::State Read::code(const core::ParameterSet& aParams)
{
  // get the ApolloSMDevice instance - we access ApolloSMDevice fns thru this
  ApolloDeviceController& lController = getActionable<ApolloDevice>().getController();

  // initalize a return value object
  State lState;

  // get the address(es) to read from the passed parameters
  std::string addr = aParams.get<std::string>("address");

  // initalize a stringstream to capture Read command result
  std::ostringstream oss; 

  // add this stream (via ApolloSMDevice functionality inherited from BUTextIO)
  lController.AddOutputStream(Level::INFO, &oss);

  // perform the Read command, get result
  // the read command will automatically write the output to the ostringstream
  lState = (lController.read(addr) == CommandReturn::status::BAD_ARGS) ? (kError) : (kDone);
  // core::RuntimeError("bad arguments") ????????

  // send the command result to the herd control console
  setResult(oss.str());

  return lState;
}

}   // commands
}   // apolloherd
}   // swatch