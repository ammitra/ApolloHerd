#ifndef __SWATCH__DUMMY_COMMANDS__CMPWRUP_HPP__
#define __SWATCH__DUMMY_COMMANDS__CMPWRUP_HPP__

#include "swatch/action/Command.hpp"

namespace swatch {
namespace dummy {
namespace commands {

class CMPwrUp : public action::Command {
public:
    CMPwrUp(const std::string& aId, action::ActionableObject& aActionable);
    ~CMPwrUp();
private:
    State code(const core::ParameterSet& aParams);
};

}   // commands
}   // dummy
}   // swatch

#endif    // __SWATCH__DUMMY_COMMANDS__CMPWRUP_HPP__