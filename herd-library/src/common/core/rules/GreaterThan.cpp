#include "swatch/core/rules/GreaterThan.hpp"


#include <cstdint>


namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------
template<typename T>
Match GreaterThan<T>::verify( const T& aValue ) const
{
  // const T& lValue = dynamic_cast<const T&>(aValue);
  return Match(aValue > lLowerBound);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
void GreaterThan<T>::describe(std::ostream& aStream) const
{
  aStream << "x > " << lLowerBound;
}
// ----------------------------------------------------------------------------

template class GreaterThan<int32_t>;
template class GreaterThan<int64_t>;
template class GreaterThan<uint32_t>;
template class GreaterThan<uint64_t>;
template class GreaterThan<float>;
template class GreaterThan<double>;


} // namespace rules
} // namespace core
} // namespace swatch
