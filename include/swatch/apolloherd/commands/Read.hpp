#ifndef __SWATCH_APOLLOHERD_COMMANDS_READ_HPP__
#define __SWATCH_APOLLOHERD_COMMANDS_READ_HPP__

#include "swatch/action/Command.hpp"

namespace swatch {
namespace apolloherd {
namespace commands {

class Read : public action::Command {
public:
  Read(const std::string& aId, action::ActionableObject& aActionable);
  ~Read();
private:
  State code(const core::ParameterSet& aParams);
};

}   // commands 
}   // apolloherd
}   // swatch

#endif