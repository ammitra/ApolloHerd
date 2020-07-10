#ifndef __SWATCH_CORE_RULES_AND_HPP__
#define __SWATCH_CORE_RULES_AND_HPP__


#include "swatch/core/Rule.hpp"
#include "swatch/core/utilities.hpp"

#include <memory>


namespace swatch {
namespace core {
namespace rules {


// ----------------------------------------------------------------------------
template <typename T>
class And: public Rule<T> {
public:

  /**
   * @brief      And rule constructor
   *
   * @param[in]  aLeft   Left rule
   * @param[in]  aRight  Right rule
   *
   * @tparam     Left    Left rule class
   * @tparam     Right   Right rule class
   */
  template<typename Left, typename Right>
  And( const Left& aLeft, const Right& aRight ); 

  And( const And& aOther );

  virtual Match verify( const T& aValue ) const final;

private:

  virtual void describe(std::ostream& aStream) const final;

  // Left rule
  std::unique_ptr<Rule<T>> mLeft;

  // Right rule
  std::unique_ptr<Rule<T>> mRight;

  //! Left rule cloner function to be used in the Copy Constructor
  RuleCloner_t mLeftCloner;

  //! Right rule cloner function to be used in the Copy Constructor
  RuleCloner_t mRightCloner;
};
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
template<typename Left, typename Right>
And<T>::And( const Left& aLeft, const Right& aRight ) {
	BOOST_STATIC_ASSERT_MSG( (std::is_base_of<Rule<T>, Left>::value) , "class Left in And( const Left& aLeft, const Right& aRight ) must be a descendant of Rule<T>" );
	BOOST_STATIC_ASSERT_MSG( (std::is_base_of<Rule<T>, Right>::value) , "class Left in And( const Left& aLeft, const Right& aRight ) must be a descendant of Rule<T>" );

	mLeft = std::unique_ptr<Rule<T>>(new Left(aLeft));
	mRight = std::unique_ptr<Rule<T>>(new Right(aRight));

	mLeftCloner = RuleCloner<Left>;
	mRightCloner = RuleCloner<Right>;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
And<T>::And( const And& aOther ) :
	mLeft(dynamic_cast<Rule<T>*>((*aOther.mLeftCloner)(*aOther.mLeft))),
	mRight(dynamic_cast<Rule<T>*>((*aOther.mRightCloner)(*aOther.mRight))),
	mLeftCloner(aOther.mLeftCloner),
	mRightCloner(aOther.mRightCloner) {
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
Match And<T>::verify( const T& aValue ) const 
{

	// Apply the sub rules
	Match lLeft = (*mLeft)(aValue);
	Match lRight = (*mRight)(aValue);

	// Merge details
	std::vector<std::string> lDetails;
	if (!lLeft.details.empty()) lDetails.push_back(lLeft.details);
	if (!lRight.details.empty()) lDetails.push_back(lRight.details);

	// Return the merger
  return Match(lLeft.ok && lRight.ok, core::join(lDetails, "; "));
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T>
void And<T>::describe(std::ostream& aStream) const 
{
  aStream << "(" << *mLeft << " && " << *mRight << ")"; 
}
// ----------------------------------------------------------------------------

} // namespace rules
} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_RULES_AND_HPP__ */