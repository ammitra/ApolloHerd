
#include "swatch/server/Server.hpp"


#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "boost/lexical_cast.hpp"

#include <msgpack.hpp>

#include "swatch/action/Board.hpp"
#include "swatch/action/Command.hpp"
#include "swatch/action/Device.hpp"
#include "swatch/action/StateMachine.hpp"
#include "swatch/core/ParameterSet.hpp"
#include "swatch/server/utilities.hpp"


namespace swatch {
namespace server {


const std::map<std::string, Server::Callback> Server::kCallbacks = {
  {"info-devices", [](const std::vector<msgpack::object>&, action::Board& aBoard) {msgpack::sbuffer reply; encodeDeviceInfo(aBoard, reply); return reply;}},
  {"info-commands", [](const std::vector<msgpack::object>&, action::Board& aBoard) {msgpack::sbuffer reply; encodeCommandInfo(aBoard, reply); return reply;}},
  {"info-fsms", [](const std::vector<msgpack::object>&, action::Board& aBoard) {msgpack::sbuffer reply; encodeFsmInfo(aBoard, reply); return reply;} },
  {"run-command", Server::executeRunCommand},
  {"engage-fsm", Server::executeEngageFSM},
  {"reset-fsm", Server::executeResetFSM},
  {"disengage-fsm", Server::executeDisengageFSM},
  {"run-fsm-transition", Server::executeRunTransition}
};

Server::Server(action::Board& aBoard, uint16_t aPort) :
  mBoard(aBoard),
  mContext(1),
  mSocket(mContext, ZMQ_REP)
{
  mSocket.bind("tcp://*:" + boost::lexical_cast<std::string>(aPort));
}


void Server::start()
{
  while (true) {
    zmq::message_t request;

    mSocket.recv(&request);

    // Deserialize the request
    msgpack::object_handle requestObjH = msgpack::unpack(request.data<char>(), request.size());
    msgpack::object requestObj = requestObjH.get();

    std::cout << "Received message: " << requestObj << std::endl;
    msgpack::sbuffer reply = handleRequest(requestObj);

    std::cout << "Sending reply" << std::endl << std::endl;
    mSocket.send(zmq::message_t(reply.data(), reply.size()));

  }
}


msgpack::sbuffer Server::handleRequest(const msgpack::object& aRequest)
{
  std::vector<msgpack::object> requestData;
  aRequest.convert(requestData);

  const std::string requestType = requestData.at(0).as<std::string>();
  std::cout << "Request type = " << requestType << std::endl;

  auto callback =  kCallbacks.find(requestType);
  if ( callback == kCallbacks.end() ) {
    msgpack::sbuffer replyBuffer;
    encodeServerError("Invalid request type \"" + requestType + "\" received", replyBuffer);
    return replyBuffer;
  }
  else {
    try {
      return callback->second(requestData, mBoard);
    }
    catch (const std::exception& e) {
      msgpack::sbuffer replyBuffer;
      encodeServerError("Exception thrown: " + std::string(e.what()), replyBuffer);
      return replyBuffer;
    }
  }
}


msgpack::sbuffer Server::executeRunCommand(const std::vector<msgpack::object>& aRequest, action::Board& aBoard)
{
  msgpack::sbuffer replyBuffer;
  if ( aRequest.size() !=  4) {
    encodeServerError("Incorrect number of parameters (" + boost::lexical_cast<std::string>(aRequest.size()) + ") received", replyBuffer);
    return replyBuffer;
  }

  const std::string deviceId = aRequest.at(1).as<std::string>();
  const std::string commandId = aRequest.at(2).as<std::string>();

  auto& command = aBoard.getDevice(deviceId).getCommand(commandId);
  core::ParameterSet params = extractParameterSet(command, aRequest.at(3));
  command.exec(params, false);

  encodeCommandResult(command.getStatus(), replyBuffer);
  return replyBuffer;
}


msgpack::sbuffer Server::executeEngageFSM(const std::vector<msgpack::object>& aRequest, action::Board& aBoard)
{
  msgpack::sbuffer replyBuffer;
  if ( aRequest.size() !=  3) {
    encodeServerError("Incorrect number of parameters (" + boost::lexical_cast<std::string>(aRequest.size()) + ") received", replyBuffer);
    return replyBuffer;
  }

  const std::string deviceId = aRequest.at(1).as<std::string>();
  const std::string fsmId = aRequest.at(2).as<std::string>();

  auto& fsm = aBoard.getDevice(deviceId).getStateMachine(fsmId);
  fsm.engage();

  const std::vector<std::string> reply{"ok"};
  msgpack::pack(replyBuffer, reply);

  return replyBuffer;
}


msgpack::sbuffer Server::executeResetFSM(const std::vector<msgpack::object>& aRequest, action::Board& aBoard)
{
  msgpack::sbuffer replyBuffer;
  if ( aRequest.size() !=  3) {
    encodeServerError("Incorrect number of parameters (" + boost::lexical_cast<std::string>(aRequest.size()) + ") received", replyBuffer);
    return replyBuffer;
  }

  const std::string deviceId = aRequest.at(1).as<std::string>();
  const std::string fsmId = aRequest.at(2).as<std::string>();

  auto& fsm = aBoard.getDevice(deviceId).getStateMachine(fsmId);
  fsm.reset();

  const std::vector<std::string> reply{"ok"};
  msgpack::pack(replyBuffer, reply);

  return replyBuffer;
}


msgpack::sbuffer Server::executeDisengageFSM(const std::vector<msgpack::object>& aRequest, action::Board& aBoard)
{
  msgpack::sbuffer replyBuffer;
  if ( aRequest.size() !=  3) {
    encodeServerError("Incorrect number of parameters (" + boost::lexical_cast<std::string>(aRequest.size()) + ") received", replyBuffer);
    return replyBuffer;
  }

  const std::string deviceId = aRequest.at(1).as<std::string>();
  const std::string fsmId = aRequest.at(2).as<std::string>();

  auto& fsm = aBoard.getDevice(deviceId).getStateMachine(fsmId);
  fsm.disengage();

  const std::vector<std::string> reply{"ok"};
  msgpack::pack(replyBuffer, reply);

  return replyBuffer; 
}


msgpack::sbuffer Server::executeRunTransition(const std::vector<msgpack::object>& aRequest, action::Board& aBoard)
{
  msgpack::sbuffer replyBuffer;

  if ( aRequest.size() !=  4) {
    encodeServerError("Incorrect number of parameters (" + boost::lexical_cast<std::string>(aRequest.size()) + ") received", replyBuffer);
    return replyBuffer;
  }

  const std::string deviceId = aRequest.at(1).as<std::string>();
  const std::string transitionId = aRequest.at(2).as<std::string>();

  action::Device& device = aBoard.getDevice(deviceId);
  action::StateMachine& fsm = device.getStateMachine(device.getStatus().getStateMachineId());
  auto& transition = fsm.getTransition(device.getStatus().getState(), transitionId);

  auto paramSets = extractParameterSets(transition, aRequest.at(3));
  transition.exec(paramSets, false);

  encodeTransitionResult(transition.getStatus(), replyBuffer);
  return replyBuffer;
}

} // namespace server
} // namespace swatch
