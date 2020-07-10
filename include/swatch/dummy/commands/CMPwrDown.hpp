#ifndef __SWATCH_DUMMY_COMMANDS_CMPWRDOWN_HPP__
#define __SWATCH_DUMMY_COMMANDS_CMPWRDOWN_HPP__

#include "swatch/action/Command.hpp"

namespace swatch {
namespace dummy {
namespace commands {

class CMPwrDown : public action::Command {
public:
    CMPwrDown(const std::string& aId, action::ActionableObject& aActionable);
    ~CMPwrDown();
private:
    State code(const core::ParameterSet& aParams);
};

}   // commands
}   // dummy
}   // swatch

#endif      // __SWATCH_DUMMY_COMMANDS_CMPWRDOWN_HPP__