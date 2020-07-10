#include "swatch/core/rules/OfSize.hpp"


namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------
template<typename T>
Match OfSize<T>::verify(const T& aValue) const 
{
  return aValue.size() == mSize;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
void OfSize<T>::describe(std::ostream& aStream) const
{
  aStream << "len(x) = " << mSize;
}
// ----------------------------------------------------------------------------

template class OfSize<std::vector<bool>>;
template class OfSize<std::vector<uint32_t>>;
template class OfSize<std::vector<uint64_t>>;
template class OfSize<std::vector<int32_t>>;
template class OfSize<std::vector<int64_t>>;
template class OfSize<std::vector<float>>;
template class OfSize<std::vector<double>>;
template class OfSize<std::vector<std::string>>;

} // namespace rules
} // namespace core
} // namespace swatch
