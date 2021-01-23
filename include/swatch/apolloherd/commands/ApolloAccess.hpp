#ifndef __SWATCH_APOLLOHERD_COMMANDS_APOLLOACCESS_HPP__
#define __SWATCH_APOLLOHERD_COMMANDS_APOLLOACCESS_HPP__

#include "swatch/action/Command.hpp"

namespace swatch {
namespace apolloherd {
namespace commands {

class ApolloAccess : public action::Command {
public:
  ApolloAccess(const std::string& aId, action::ActionableObject& aActionable);
  ~ApolloAccess();
private:
  State code(const core::ParameterSet& aParams);
};

}   // commands
}   // apolloherd
}   // swatch

#endif