
#include "swatch/core/PSetConstraint.hpp"


#include "swatch/core/utilities.hpp"
#include "swatch/core/ParameterSet.hpp"


namespace swatch {
namespace core {


// ----------------------------------------------------------------------------
std::set<std::string> PSetConstraint::getParameterNames() const
{
  std::set<std::string> lNames;
  std::transform(mParameterTypeMap.begin(), mParameterTypeMap.end(), 
    std::inserter(lNames, lNames.end()), 
    boost::bind(&ParameterTypeMap_t::value_type::first, _1)
  );
  return lNames;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
Match PSetConstraint::operator() ( const ParameterSet& aParams ) const 
{
  core::ParameterSet lParams(aParams);

  throwIfNotApplicable(lParams);
  

  // Create a minimal parameter set to feed verify
  core::ParameterSet lOnlyRequired;
  for ( const std::string& lName : getParameterNames() ) {
    lOnlyRequired.adopt(lName, lParams);
  }

  // Additional try-catch block wrapping usercode "just in case"
  try {
    return verify( lOnlyRequired );
  } catch ( const exception& lExc ) {
    std::ostringstream lExcMsg;
    lExcMsg << "Failed to apply constraint" << *this << ": " << lExc.what();
    throw ConstraintError(lExcMsg.str());
  }
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
void PSetConstraint::throwIfNotApplicable( const ParameterSet& aParams ) const
{
  std::set<std::string> lNotFound;
  std::map<std::string, std::pair<const std::type_info*, const std::type_info*> > lTypeMismatches;

  for( const auto& x : mParameterTypeMap ) {

    const boost::any* lParam;
    try {
      lParam = &aParams.get(x.first);
    } catch ( const ParameterNotFound& lExc ) {
      // std::cout << x.first << " is missing!" << std::endl;
      lNotFound.insert(x.first);
      continue;
    }

    // std::cout << x.first << " - " << x.second << " - " << &typeid(*lParamPtr) << std::endl;

    if ( *(x.second) != lParam->type()  ) {
        lTypeMismatches[x.first] = { x.second, &lParam->type() };
    }
  }

  if ( !(lNotFound.empty() && lTypeMismatches.empty()) ) {

    std::ostringstream lExcMsg;
    lExcMsg << "Error detected while applying ParameterSet rules." << std::endl;

    // Fill the parameters not found section
    if ( !lNotFound.empty() ) {
      lExcMsg << "Missing parameters: ";
      lExcMsg << "'" << *lNotFound.begin() << "'";

      std::for_each(
          std::next(lNotFound.begin()), lNotFound.end(), 
          [&lExcMsg](const std::string& lStr ) { lExcMsg << ", '" <<  lStr << "'"; } );

      lExcMsg << std::endl;
    }

    // Fill the type mismatch section
    if ( !lTypeMismatches.empty() ) {
      lExcMsg << "Mismatching parameter types" << std::endl;
      for( const auto& lP : lTypeMismatches ) {
        lExcMsg << "  " << lP.first << ":"
                << " expected '" << demangleName(lP.second.first->name()) << "'"
                << " found '" << demangleName(lP.second.second->name()) << "'"
                << std::endl; 
      }
    }

    throw ConstraintIncompatibleParameter(lExcMsg.str());

  }
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& aOut, const PSetConstraint& aConstraint)
{
  aConstraint.describe(aOut);
  return aOut;
}
// ----------------------------------------------------------------------------

} // namespace core
} // namespace swatch
