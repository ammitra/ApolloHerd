#include "swatch/core/rules/OutOfRange.hpp"


namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------
template<typename T>
OutOfRange<T>::OutOfRange( const T& aLowerBound, const T& aUpperBound ) : mLowerBound(aLowerBound), mUpperBound(aUpperBound) {
	
	if (mLowerBound >= mUpperBound ) {
		std::ostringstream lMsg;
		lMsg << "Upper bound (" << mUpperBound << ") is smaller than lower bound (" << mLowerBound << ")";
		throw RuleArgumentError(lMsg.str());
	}
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T>
Match OutOfRange<T>::verify( const T& aValue ) const
{
  // const T& lValue = dynamic_cast<const T&>(aValue);
  return Match(aValue < mLowerBound || mUpperBound < aValue);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
void OutOfRange<T>::describe(std::ostream& aStream) const
{
  aStream << "(x < " << mLowerBound << " || x > " << mUpperBound << ")";
}
// ----------------------------------------------------------------------------

template class OutOfRange<bool>;
template class OutOfRange<int32_t>;
template class OutOfRange<int64_t>;
template class OutOfRange<uint32_t>;
template class OutOfRange<uint64_t>;
template class OutOfRange<float>;
template class OutOfRange<double>;

} // namespace rules
} // namespace core
} // namespace swatch
