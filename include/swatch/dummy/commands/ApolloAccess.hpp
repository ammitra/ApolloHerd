#ifndef __SWATCH_DUMMY_COMMANDS_APOLLOACCESS_HPP__
#define __SWATCH_DUMMY_COMMANDS_APOLLOACCESS_HPP__

#include "swatch/action/Command.hpp"

namespace swatch {
namespace dummy {
namespace commands {

class ApolloAccess : public action::Command {
public:
    ApolloAccess(const std::string& aId, action::ActionableObject& aActionable);
    ~ApolloAccess();
private:
    State code(const core::ParameterSet& aParams);
};

}   // commands
}   // dummy
}   // swatch

#endif      // __SWATCH_DUMMY_COMMANDS_APOLLOACCESS_HPP__