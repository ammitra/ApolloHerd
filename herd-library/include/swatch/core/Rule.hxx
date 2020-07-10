#ifndef __SWATCH_CORE_RULE_HXX__
#define __SWATCH_CORE_RULE_HXX__

#include "Rule.hpp"


#include "swatch/core/utilities.hpp"


namespace swatch {
namespace core {

// ----------------------------------------------------------------------------
template<typename T>
const std::type_info& Rule<T>::type() const
{ 
  return typeid(T);
};
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
Match Rule<T>::operator()(const boost::any& aValue) const
{
  try {
    const T& lValue = boost::any_cast<const T&>(aValue);
    return verify(lValue);
  } catch ( const boost::bad_any_cast& lExc) {
    // Do something?
    throw RuleTypeMismatch("Mismatch between expected type (" + demangleName(typeid(T).name()) + ") and received type (" + demangleName(aValue.type().name()) + ").");
  }
}
// ----------------------------------------------------------------------------


} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_RULE_HXX__ */