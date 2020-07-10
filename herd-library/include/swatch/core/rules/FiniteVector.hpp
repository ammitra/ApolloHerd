#ifndef __SWATCH_CORE_RULES_FINITEVECTOR_HPP__
#define __SWATCH_CORE_RULES_FINITEVECTOR_HPP__


// SWATCH headers
#include "swatch/core/Rule.hpp"


namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------


//! Class for finite vector rule.
template<typename T>
class FiniteVector : public Rule<T> {

  // FIXME: Add static assert requiring that type is a std::vector (or adjust template parameters accordingly) 

public:
  
  FiniteVector() {};
  virtual ~FiniteVector() {};

  /**
   * @brief      Checks if all elements in user-supplied vector are finite
   *
   * @param[in]  aValue  The vector to check
   *
   * @return     True if all elements in aValue are finite; false otherwise
   */
  virtual Match verify( const T& aValue ) const;

private:

  virtual void describe(std::ostream& aStream) const;
};
// ----------------------------------------------------------------------------


} // namespace rules
} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_RULES_FINITEVECTOR_HPP__ */