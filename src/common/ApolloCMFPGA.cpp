#include "swatch/apolloherd/ApolloCMFGPA.hpp"
#include "swatch/apolloherd/commands/Program.hpp"
#include "swatch/core/Factory.hpp"

SWATCH_REGISTER_CLASS(swatch::apolloherd::ApolloCMFPGA)

namespace swatch {
namespace apolloherd {

ApolloCMFPGA::ApolloCMFPGA(const swatch::core::AbstractStub& aStub) : EMPDevice(aStub) {
  if (aStub.id == "kintex")
    mFPGA = FPGA::KINTEX;
  else if (aStub.id == "virtex")
    mFPGA = FPGA::VIRTEX;
  else 
    throw std::runtime_error("Invalid CM FPGA ID '" + aStub.id "' (must be either 'kintex' or 'virtex')");
  
  SMDevice = new BUTool::ApolloSMDevice(arg);
  
  registerCommand<commands::Program>("program");
}

ApolloCMFPGA::~ApolloCMFPGA() {
  if (NULL != SMDevice) {
    delete SMDevice;
  }
}

int ApolloCMFGPA::ApolloAccess(std::string command_args) {
  std::istringstream iss(command_args);
  std::vector<std::string> CommandArgs;
  std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(CommandArgs));
  return SMDevice->EvaluateCommand(CommandArgs);
}

void ApolloCMFPGA::AddStream(Level::level level, std::ostream* os) {
  SMDevice->AddOutputStream(level, os);
}

FPGA ApolloCMFPGA::getFPGA() {
  return mFPGA;
}

} // apolloherd
} // swatch
