/**
 * @file    Rule.hpp
 * @author  Alessandro Thea
 * @brief   
 * @date    April 2017
 *
 */

#ifndef __SWATCH_CORE_RULE_HPP__
#define __SWATCH_CORE_RULE_HPP__

#include <iosfwd>
#include <typeinfo>

#include <boost/any.hpp>
#include <boost/static_assert.hpp>

#include "swatch/core/exception.hpp"
#include "swatch/core/Match.hpp"


namespace swatch {
namespace core {

// ----------------------------------------------------------------------------

/**
 * @brief      Base class for Rule Validator objects
 * @details    Defined the abstract interface for rule objects
 */
class AbstractRule {
public:
  virtual ~AbstractRule() {}

  /**
   * @brief      Applies this rule against a parameter
   *
   * @param      The                object instance that the rule is applied to
   * @throws     RuleTypeMismatch  if the supplied object instance is not of
   *                                the expected type
   * @return     true if the supplied object instance passes this rule; false
   *             otherwise.
   */
  virtual Match operator()(const boost::any& aObject) const = 0;

  //! Returns type of object that this rule can be applied to
  virtual const std::type_info& type() const = 0;

protected:

  virtual void describe(std::ostream& aStream) const = 0;

private:
  AbstractRule() {}

  //  AbstractRule can only extended by Rule template 
  template<typename T>
  friend class Rule; // every Rule<T> is a friend of AbstractRule

  friend std::ostream& operator<<(std::ostream& aOut, const AbstractRule& aRule);
};
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& aOut, const AbstractRule& aRule);
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template<typename T>
class Rule : public AbstractRule {

public:

  virtual ~Rule() {};

  virtual const std::type_info& type() const final;

  virtual Match operator()(const boost::any& aValue) const final;

  virtual Match verify(const T&) const = 0;

protected:

  Rule() {};
};
// ----------------------------------------------------------------------------


typedef AbstractRule* (*RuleCloner_t)(const AbstractRule&);

// ----------------------------------------------------------------------------
//
// @param[in]  aRule  Rule to Clone
//
// @tparam     R      Target Rule class
//
// @return     Clone of aRule
//
template<typename R>
AbstractRule* RuleCloner( const AbstractRule& aRule ) {
  return new R(dynamic_cast<const R&>(aRule));
}
// ----------------------------------------------------------------------------

} // namespace core
} // namespace swatch


SWATCH_DEFINE_EXCEPTION(RuleTypeMismatch);
SWATCH_DEFINE_EXCEPTION(RuleArgumentError);


#include "swatch/core/Rule.hxx"

#endif /* __SWATCH_CORE_RULE_HPP__ */