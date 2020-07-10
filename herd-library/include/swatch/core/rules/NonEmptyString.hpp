#ifndef __SWATCH_CORE_RULES_NONEMPTYSTRING_HPP__
#define __SWATCH_CORE_RULES_NONEMPTYSTRING_HPP__


#include "swatch/core/Rule.hpp"


namespace swatch {
namespace core {
namespace rules {

class NonEmptyString : public Rule<std::string> {

public:
  NonEmptyString() {};
  ~NonEmptyString() {};

  virtual Match verify( const std::string& aValue ) const;

private:

  virtual void describe(std::ostream& aStream) const;
  
};

} // namespace rules	
} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_RULES_NONEMPTYSTRING_HPP__ */