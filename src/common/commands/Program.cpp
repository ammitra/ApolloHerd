#include "swatch/apolloherd/commands/Program.hpp"

namespace swatch {
namespace apolloherd {
namespace commands {

using namespace ::swatch;

Program::Program(const std::string& aId, action::ActionableObject& aActionable) :
  Command(aId, aActionable, std::string())
{
  registerParameter<action::File>("packagePath", { "/path/to/package.tgz", "package.tgz"});

  registerParameter<std::string>("XVCLabel","");  // might just be able to hard-code
  
  registerParameter<std::string>("addressTable", "top.xml");
}

Program::~Program() 
{
}

action::Command::State Program::code(const core::ParameterSet& aParams)
{
  // get an ApolloCMFPGA 
  ApolloCMFPGA& ApolloCM = getActionable<ApolloCMFPGA>();

  // Give stringstream to ApolloSMDevice
  std::ostringstream oss;
  ApolloCM.AddStream(Level::INFO, &oss);

  // 1) Extract the tarball (using refactored EMP utilities code)
  setProgress(0., "Extracting FW package");
  using emp::swatch::utilities::extractFirmwarePackage;
  const auto lBuildProducts = extractFirmwarePackage(aParams.get<action::File>("package").getPath(),
                                                      ".svf",
                                                      aParams.get<std::string>("addressTable"));
  
  // 2) Power up the FPGA (maybe this should be its own ApolloCMFGPA::CMPowerUp command??)
  std::string CMID;
  switch (ApolloCM.getFPGA()) {
    //  NOTE : might need to change this statement depending on which FPGA has which CMID
    case FPGA::KINTEX:
      CMID = "0";
      break;
    case FPGA::VIRTEX:
      CMID = "1";
      break;
  }
  setProgress(0.3, "Powering up CM" + CMID);
  // cmpwrup command will return success regardless of whether CM powered up -> need to add BUTextIO to ApolloSMDevice::CMPowerUp
  ApolloCMFPGA.ApolloAccess("cmpwrup "+CMID); // use default wait time (1s)

  // 3) Program the FPGA (using svfplayer from ApolloSMDevice -> ApolloSM)
  SetProgress(0.6, "Programming CM" + CMID + " via svfplayer");
  std::string svfplayer("svfplayer");
  // add svfile and xvclabel strings to command and argument string 
  std::string svfFile = lBuildProducts.programmingFile; 
  std::string XVCLabel = aParams.get<std::string>("XVCLabel");
  std::string command_and_args = svfplayer + " " + svfFile + " " + XVCLabel;
  // pass svfplayer command and args to EvaluateCommand via ApolloAccess
  int result = ApolloCMFPGA.ApolloAccess(command_and_args);

  // 4) compare command result
  // probably need to add a few more here - will do after i understand how svfplayer works and what it returns
  if (result == CommandReturn::status::BAD_ARGS)
    throw core::RuntimeError("bad arguments");

  // 5) Update address table used by EMP commands
  setProgress(0.9, "Updating address table");
  ApolloCMFPGA.replaceController("file://" + lBuildProducts.addressTable);


  // 6) Read build metadata and run simple checks
  //setProgress(0.9, "Reading build metadata");
  //lController.checkFirmware([&] (const std::string& x) { return this->setStatusMsg(x); })


  return State::kDone;
}

}   // commands
}   // apolloherd
}   // swatch