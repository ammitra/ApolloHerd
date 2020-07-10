#include "swatch/core/rules/NonEmptyString.hpp"


namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------
Match NonEmptyString::verify(const std::string& aValue) const 
{
	return !aValue.empty();
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void NonEmptyString::describe(std::ostream& aStream) const
{
  aStream << "!x.empty()";
}
// ----------------------------------------------------------------------------

} // namespace rules
} // namespace core
} // namespace swatch
