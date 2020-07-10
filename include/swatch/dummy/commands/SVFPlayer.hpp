#ifndef __SWATCH_DUMMY_COMMANDS_SVFPLAYER_HPP__
#define __SWATCH_DUMMY_COMMANDS_SVFPLAYER_HPP__

#include "swatch/action/Command.hpp"

namespace swatch {
namespace dummy {
namespace commands {

class SVFPlayer : public action::Command {
public:
    SVFPlayer(const std::string& aId, action::ActionableObject& aActionable);
    ~SVFPlayer();
private:
    State code(const core::ParameterSet& aParams);
};

}   // commands
}   // dummy
}   // swatch

#endif    // __SWATCH_DUMMY_COMMANDS_SVFPLAYER_HPP__