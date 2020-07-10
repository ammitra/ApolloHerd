#ifndef __SWATCH_CORE_RULES_NONE_HPP__
#define __SWATCH_CORE_RULES_NONE_HPP__


#include "swatch/core/Rule.hpp"


namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------


//! Class representing no rule - i.e. parameters with any value will pass this rule.
template <typename T>
class None: public Rule<T> {
public:

  virtual Match verify( const T& aValue ) const final;

private:

  virtual void describe(std::ostream& aStream) const final;

};
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
template<typename T>
Match None<T>::verify( const T& aValue ) const 
{
  return Match(true);
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T>
void None<T>::describe(std::ostream& aStream) const 
{
  aStream << "true"; 
}
// ----------------------------------------------------------------------------

} // namespace rules
} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_RULES_NONE_HPP__ */