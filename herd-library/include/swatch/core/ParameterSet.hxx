/**
 * @file    ParameterSet.hxx
 * @author  Alessandro Thea, Tom Williams
 * @date    February 2015
 */

#ifndef __SWATCH_CORE_PARAMETERSET_HXX__
#define __SWATCH_CORE_PARAMETERSET_HXX__


// IWYU pragma: private, include "swatch/core/ParameterSet.hpp"

#include <boost/any.hpp>

// SWATCH headers
#include "swatch/core/ParameterSet.hpp"
#include "swatch/core/utilities.hpp"


namespace swatch {
namespace core {


//---
template<typename T>
const T& ParameterSet::get( const std::string& aName ) const
{
  try {
    return boost::any_cast<const T&>( get(aName) );
  }
  catch ( const boost::bad_any_cast& ) {
    const std::type_info* lFrom( &get(aName).type() );
    const std::type_info* lTo( &typeid(T) );
    throw ParameterFailedCast("Unable to cast '"+aName+"' from '"+demangleName( lFrom->name() )+"' to '"+ demangleName( lTo->name() ) + "'" );
  }
}

//---
template<typename T>
void ParameterSet::add( const std::string& aName , const T& aData )
{
  adopt(aName, std::shared_ptr<boost::any>(new boost::any(aData)));
}

} // namespace core
} // namespace swatch


#endif /* __SWATCH_CORE_PARAMETERSET_HXX__ */
