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

  // Get a stringstream for general status messages (optional - can be removed)
  std::ostringstream statusMsg;

  // 1) Extract the tarball (using refactored EMP utilities code)
  setProgress(0., "Extracting FW package");
  using emp::swatch::utilities::extractFirmwarePackage;
  const auto lBuildProducts = extractFirmwarePackage(aParams.get<action::File>("packagePath").getPath(),
                                                      ".svf",
                                                      aParams.get<std::string>("addressTable"));
  
  // 2) Get the CM_ID
  std::string CMID;
  switch (ApolloCM.getFPGA()) {
    //  NOTE : might need to change this statement depending on which FPGA has which CMID
    case FPGA::KINTEX:
      CMID = "1";
      break;
    case FPGA::VIRTEX:
      CMID = "2";
      break;
  }

  // 3) Program the FPGA (using svfplayer from ApolloSMDevice -> ApolloSM)
  setProgress(0.2, "Programming CM_" + CMID + " via svfplayer");
  std::string svfplayer("svfplayer");
  // add svfile and xvclabel strings to command and argument string 
  std::string svfFile = lBuildProducts.programmingFile; 
  std::string XVCLabel = aParams.get<std::string>("XVCLabel");
  std::string command_and_args = svfplayer + " " + svfFile + " " + XVCLabel;
  // pass svfplayer command and args to EvaluateCommand via ApolloAccess
  int result = ApolloCM.ApolloAccess(command_and_args);

  // 4) compare command result
  // probably need to add a few more here - will do after i understand how svfplayer works and what it returns
  if (result == CommandReturn::status::BAD_ARGS) {
    throw core::RuntimeError("bad arguments");
  }

  // 5) ensure that the C2C link comes up
  setProgress(0.4, "Initializing C2C link");
  std::ostringstream initialize;
  ApolloCM.AddStream(Level::INFO, &statusMsg);  // add statusMsg stringstream
  initialize << "write CM.CM_" << CMID << ".C2C.INITIALIZE 1";
  result = ApolloCM.ApolloAccess(initialize.str());
  // check if the write call worked - this SHOULD work, keep this here for debugging for now
  if (result == CommandReturn::status::BAD_ARGS) {
    throw core::RuntimeError("bad arguments");
  }
  // update status, clear the stringstream
  setStatusMsg(statusMsg.str());
  statusMsg.str("");

  // sleep 1s
  initialize.str("");
  initialize << "sleep 1";
  ApolloCM.ApolloAccess(initialize.str());
  setStatusMsg(statusMsg.str());
  statusMsg.str("");

  // initialize 0
  initialize.str("");
  initialize << "write CM.CM_" << CMID << ".C2C.INITIALIZE 0";
  result = ApolloCM.ApolloAccess(initialize.str());
  // check if the write call worked - this SHOULD work, keep this here for debugging for now
  if (result == CommandReturn::status::BAD_ARGS) {
    throw core::RuntimeError("bad arguments");
  }
  // update status, clear the stringstream
  setStatusMsg(statusMsg.str());
  statusMsg.str("");

  // sleep 1s
  initialize.str("");
  initialize << "sleep 1";
  ApolloCM.ApolloAccess(initialize.str());
  setStatusMsg(statusMsg.str());
  statusMsg.str("");
  ApolloCM.RemoveStream(Level::INFO, &statusMsg);  // no longer needed.

  // unblock AXI
  setStatusMsg("Unblocking AXI");
  ApolloCM.ApolloAccess("unblockAXI");  // no output from this function


  // 6) Update address table used by EMP commands
  setProgress(0.6, "Updating address table");
  ApolloCM.replaceController("file://" + lBuildProducts.addressTable);

  // 7) Read build metadata and run simple checks
  setProgress(0.8, "Reading build metadata");
  ApolloCM.checkFirmware([&] (const std::string& x) { return this->setStatusMsg(x); });

  return State::kDone;
}

}   // commands
}   // apolloherd
}   // swatch