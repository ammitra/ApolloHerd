#include "swatch/core/rules/PathExists.hpp"

#include <boost/filesystem.hpp>

namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------
Match PathExists::verify(const std::string& aValue) const 
{

	if ( aValue.empty() ) {
		return Match(false, "Empty path");
	}

	boost::filesystem::path lPath = boost::filesystem::path(aValue);

	if (!mPrefix.empty()) 
		lPath = boost::filesystem::path(mPrefix) / lPath;

	if (!mExtension.empty())
		lPath.replace_extension(mExtension);

	return (boost::filesystem::exists(lPath));
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void PathExists::describe(std::ostream& aStream) const
{
  aStream << "exists(x)";
}
// ----------------------------------------------------------------------------

} // namespace rules
} // namespace core
} // namespace swatch
