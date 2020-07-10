#include "swatch/core/rules/FiniteNumber.hpp"


namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------
template<typename T>
Match FiniteNumber<T>::verify( const T& aValue ) const
{
  return Match(std::isfinite(aValue));
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
void FiniteNumber<T>::describe(std::ostream& aStream) const
{
  aStream << "isFinite(x)";
}
// ----------------------------------------------------------------------------

template class FiniteNumber<float>;
template class FiniteNumber<double>;


} // namespace rules
} // namespace core
} // namespace swatch
