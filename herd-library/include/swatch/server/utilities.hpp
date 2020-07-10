
#ifndef __SWATCH_SERVER_UTILITIES_HPP__
#define __SWATCH_SERVER_UTILITIES_HPP__


#include <string>
#include <vector>

#include <msgpack.hpp>
#include <zmq.hpp>

#include "swatch/action/StateMachine.hpp"


namespace swatch {

namespace action {
class Board;
class Command;
class CommandSnapshot;
}
namespace core {
class ParameterSet;
}

namespace server {

void encodeServerError(const std::string& , msgpack::sbuffer&);

void encodeDeviceInfo(const action::Board& , msgpack::sbuffer&);

void encodeCommandInfo(const action::Board& , msgpack::sbuffer&);

void encodeFsmInfo(const action::Board& aBoard, msgpack::sbuffer& aBuffer);

void encodeCommandResult(const action::CommandSnapshot& , msgpack::sbuffer&);

void encodeTransitionResult(const action::TransitionSnapshot& , msgpack::sbuffer&);

core::ParameterSet extractParameterSet(const action::Command&, const msgpack::object&);

std::vector<core::ParameterSet> extractParameterSets(const action::Transition&, const msgpack::object&);

} // namespace server
} // namespace swatch

#endif
