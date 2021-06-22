#include "swatch/apolloherd/commands/Program.hpp"
#include "swatch/apolloherd/ApolloDevice.hpp"

#include <BUTool/CommandReturn.hh>

namespace swatch {
namespace apolloherd {
namespace commands {

Program::Program(const std::string& aId, action::ActionableObject& aActionable) :
  Command(aId, aActionable, std::string())
{
  registerParameter<action::File>("packagePath", { "/path/to/package.tgz", "package.tgz"});
  registerParameter<std::string>("XVCLabel","");
  registerParameter<std::string>("addressTable", "top.xml");
}

Program::~Program() 
{
}

action::Command::State Program::code(const core::ParameterSet& aParams)
{
  // get the Controller to be able to access ApolloSMDevice functions
  ApolloDeviceController& lController = getActionable<ApolloDevice>().getController();

  // 1) Extract the tarball (using refactored EMP utilities code)
  setProgress(0., "Extracting FW package");
  using emp::swatch::utilities::extractFirmwarePackage;
  const auto lBuildProducts = extractFirmwarePackage(aParams.get<action::file>("package").getPath(),
                                                      ".svf",
                                                      aParams.get<std::string>("addressTable"));
  
  // 2) Program the FPGA (using SVFplayer from ApolloSM)
  setProgress(0.3, "Programming FPGA via svfplayer");
  std::string svfFile = lBuildProducts.programmingFile;
  std::string XVCLabel = aParams.get<std::string>("XVCLabel");
  std::string args = svfFile + ' ' + XVCLabel;
  // initialize stringstream, add it to ApolloSMDevice
  std::ostringstream oss;
  lController.AddStream(Level::INFO, &oss);
  // run the command, get the result
  int result = lController.Program(args);

  // 3) compare command result
  // probably need to add a few more here - will do after i understand how svfplayer works and what it returns
  if (result == CommandReturn::status::BAD_ARGS)
    throw core::RuntimeError("bad arguments")

  // 4) Update address table used by EMP commands
  setProgress(0.6, "Updating address table");
  lController.replaceController("file://" + lBuildProducts.addressTable);

  // 5) Read build metadata and run simple checks
  setProgress(0.9, "Reading build metadata");
  lController.checkFirmware([&] (const std::string& x) { return this->setStatusMsg(x); });
  
  return State::kDone;
}

}   // commands
}   // apolloherd
}   // swatch