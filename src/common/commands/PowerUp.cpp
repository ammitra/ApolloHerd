#include "swatch/apolloherd/commands/PowerUp.hpp"

namespace swatch {
namespace apolloherd {
namespace commands {

PowerUp::PowerUp(const std::string& aId, action::ActionableObject& aActionable) :
  Command(aId, aActionable, std::string())
{
  registerParameter<std::string>("wait (s)", "1");
}

PowerUp::~PowerUp()
{
}

action::Command::State PowerUp::code(const core::ParameterSet& aParams) 
{
  // get the ApolloCMFPGA
  ApolloCMFPGA& ApolloCM = getActionable<ApolloCMFPGA>();

  // give stringstream to ApolloSMDevice (WIP)
  /*
      this will require changes to ApolloSMDevice as well as ApolloCM:
        - ApolloSMDevice issues a printf call to let user know if CM powered up or not
        - ApolloSM issues a boolean to let ApolloSMDevice know
        - need to integrate BUTextIO into ApolloSMDevice 
      Therefore, adding a stringstream atm does nothing 
  */
  std::ostringstream oss;
  ApolloCM.AddStream(Level::INFO, &oss);

  // get the CM ID
  std::string CMID;
  switch (ApolloCM.getFPGA()) {
    case FPGA::KINTEX:
      CMID = "1";
      break;
    case FPGA::VIRTEX:
      CMID = "2";
      break;
  }

  // power up CM
  setProgress(0.5, "Powering up CM_" + CMID);
  std::string wait = aParams.get<std::string>("wait (s)");
  std::string command_and_args = std::string("cmpwrup") + " " + CMID + " " + wait;
  int result = ApolloCM.ApolloAccess(command_and_args);
  if (result == CommandReturn::status::BAD_ARGS)
    throw core::RuntimeError("bad arguments");

  // return cmpwrup result to Shep
  setResult(oss.str());

  return State::kDone;
}

} // commands
} // apolloherd
} // swatch