#ifndef __SWATCH_APOLLOHERD_COMMANDS_POWERUP_HPP__
#define __SWATCH_APOLLOHERD_COMMANDS_POWERUP_HPP__

#include "swatch/action/Command.hpp"

#include "swatch/apolloherd/ApolloCMFPGA.hpp"
#include <BUTool/CommandReturn.hh>

namespace swatch {
namespace apolloherd {
namespace commands {

class PowerUp : public action::Command {
public:
  PowerUp(const std::string& aId, action::ActionableObject& aActionable);
  ~PowerUp();
private:
  State code(const core::ParameterSet& aParams);
};

} // commands
} // apolloherd
} // swatch

#endif //__SWATCH_APOLLOHERD_COMMANDS_POWERUP_HPP__