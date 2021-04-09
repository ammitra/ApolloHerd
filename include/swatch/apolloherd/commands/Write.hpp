#ifndef __SWATCH_APOLLOHERD_COMMANDS_WRITE_HPP__
#define __SWATCH_APOLLOHERD_COMMANDS_WRITE_HPP__

namespace swatch {
namespace apolloherd {
namespace commands {

class Write : public action::Command {
public:
  Write(const std::string& aId, action::ActionableObject& aActionable);
  ~Write();
private:
  State code(const core::ParameterSet& aParams);
};

}   // commands 
}   // apolloherd
}   // swatch

#endif