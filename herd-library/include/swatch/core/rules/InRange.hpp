#ifndef __SWATCH_CORE_RULES_INRANGE_HPP__
#define __SWATCH_CORE_RULES_INRANGE_HPP__


#include "swatch/core/Rule.hpp"


namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------
template<typename T>
class InRange : public Rule<T> {
    
public:

  /**
   * @brief      Constructor
   *
   * @param[in]  aLowerBound  The range lower bound
   * @param[in]  aUpperBound  The range upper bound
   */
  InRange( const T& aLowerBound, const T& aUpperBound );
  virtual ~InRange() {};

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

  const T mLowerBound;
  const T mUpperBound;
};
// ----------------------------------------------------------------------------


} // namespace rules
} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_RULES_INRANGE_HPP__ */