#include "swatch/core/rules/InRange.hpp"


namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------
template<typename T>
InRange<T>::InRange( const T& aLowerBound, const T& aUpperBound ) :
  mLowerBound(aLowerBound),
  mUpperBound(aUpperBound)
{
  if (mLowerBound >= mUpperBound ) {
    std::ostringstream lMsg;
    lMsg << "Upper bound (" << mUpperBound << ") is smaller than lower bound (" << mLowerBound << ")";
    throw RuleArgumentError(lMsg.str());
  }
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T>
Match InRange<T>::verify( const T& aValue ) const
{
  return Match(mLowerBound < aValue && aValue < mUpperBound);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
void InRange<T>::describe(std::ostream& aStream) const
{
  aStream << mLowerBound << " < x < " << mUpperBound;
}
// ----------------------------------------------------------------------------

template class InRange<int32_t>;
template class InRange<int64_t>;
template class InRange<uint32_t>;
template class InRange<uint64_t>;
template class InRange<float>;
template class InRange<double>;


} // namespace rules
} // namespace core
} // namespace swatch
