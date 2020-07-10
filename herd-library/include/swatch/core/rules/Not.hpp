#ifndef __SWATCH_CORE_RULES_NOT_HPP__
#define __SWATCH_CORE_RULES_NOT_HPP__


#include "swatch/core/Rule.hpp"
#include "swatch/core/utilities.hpp"

#include <memory>

namespace swatch {
namespace core {
namespace rules {

// ----------------------------------------------------------------------------


template <typename T>
class Not: public Rule<T> {
public:

  /**
   * @brief      Not rule constructor
   *
   * @param[in]  aRule  A rule
   *
   * @tparam     Rule   { description }
   */
  template<typename SubRule>
  Not( const SubRule& aSubRule ); 

  Not( const Not& aOther );

  virtual Match verify( const T& aValue ) const final;

private:

  virtual void describe(std::ostream& aStream) const final;

  // Left rule
	std::unique_ptr<Rule<T>> mRule;

	//! Left rule cloner function to be used in the Copy Constructor
	RuleCloner_t mRuleCloner;

};
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
template<typename SubRule>
Not<T>::Not( const SubRule& aSubRule ) {
	BOOST_STATIC_ASSERT_MSG( (std::is_base_of<Rule<T>, SubRule>::value), "class SubRule in Not( const SubRule& aSubRule ) must be a descendant of Rule<T>" );

	mRule = std::unique_ptr<Rule<T>>(new SubRule(aSubRule));

	mRuleCloner = RuleCloner<SubRule>;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
Not<T>::Not( const Not& aOther ) :
	mRule(dynamic_cast<Rule<T>*>((*aOther.mRuleCloner)(*aOther.mRule))),
	mRuleCloner(aOther.mRuleCloner) {
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
Match Not<T>::verify( const T& aValue ) const 
{

	Match lResult = (*mRule)(aValue);

	// Flip the retirn status
	lResult.ok = !lResult.ok;

	// Return the merger
  return lResult;
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T>
void Not<T>::describe(std::ostream& aStream) const 
{
  aStream << "!(" << *mRule << ")"; 
}
// ----------------------------------------------------------------------------

} // namespace rules
} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_RULES_NOT_HPP__ */