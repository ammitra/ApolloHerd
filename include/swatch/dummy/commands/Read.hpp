#ifndef __SWATCH_DUMMY_COMMANDS_READ_HPP__
#define __SWATCH_DUMMY_COMMANDS_READ_HPP__

#include "swatch/action/Command.hpp"

namespace swatch {
namespace dummy {
namespace commands {

class Read : public action::Command {
public:
    Read(const std::string& aId, action::ActionableObject& aActionable);
    ~Read();
private:
    State code(const core::ParameterSet& aParams);
};

}   // commands 
}   // dummy
}   // swatch

#endif    // __SWATCH_DUMMY_COMMANDS_READ_HPP__