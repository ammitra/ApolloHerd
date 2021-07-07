#include "swatch/apolloherd/commands/PowerDown.hpp"

namespace swatch {
namespace apolloherd {
namespace commands {

PowerDown::PowerDown(const std::string& aId, action::ActionableObject& aActionable) :
  Command(aId, aActionable, std::string())
{
  registerParameter<std::string>("wait (s)", "1");
}

PowerDown::~PowerDown()
{
}

// See PowerUp for all the things that currently need to change with ApolloSMDevice
action::Command::State PowerDown::code(const core::ParameterSet& aParams)
{
  ApolloCMFPGA& ApolloCM = getActionable<ApolloCMFPGA>();

  std::ostringstream oss;
  ApolloCM.AddStream(Level::INFO, &oss);

  std::string CMID;
  switch (ApolloCM.getFPGA()) {
    case FPGA::KINTEX:
      CMID = "1";
      break;
    case FPGA::VIRTEX:
      CMID = "2";
      break;
  }

  setProgress(0.5, "Powering down CM_" + CMID);
  std::string wait = aParams.get<std::string>("wait (s)");
  std::string command_and_args = std::string("cmpwrdown") + " " + CMID + " " + wait;
  int result = ApolloCM.ApolloAccess(command_and_args);
  if (result == CommandReturn::status::BAD_ARGS)
    throw core::RuntimeError("bad arguments");

  // return cmpwrdown result to Shep
  setResult(oss.str());

  return State::kDone;
}

} // commands
} // apolloherd
} // swatch