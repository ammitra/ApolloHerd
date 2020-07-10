
#include "swatch/examples/utilities.hpp"


#include <dlfcn.h>
#include <iostream>

#include <boost/filesystem/operations.hpp>

#include <yaml-cpp/yaml.h>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>

#include "swatch/action/Board.hpp"
#include "swatch/action/Command.hpp"
#include "swatch/action/Device.hpp"
#include "swatch/action/StateMachine.hpp"
#include "swatch/core/ParameterSet.hpp"


namespace swatch {


AppConfig parseConfigFile(const std::string& aPath)
{
  YAML::Node topNode = YAML::LoadFile(aPath);
  AppConfig c;

  YAML::Node libNode = topNode["libraries"];
  if (libNode) {
    for (YAML::const_iterator it=libNode.begin(); it != libNode.end(); it++)
      c.libraries.push_back(it->as<std::string>());
  }

  YAML::Node devicesNode = topNode["devices"];
  if (devicesNode) {
    for (YAML::const_iterator it=devicesNode.begin(); it != devicesNode.end(); it++) {
      YAML::Node childNode(*it);
      action::DeviceStub stub(childNode["id"].as<std::string>());
      stub.creator = childNode["class"].as<std::string>();
      stub.uri = childNode["uri"].as<std::string>();
      stub.addressTable = childNode["addressTable"].as<std::string>();

      c.devices.push_back(stub);
    }
  }

  return c;
}


void printCommandSummary(const action::Board& aBoard)
{
  using namespace std;
  using namespace swatch::action;
  using namespace swatch::core;

  cout << "Summary of devices:" << endl;

  for (auto deviceId : aBoard.getDevices())
  {
    const Device& device = aBoard.getDevice(deviceId);
    cout << endl;

    cout << " * " << deviceId << endl;
    cout << "   Type: " << device.getStub().creator << endl;

    cout << "   Commands:" << endl;
    size_t i = 0;
    for(auto commandId : device.getCommands())
    {
      const Command& command = device.getCommand(commandId);
      i++;

      cout << "    " << i << ") " << command.getId() << endl;
      if ( command.getDefaultParams().size() == 0 )
        cout << "       No parameters" << endl;
      else {
        cout << "       Parameters:" << endl;
        for (auto parameterId : command.getDefaultParams().keys())
        {
          cout << "        - " << parameterId;
          cout <<" [" << demangleName(command.getDefaultParams().get(parameterId).type().name()) << "]" << endl;
        }
      }
    }
  }
}


void printCommandAndFSMSummary(const action::Board& aBoard)
{
  using namespace std;
  using namespace swatch::action;
  using namespace swatch::core;

  cout << "Summary of devices:" << endl;

  for (auto deviceId : aBoard.getDevices())
  {
    const Device& device = aBoard.getDevice(deviceId);
    cout << endl;

    cout << " * " << deviceId << endl;
    cout << "   Type: " << device.getStub().creator << endl;

    cout << "   Commands:" << endl;
    size_t i = 0;
    for(auto commandId : device.getCommands())
    {
      const Command& command = device.getCommand(commandId);
      i++;

      cout << "    " << i << ") " << command.getId() << endl;
      if ( command.getDefaultParams().size() == 0 )
        cout << "       No parameters" << endl;
      else {
        cout << "       Parameters:" << endl;
        for (auto parameterId : command.getDefaultParams().keys())
        {
          cout << "        - " << parameterId;
          cout <<" [" << demangleName(command.getDefaultParams().get(parameterId).type().name()) << "]" << endl;
        }
      }
    }

    cout << "   FSMs:" << endl;
    i = 0;
    for(auto fsmId : device.getStateMachines())
    {
      const StateMachine& fsm = device.getStateMachine(fsmId);
      i++;

      cout << "    " << i << ") " << fsm.getId() << endl;
      cout << "       States: " << core::join(fsm.getStates(), ", ") << endl;

      cout << "       Transitions:" << endl;
      for (auto stateId : fsm.getStates()) {
        for (auto transition : fsm.getTransitions(stateId)) {
          cout << "        * " << transition.first << " (" << stateId << " -> " << transition.second->getEndState() << ")" << endl;
        }
      }
    }
  }
}


void loadLibraries(const std::vector<std::string>& aPaths, const boost::filesystem::path& aBase)
{
  for (auto path: aPaths)
  {
    boost::filesystem::path filePath( path );

    if (filePath.is_relative())
      filePath = aBase / filePath;

    if ( (not boost::filesystem::is_regular_file(filePath)) and (not boost::filesystem::is_symlink(filePath)) )
      throw std::runtime_error("Path '" + filePath.native() + "' does not point to a file");

    std::cout << "Loading library '" << filePath.c_str() << "'" << std::endl;
    if ( not dlopen( filePath.c_str(), RTLD_LAZY | RTLD_GLOBAL ) )
      throw std::runtime_error( "Failed to load library at '" + filePath.native() + "': '" + dlerror() + "'" );
  }
}


}
