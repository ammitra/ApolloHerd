#ifndef __SWATCH_ACTION_COMMANDPARVALIDATORDEFAULTS_HPP__
#define __SWATCH_ACTION_COMMANDPARVALIDATORDEFAULTS_HPP__

#include "swatch/core/Rule.hpp"
#include "swatch/core/rules/None.hpp"
#include "swatch/core/rules/FiniteNumber.hpp"
#include "swatch/core/rules/FiniteVector.hpp"


namespace swatch {
namespace action {

template <typename T>
class UndefinedDefaultRule: public core::Rule<T> {
    // Triggered the following assert only if the class is instantiated
    BOOST_STATIC_ASSERT_MSG( sizeof(T) == 0 , "No default validator defined for class T" );
public:
    
    virtual bool apply( const T& aValue ) const final { return false; }

private:

    virtual void describe(std::ostream& aStream) const final {}

};
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T, class Enable = void>
struct DefaultCmdParRule {
  typedef UndefinedDefaultRule<T> type;
};
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T>
struct DefaultCmdParRule< T, typename std::enable_if<
    std::is_same<float, T>{} ||
    std::is_same<double, T>{}
  >::type > {
  
  typedef core::rules::FiniteNumber<T> type;
};
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T>
struct DefaultCmdParRule< T, typename std::enable_if<
    std::is_same<std::vector<float>, T>{} ||
    std::is_same<std::vector<double>, T>{}
  >::type > {

  typedef core::rules::FiniteVector<T> type;
};
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T>
struct DefaultCmdParRule< T, typename std::enable_if<
    std::is_same<bool, T>{} || 
    std::is_same<int32_t, T>{} ||
    std::is_same<int64_t, T>{} ||
    std::is_same<uint32_t, T>{} ||
    std::is_same<uint64_t, T>{} ||
    std::is_same<std::string, T>{} ||
    std::is_same<std::vector<bool>, T>{} || 
    std::is_same<std::vector<int32_t>, T>{} ||
    std::is_same<std::vector<int64_t>, T>{} ||
    std::is_same<std::vector<uint32_t>, T>{} ||
    std::is_same<std::vector<uint64_t>, T>{} ||
    std::is_same<std::vector<std::string>, T>{}
  >::type > {
  
  typedef core::rules::None<T> type;
};
// ----------------------------------------------------------------------------


} // namespace action
} // namespace swatch


#endif
