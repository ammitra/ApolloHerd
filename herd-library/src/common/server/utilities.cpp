
#include "swatch/server/utilities.hpp"


#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <typeindex>
#include <vector>

#include "boost/lexical_cast.hpp"

#include <msgpack.hpp>

#include "swatch/action/Board.hpp"
#include "swatch/action/Command.hpp"
#include "swatch/action/Device.hpp"
#include "swatch/action/StateMachine.hpp"
#include "swatch/core/ParameterSet.hpp"


namespace swatch {
namespace server {

void encodeServerError(const std::string& aMessage, msgpack::sbuffer& aBuffer)
{
  aBuffer.clear();

  std::pair<std::string, std::string> reply("error", aMessage);
  msgpack::pack(aBuffer, reply);
}


void encodeDeviceInfo(const action::Board& aBoard, msgpack::sbuffer& aBuffer)
{
  msgpack::zone z;
  std::pair<std::string, std::map<std::string,std::map<std::string,msgpack::object>>> data;
  data.first = "ok";
  
  for (auto deviceId : aBoard.getDevices()) {
    const action::Device& device = aBoard.getDevice(deviceId);

    data.second[deviceId]["role"] = msgpack::object(device.getStub().role, z);
    data.second[deviceId]["type"] = msgpack::object(device.getStub().creator, z);
    data.second[deviceId]["uri"] = msgpack::object(device.getStub().uri, z);
    data.second[deviceId]["addressTable"] = msgpack::object(device.getStub().addressTable, z);

    auto status = device.getStatus();
    if (status.isEngaged()) {
      data.second[deviceId]["fsm"] = msgpack::object(status.getStateMachineId(), z);
      data.second[deviceId]["state"] = msgpack::object(status.getState(), z);
    }
    else {
      data.second[deviceId]["fsm"] = msgpack::object();
      data.second[deviceId]["state"] = msgpack::object();
    }
  }

  msgpack::pack(aBuffer, data);
}


void encodeCommandInfo(const action::Board& aBoard, msgpack::sbuffer& aBuffer)
{
  using namespace std;
  msgpack::zone z;
  pair<string, map<string, map<string, tuple<string, msgpack::object, msgpack::object> >>> data;
  data.first = "ok";

  const std::map<std::type_index, std::string> kParameterTypeMap = {
    {std::type_index(typeid(bool)), "bool"},
    {std::type_index(typeid(unsigned int)), "uint"},
    {std::type_index(typeid(int)), "int"},
    {std::type_index(typeid(float)), "float"},
    {std::type_index(typeid(std::string)), "string"},
    {std::type_index(typeid(std::vector<bool>)), "vector:bool"},
    {std::type_index(typeid(std::vector<unsigned int>)), "vector:uint"},
    {std::type_index(typeid(std::vector<int>)), "vector:int"},
    {std::type_index(typeid(std::vector<float>)), "vector:float"},
    {std::type_index(typeid(std::vector<std::string>)), "vector:string"},
  };

  for (auto deviceId : aBoard.getDevices()) {
    const action::Device& device = aBoard.getDevice(deviceId);

    for (auto commandId : device.getCommands()) {
      const action::Command& cmd = device.getCommand(commandId);

      map<string, pair<string, msgpack::object>> parameterInfo;
      for (auto paramId : cmd.getDefaultParams().keys()) {
        const std::string typeName = kParameterTypeMap.at(std::type_index(cmd.getDefaultParams().get(paramId).type()));
        msgpack::object ruleDescription;
        auto ruleIt = cmd.getRules().find(paramId);
        if (ruleIt != cmd.getRules().end())
          ruleDescription = msgpack::object(boost::lexical_cast<string>(*ruleIt->second), z);
        parameterInfo[paramId] = std::make_pair(typeName, msgpack::object(ruleDescription, z));
      }

      map<string, pair<vector<string>, string>> constraintInfo;
      for (auto x: cmd.getConstraints()) {
        constraintInfo[x.first] = make_pair(vector<string>(), boost::lexical_cast<string>(*x.second));
        for (auto parameter: x.second->getParameterNames())
          constraintInfo.at(x.first).first.push_back(parameter);
      }

      data.second[deviceId][cmd.getId()] = make_tuple(cmd.getAlias(), msgpack::object(parameterInfo, z), msgpack::object(constraintInfo, z));
    }
  }

  msgpack::pack(aBuffer, data);
}


void encodeFsmInfo(const action::Board& aBoard, msgpack::sbuffer& aBuffer)
{
  using namespace std;
  msgpack::zone z;
  pair<string, map<string, map<string, tuple<string, string, string, msgpack::object>>>> data;
  data.first = "ok";

  for (auto deviceId : aBoard.getDevices()) {
    const action::Device& device = aBoard.getDevice(deviceId);
    data.second[deviceId] = {};

    for (auto fsmId : device.getStateMachines()) {
      const action::StateMachine& fsm = device.getStateMachine(fsmId);

      map<string, map<string, tuple<string, string, vector<pair<string, string>>>>> fsmStructure;
      for (auto stateId : fsm.getStates()) {
        fsmStructure[stateId] = {};
        for (auto transition : fsm.getTransitions(stateId)) {
          const auto& t = *transition.second;
          vector<pair<string, string>> transitionSteps;
          for (const auto& transitionStep : t)
            transitionSteps.push_back(make_pair(transitionStep.get().getId(), transitionStep.getNamespace()));
          fsmStructure[stateId][transition.first] = make_tuple(t.getAlias(), t.getEndState(), transitionSteps);
        }
      }

      data.second[deviceId][fsmId] = make_tuple(fsm.getAlias(), fsm.getInitialState(), fsm.getErrorState(), msgpack::object(fsmStructure, z));
    }
  }

  msgpack::pack(aBuffer, data);
}


msgpack::object convertCommandSnapshot(const action::CommandSnapshot& aSnapshot, msgpack::zone& aZone)
{
  std::map<std::string, msgpack::object> infoMap;

  infoMap["path"] = msgpack::object(aSnapshot.getActionPath(), aZone);
  const std::string state(boost::lexical_cast<std::string>(aSnapshot.getState()));
  infoMap["state"] = msgpack::object(state, aZone);
  infoMap["duration"] = msgpack::object(aSnapshot.getRunningTime(), aZone);
  infoMap["progress"] = msgpack::object(aSnapshot.getProgress(), aZone);
  infoMap["message"] = msgpack::object(aSnapshot.getStatusMsg(), aZone);

  return msgpack::object(infoMap, aZone);
}


void encodeCommandResult(const action::CommandSnapshot& aSnapshot, msgpack::sbuffer& aBuffer)
{
  std::pair<std::string, msgpack::object > data;
  data.first = "ok";

  msgpack::zone z;
  data.second = convertCommandSnapshot(aSnapshot, z);

  msgpack::pack(aBuffer, data);
}


void encodeTransitionResult(const action::TransitionSnapshot& aSnapshot, msgpack::sbuffer& aBuffer)
{
  std::pair<std::string, std::map<std::string, msgpack::object>> data;
  data.first = "ok";

  msgpack::zone z;
  data.second["path"] = msgpack::object(aSnapshot.getActionPath(), z);
  const std::string state(boost::lexical_cast<std::string>(aSnapshot.getState()));
  data.second["state"] = msgpack::object(state, z);
  data.second["duration"] = msgpack::object(aSnapshot.getRunningTime(), z);
  data.second["progress"] = msgpack::object(aSnapshot.getProgress(), z);
  data.second["num-commands-done"] = msgpack::object(aSnapshot.getNumberOfCompletedCommands(), z);
  data.second["num-commands-total"] = msgpack::object(aSnapshot.getTotalNumberOfCommands(), z);

  std::vector<msgpack::object> cmdInfo;
  for (const auto& cmdSnapshot : aSnapshot)
    cmdInfo.push_back( convertCommandSnapshot(cmdSnapshot, z) );

  data.second["commands"] = msgpack::object(cmdInfo, z);

  msgpack::pack(aBuffer, data);
}


template <typename T>
void extractParameter(core::ParameterSet& aSet, const std::string& aId, const msgpack::object& aObject)
{
  try {
    aSet.add<T>(aId, aObject.as<T>());
  }
  catch (const msgpack::type_error& e) {
    throw std::runtime_error("Error occurred when extracting parameter '" + aId + "' from msgpack message");
  }
}


core::ParameterSet extractParameterSet(const action::Command& aCommand, const msgpack::object& aMessage)
{
  std::map<std::string, msgpack::object> objectMap;
  aMessage.convert(objectMap);

  typedef std::function<void (core::ParameterSet&, const std::string&, const msgpack::object&)> ParameterReader;
  const std::map<std::type_index, ParameterReader> kParameterReaderMap = {
    {std::type_index(typeid(bool)), extractParameter<bool>},
    {std::type_index(typeid(unsigned int)), extractParameter<unsigned int>},
    {std::type_index(typeid(int)), extractParameter<int>},
    {std::type_index(typeid(float)), extractParameter<float>},
    {std::type_index(typeid(std::string)), extractParameter<std::string>},
    {std::type_index(typeid(std::vector<bool>)), extractParameter<std::vector<bool>>},
    {std::type_index(typeid(std::vector<unsigned int>)), extractParameter<std::vector<unsigned int>>},
    {std::type_index(typeid(std::vector<int>)), extractParameter<std::vector<int>>},
    {std::type_index(typeid(std::vector<float>)), extractParameter<std::vector<float>>},
    {std::type_index(typeid(std::vector<std::string>)), extractParameter<std::vector<std::string>>},
  };

  const std::set<std::string> knownParameters = aCommand.getDefaultParams().keys();
  core::ParameterSet parameters;

  for (auto& x : objectMap) {
    if (knownParameters.count(x.first) == 0)
      throw std::runtime_error("Unknown parameter '" + x.first + "' specified for command '" + aCommand.getPath() + "'");

    const std::type_info& type = aCommand.getDefaultParams().get(x.first).type();
    const auto readerIt = kParameterReaderMap.find(std::type_index(type));
    if (readerIt != kParameterReaderMap.end())
      readerIt->second(parameters, x.first, x.second);
    else
      throw std::runtime_error("Cannot extract parameter of type '" + core::demangleName(type.name()) + "' (id: '" + x.first + "')");
  }

  return parameters;
}

std::vector<core::ParameterSet> extractParameterSets(const action::Transition& aTransition, const msgpack::object& aMessage)
{
  std::vector<msgpack::object> objectVec;
  aMessage.convert(objectVec);
  if ( objectVec.size() != aTransition.size() )
    throw std::runtime_error("Vector of invalid length (" + boost::lexical_cast<std::string>(objectVec.size()) + " != " + boost::lexical_cast<std::string>(aTransition.size()) + ") given to extractParameterSets");

  std::vector<core::ParameterSet> paramSets;
  for(size_t i = 0; i < aTransition.size(); i++)
    paramSets.push_back(extractParameterSet((aTransition.begin() + i)->get(), objectVec.at(i)));

  return paramSets;
}

} // namespace server
} // namespace swatch
