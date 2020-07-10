#include "swatch/core/rules/FiniteVector.hpp"


#include <cmath>


namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------
template<typename T>
Match FiniteVector<T>::verify(const T& aValue) const 
{
  return {std::all_of(
      aValue.begin(), aValue.end(),
      [] ( const typename T::value_type& aX ) { return std::isfinite(aX); }
    ),
    ""
  };
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
void FiniteVector<T>::describe(std::ostream& aStream) const
{
  aStream << "all(x,isFinite)";
}
// ----------------------------------------------------------------------------


template class FiniteVector<std::vector<float>>;
template class FiniteVector<std::vector<double>>;


} // namespace rules
} // namespace core
} // namespace swatch
