#include "swatch/core/rules/LesserThan.hpp"


namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------
template<typename T>
Match LesserThan<T>::verify( const T& aValue ) const
{
  // const T& lValue = dynamic_cast<const T&>(aValue);
  return Match(aValue < lLowerBound);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
void LesserThan<T>::describe(std::ostream& aStream) const
{
  aStream << "x < " << lLowerBound;
}
// ----------------------------------------------------------------------------

template class LesserThan<int32_t>;
template class LesserThan<int64_t>;
template class LesserThan<uint32_t>;
template class LesserThan<uint64_t>;
template class LesserThan<float>;   
template class LesserThan<double>;   


} // namespace rules
} // namespace core
} // namespace swatch
