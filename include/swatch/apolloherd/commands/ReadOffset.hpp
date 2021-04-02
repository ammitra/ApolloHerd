#ifndef __SWATCH_APOLLOHERD_COMMANDS_READOFFSET_HPP__
#define __SWATCH_APOLLOHERD_COMMANDS_READOFFSET_HPP__

#include "swatch/action/Command.hpp"

namespace swatch {
namespace apolloherd {
namespace commands {

class ReadOffset : public action::Command {
public:
  ReadOffset(const std::string& aId, action::ActionableObject& aActionable);
  ~ReadOffset();
private:
  State code(const core::ParameterSet& aParams);
};

}   // commands 
}   // apolloherd
}   // swatch

#endif 