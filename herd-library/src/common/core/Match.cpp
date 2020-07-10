#include "swatch/core/Match.hpp"


namespace swatch {
namespace core {


// ----------------------------------------------------------------------------
Match::Match(bool ok, std::string details) :
ok(ok), details(details)
{
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
bool Match::operator!=(const Match& right) const
{
  bool result = !(*this == right); // Reuse equals operator
  return result;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
bool Match::operator==(const Match& right) const
{
  bool result = (this->ok == right.ok && this->details == right.details); // Compare right and *this here
  return result;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const Match& obj)
{
  // Write obj to stream
  os << obj.ok;
  return os;
}
// ----------------------------------------------------------------------------

} // namespace core
} // namespace swatch
