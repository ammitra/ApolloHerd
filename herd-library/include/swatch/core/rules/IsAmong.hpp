#ifndef __SWATCH_CORE_RULES_ISAMONG_HPP__
#define __SWATCH_CORE_RULES_ISAMONG_HPP__


// SWATCH headers
#include "swatch/core/Rule.hpp"


namespace swatch {
namespace core {
namespace rules {

class IsAmong : public Rule<std::string> {

public:
  IsAmong( const std::vector<std::string>& aChoices ) :
    mChoices(aChoices)
  {}
  ~IsAmong() {}

  virtual Match verify( const std::string& aValue ) const;

private:

  virtual void describe(std::ostream& aStream) const;
  
  std::vector<std::string> mChoices;
};

} // namespace rules
} // namespace core
} // namespace swatch


#endif /* __SWATCH_CORE_RULES_ISAMONG_HPP__ */