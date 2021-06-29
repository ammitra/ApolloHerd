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
      CMID = "0";
      break;
    case FPGA::VIRTEX:
      CMID = "1";
      break;
  }

  setProgress(0.5, "Powering down CM" + CMID);
  std::string wait = aParams.get<std::string>("wait (s)");
  int result = ApolloCM.ApolloAccess("cmpwrdown " + CMID + wait);
  if (result == CommandReturn::status::BAD_ARGS)
    throw core::RuntimeError("bad arguments");

  return State::kDone;
}

} // commands
} // apolloherd
} // swatch