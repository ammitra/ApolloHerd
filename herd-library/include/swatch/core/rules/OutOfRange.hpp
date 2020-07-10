#ifndef __SWATCH_CORE_RULES_OUTOFRANGE_HPP__
#define __SWATCH_CORE_RULES_OUTOFRANGE_HPP__


#include "swatch/core/Rule.hpp"


namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------
template<typename T>
class OutOfRange : public Rule<T> {
    
public:
  
    /**
   * @brief      Constructor
   *
   * @param[in]  aLowerBound  The range lower bound
   * @param[in]  aUpperBound  The range upper bound
   */
  OutOfRange( const T& aLowerBound, const T& aUpperBound );
  virtual ~OutOfRange() {};

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

#endif /* __SWATCH_CORE_RULES_OUTOFRANGE_HPP__ */