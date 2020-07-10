#ifndef __SWATCH_CORE_PSETCONSTRAINT_HPP__
#define __SWATCH_CORE_PSETCONSTRAINT_HPP__


// Standard C++ headers
#include <typeinfo>
#include <unordered_map>

// SWATCH headers
#include "swatch/core/exception.hpp"
#include "swatch/core/Match.hpp"


namespace swatch {
namespace core {

class ParameterSet;
class ConstraintParameterAlreadyExists;

//! Abstract base class for rules on sets of parameters
class PSetConstraint {
public:

  virtual ~PSetConstraint() = default;

  /**
   * @brief      Lists the parameters the constraints depends on.
   *
   * @return     Vector of parameter names.
   */
  std::set<std::string> getParameterNames() const;

  /**
   * @brief      Applies this constraint to the parameters
   * 
   * @throw      
   * @param      Set of parameters that the constraint is applied to
   * @return     true if the supplied ParameterSet passes this constraint; false otherwise.
   */
  Match operator()(const ParameterSet& aParams) const;


protected:

  /**
   * @brief      Registers a new parameter with of type T with this
   *             parameterset rule.
   *
   * @param[in]  aName  Name of the parameter
   *
   * @tparam     T      Type of the new parameter
   */
  template <typename T>
  void require(const std::string& aName);

  /**
   * @brief      Apply this rule to a parameter set.
   *
   * @param[in]  aParams  Input parameters.
   * @return     True if the input parameters match this rule.
   */
  virtual Match verify(const ParameterSet& aParams) const = 0;

  /**
   * @brief      Print the description of this contraint to a output stream.
   *
   * @param      aStream  Target output stream.
   */
  virtual void describe(std::ostream& aStream) const = 0;

  PSetConstraint() = default;

private:
  /**
   * @brief      Check compatibility between the parameters and the rules.
   *
   * @param[in]  aParams  Input parameter set.
   */
  void throwIfNotApplicable(const ParameterSet& aParams) const;

  typedef std::unordered_map<std::string, const std::type_info*> ParameterTypeMap_t;

  //! Map of parameter names and associated types.
  ParameterTypeMap_t mParameterTypeMap;

  friend std::ostream& operator<<(std::ostream& aOut, const PSetConstraint& aConstraint);

};
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& aOut, const PSetConstraint& aConstraint);
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
template <typename T>
void PSetConstraint::require(const std::string& aName)
{
  if (!mParameterTypeMap.insert({aName, &typeid (T)}).second) {
    throw ConstraintParameterAlreadyExists("Parameter '" + aName + "' is already defined.");
  }
}
// ----------------------------------------------------------------------------


SWATCH_DEFINE_EXCEPTION(ConstraintIncompatibleParameter);
SWATCH_DEFINE_EXCEPTION(ConstraintParameterAlreadyExists);
SWATCH_DEFINE_EXCEPTION(ConstraintError);

} // namespace core
} // namespace swatch



#endif /* __SWATCH_CORE_PSETCONSTRAINT_HPP__ */