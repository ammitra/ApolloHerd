#ifndef __SWATCH_CORE_RULES_GREATERTHAN_HPP__
#define __SWATCH_CORE_RULES_GREATERTHAN_HPP__


#include "swatch/core/Rule.hpp"


namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------
template<typename T>
class GreaterThan : public Rule<T> {
    
public:
  GreaterThan( const T& aLowerBound ) : lLowerBound(aLowerBound) {};
  virtual ~GreaterThan() {};

  /**
   * @brief      Checks if aValue is greater than .
   *
   * @param[in]  aValue  Input value
   *
   * @return     True if aValue is finite
   */
  virtual Match verify( const T& aValue ) const;

private:

  virtual void describe(std::ostream& aStream) const;

  const T lLowerBound;
};
// ----------------------------------------------------------------------------


} // namespace rules
} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_RULES_GREATERTHAN_HPP__ */