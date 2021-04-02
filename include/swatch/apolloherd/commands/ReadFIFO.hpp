#ifndef __SWATCH_APOLLOHERD_COMMANDS_READFIFO_HPP__
#define __SWATCH_APOLLOHERD_COMMANDS_READFIFO_HPP__

#include "swatch/action/Command.hpp"

namespace swatch {
namespace apolloherd {
namespace commands {

class ReadFIFO : public action::Command {
public:
  ReadFIFO(const std::string& aId, action::ActionableObject& aActionable);
  ~ReadFIFO();
private:
  State code(const core::ParameterSet& aParams);
};

}   // commands 
}   // apolloherd
}   // swatch

#endif 