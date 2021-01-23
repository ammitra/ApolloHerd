#ifndef __SWATCH_APOLLOHERD_COMMANDS_CMPWRDOWN_HPP__
#define __SWATCH_APOLLOHERD_COMMANDS_CMPWRDOWN_HPP__

#include "swatch/action/Command.hpp"

namespace swatch {
namespace apolloherd {
namespace commands {

class CMPwrDown : public action::Command {
public:
  CMPwrDown(const std::string& aId, action::ActionableObject& aActionable);
  ~CMPwrDown();
private:
  State code(const core::ParameterSet& aParams);
};

}   // commands
}   // apolloherd
}   // swatch

#endif