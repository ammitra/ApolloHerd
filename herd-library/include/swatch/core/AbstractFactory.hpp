/**
 * @file    AbstractFactory.hpp
 * @author  Alessandro Thea
 * @brief   Template class to provide support for abstract classes
 * @date    July 2014
 *
 */

#ifndef __SWATCH_CORE_ABSTRACTFACTORY_HPP__
#define __SWATCH_CORE_ABSTRACTFACTORY_HPP__

// C++ headers
#include <string>
#include <memory>
#include <unordered_map>

// SWATCH headers
#include "swatch/core/AbstractStub.hpp"
#include "swatch/core/exception.hpp"


// Standard factory registration macros
#define _SWATCH_ABSTRACT_REGISTER_CLASS( productname, classname ) \
template<> bool swatch::core::ClassRegistrationHelper< productname, classname >::sInitialised= \
  swatch::core::ClassRegistrationHelper< productname, classname >::init(#classname);

#define _SWATCH_ABSTRACT_REGISTER_CREATOR( productname, creatorname ) \
template<> bool swatch::core::CreatorRegistrationHelper< productname, creatorname >::sInitialised= \
  swatch::core::CreatorRegistrationHelper< productname, creatorname >::init(#creatorname);


namespace swatch {
namespace core {

///! Template class to provide support for factory-style creation of abstract classes
template<typename T>
class AbstractFactory {
private:
  AbstractFactory() {}

public:
  template< typename A, typename D > friend struct ClassRegistrationHelper;
  template< typename A, typename K > friend struct CreatorRegistrationHelper;

  typedef T Product;

  // Non-copyable
  AbstractFactory(const AbstractFactory&) = delete;
  AbstractFactory& operator=(const AbstractFactory& aGuard) = delete;

  virtual ~AbstractFactory() {}

  static AbstractFactory* get();

  template<typename P>
  P* make( const std::string& aCreatorId, const AbstractStub& aStub );

  class CreatorInterface {
  public:
    virtual T* operator()( const AbstractStub& aStub ) = 0;
  };

private:

  template<typename D>
  class BasicCreator : public CreatorInterface {
  public:
    virtual T* operator() ( const AbstractStub& aStub );
  };

  template<typename K>
  bool add(const std::string& aCreatorName);

private:
  static AbstractFactory* sInstance;
  std::unordered_map<std::string, std::shared_ptr<CreatorInterface> > mCreators;
};


/**
 * Factory helper class
 *
 * @tparam A Base product type
 * @tparam D Derived product type
 */
template< typename A, typename D >
struct ClassRegistrationHelper {
  //! Dummy variable required as initialisation target
  static bool sInitialised;

  static bool init(const std::string& aClassName)
  {
    AbstractFactory<A>::get()->template add< typename AbstractFactory<A>::template BasicCreator<D> > ( aClassName );
    return true;
  }
};


/**
 * Factory helper class
 *
 * @tparam A Base product type
 * @tparam K Creator type
 */
template< typename A, typename K >
struct CreatorRegistrationHelper {
  //! Dummy variable required as initialisation target
  static bool sInitialised;

  static bool init(const std::string& aCreatorName)
  {
    AbstractFactory<A>::get()->template add< K > ( aCreatorName );
    return true;
  }
};


} // namespace core
} // namespace swatch

SWATCH_DEFINE_EXCEPTION(CreatorNotFound)
SWATCH_DEFINE_EXCEPTION(FailedFactoryCast)

#include "swatch/core/AbstractFactory.hxx"


#endif  /* __SWATCH_CORE_ABSTRACTFACTORY_HPP__ */
