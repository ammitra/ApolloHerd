
#include "swatch/core/Rule.hpp"


namespace swatch {
namespace core {

// ----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& aOut, const AbstractRule& aRule)
{
  aRule.describe(aOut);
  return aOut;
}
// ----------------------------------------------------------------------------


} // namespace core
} // namespace swatch
