#ifndef __SWATCH_APOLLOHERD_COMMANDS_SVFPLAYER_HPP__
#define __SWATCH_APOLLOHERD_COMMANDS_SVFPLAYER_HPP__

#include "swatch/action/Command.hpp"

namespace swatch {
namespace apolloherd {
namespace commands {

class SVFPlayer : public action::Command {
public:
  SVFPlayer(const std::string& aId, action::ActionableObject& aActionable);
  ~SVFPlayer();
private:
  State code(const core::ParameterSet& aParams);
};

}   // commands
}   // apolloherd
}   // swatch

#endif