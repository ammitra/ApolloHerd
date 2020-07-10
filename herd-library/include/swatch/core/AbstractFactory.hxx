/**
 * @file    AbstractFactory.hxx
 * @author  Alessandro Thea
 * @brief   Implementation of the templated methods
 * @date    July 2014
 *
 */

#ifndef __SWATCH_CORE_ABSTRACTFACTORY_HXX__
#define __SWATCH_CORE_ABSTRACTFACTORY_HXX__


#include "swatch/core/utilities.hpp"


namespace swatch {
namespace core {

//---
template<typename T>
AbstractFactory<T>* AbstractFactory<T>::sInstance = 0x0;


//---
template<typename T>
AbstractFactory<T>* AbstractFactory<T>::get()
{
  if ( !sInstance )
    sInstance = new AbstractFactory<T>();

  return sInstance;
}


//---
template<typename T>
template<typename P>
P* AbstractFactory<T>::make( const std::string& aCreatorId, const AbstractStub& aStub )
{

  // Basic consistency check at compile time
  BOOST_STATIC_ASSERT( (boost::is_base_of<T,P>::value) );

  typename std::unordered_map< std::string , std::shared_ptr<CreatorInterface> >::const_iterator lIt = mCreators.find ( aCreatorId );

  if ( lIt == mCreators.end() ) {
    throw CreatorNotFound (demangleName(typeid(this).name())+": Creator '"+aCreatorId+"' not found");
  }

  // Keep a reference to the bare product
  T* lBase = (*lIt->second)( aStub );
  P* lProduct = dynamic_cast<P*>( lBase );

  // If dynamic cast failed, delete the product and throw
  if ( not lProduct ) {
    delete lBase;
    throw FailedFactoryCast("Failed to cast factory product to "+demangleName(typeid(P).name()));
  }

  return lProduct;
}


//---
template<typename T>
template<typename U>
T*
AbstractFactory<T>::BasicCreator<U>::operator ()( const AbstractStub& aStub )
{
  return new U( aStub );
}


//---
template <typename T>
template <typename K>
bool
AbstractFactory<T>::add(const std::string& aName)
{
  typename std::unordered_map<std::string, std::shared_ptr<CreatorInterface> >::const_iterator lIt = mCreators.find(aName);

  if (lIt != mCreators.end()) {
    return false;
  }

  mCreators[aName] = std::shared_ptr<CreatorInterface> (new K());
  return true;
}

} // namespace test
} // namespace swatch


#endif	/* __SWATCH_CORE_ABSTRACTFACTORY_HXX__ */

