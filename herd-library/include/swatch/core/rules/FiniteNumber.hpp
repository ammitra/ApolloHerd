#ifndef __SWATCH_CORE_RULES_FINITENUMBER_HPP__
#define __SWATCH_CORE_RULES_FINITENUMBER_HPP__


#include "swatch/core/Rule.hpp"


namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------
template<typename T>
class FiniteNumber : public Rule<T> {
    
public:
  FiniteNumber() {};
  virtual ~FiniteNumber() {};

  /**
   * @brief      Checks if aValue is finite.
   *
   * @param[in]  aValue  Input value
   *
   * @return     True if aValue is finite
   */
  virtual Match verify( const T& aValue ) const;

private:

  virtual void describe(std::ostream& aStream) const;

};
// ----------------------------------------------------------------------------


} // namespace rules
} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_RULES_FINITENUMBER_HPP__ */