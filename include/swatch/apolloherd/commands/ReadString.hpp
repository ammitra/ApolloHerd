#ifndef __SWATCH_APOLLOHERD_COMMANDS_READ_STRING_HPP__
#define __SWATCH_APOLLOHERD_COMMANDS_READ_STRING_HPP__

namespace swatch {
namespace apolloherd {
namespace commands {

class ReadString : public action::Command {
public:
  ReadString(const std::string& aId, action::ActionableObject& aActionable);
  ~ReadString();
private:
  State code(const core::ParameterSet& aParams);
};

}   // commands 
}   // apolloherd
}   // swatch


#endif //__SWATCH_APOLLOHERD_COMMANDS_READ_STRING_HPP__