#ifndef __SWATCH_APOLLOHERD_COMMANDS_POWERDOWN_HPP__
#define __SWATCH_APOLLOHERD_COMMANDS_POWERDOWN_HPP__

#include "swatch/action/Command.hpp"

#include "swatch/apolloherd/ApolloCMFPGA.hpp"
#include <BUTool/CommandReturn.hh>

namespace swatch {
namespace apolloherd {
namespace commands {

class PowerDown : public action::Command {
public:
  PowerDown(const std::string& aId, action::ActionableObject& aActionable);
  ~PowerDown();
private:
  State code(const core::ParameterSet& aParams);
};

} // commands
} // apolloherd
} // swatch

#endif //__SWATCH_APOLLOHERD_COMMANDS_POWERDOWN_HPP__