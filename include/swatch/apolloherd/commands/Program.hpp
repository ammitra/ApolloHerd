#ifndef __SWATCH_APOLLOHERD_COMMANDS_PROGRAM_HPP__
#define __SWATCH_APOLLOHERD_COMMANDS_PROGRAM_HPP__

#include "swatch/action/Command.hpp"
#include "swatch/action/File.hpp"
#include "emp/swatch/utilities.hpp"

#include "swatch/apolloherd/ApolloCMFPGA.hpp"
#include <BUTool/CommandReturn.hh>

namespace swatch {
namespace apolloherd {
namespace commands {

class Program : public action::Command {
public: 
  Program(const std::string& aId, action::ActionableObject& aActionable);
  ~Program();
private:
  State code(const core::ParameterSet& aParams);
};

}   // commands
}   // apolloherd
}   // swatch

#endif  // __SWATCH_APOLLOHERD_COMMANDS_PROGRAM_HPP__