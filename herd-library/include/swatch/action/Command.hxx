/*
 * File:   Command.hxx
 * Author: kreczko
 *
 * Created on March 3, 2015
 */

#ifndef __SWATCH_ACTION_COMMAND_HXX__
#define __SWATCH_ACTION_COMMAND_HXX__


// IWYU pragma: private, include "swatch/action/Command.hpp"

#include "swatch/action/Command.hpp"
#include "swatch/action/ActionableObject.hpp"
#include "swatch/core/Rule.hpp"
#include "swatch/core/PSetConstraint.hpp"


namespace swatch {
namespace action {


// ----------------------------------------------------------------------------
// TODO (when migrated to full C++11) : Call other Command constructor to minimise duplication
template<typename T>
Command::Command( const std::string& aId , ActionableObject& aResource, const T& aDefault ) :
  Functionoid( aId, "", aResource ),
  mActionableStatus(aResource.mStatus),
  mDefaultResult(T(aDefault)),
  mState(kInitial),
  mProgress(0.),
  mStatusMsg("initialised"),
  mResult(new boost::any()),
  mMutex()
{
}


// ----------------------------------------------------------------------------
template<typename T>
Command::Command( const std::string& aId , const std::string& aAlias, ActionableObject& aResource, const T& aDefault ) :
  Functionoid( aId, aAlias, aResource ),
  mActionableStatus(aResource.mStatus),
  mDefaultResult(T(aDefault)),
  mState(kInitial),
  mProgress(0.),
  mStatusMsg("initialised"),
  mResult(new boost::any()),
  mMutex()
{
}


// ----------------------------------------------------------------------------
template<typename T, typename R>
void Command::registerParameter(const std::string& aName, const T& aDefaultValue, R aRule)
{
  // R must be derived from Rule<T>
  BOOST_STATIC_ASSERT_MSG( (std::is_base_of<core::Rule<T>, R>::value) , "class R must be a descendant of core::Rule<T>" );

  if ( getPath() != getId() ) {
    // The Command has already been registered. Parameters list cannot be modified
    throw CommandParameterRegistrationFailed("Registering parameter outside constructors is not allowed");
  }
  
  if (mDefaultParams.has(aName)) {
    std::ostringstream lExc;
    lExc << "Parameter " << aName << " is already registered";
    throw CommandParameterRegistrationFailed(lExc.str());
  }

  core::Match lResult = aRule(aDefaultValue);
  if (!lResult.ok) {
    std::ostringstream lExc;
    lExc << "Command " << getId() << " - "
      << "The default value for parameter '" << aName << "'"// "' (" << aDefaultValue << ")"
      << " does not comply with rule '" << aRule << "'"; 
    throw CommandParameterCheckFailed(lExc.str());
  }

  mDefaultParams.add(aName, aDefaultValue);

  mRules[aName] = std::shared_ptr<core::AbstractRule>(new R(aRule));
  
}


// ----------------------------------------------------------------------------
template<typename C>
void Command::addConstraint( const std::string& aName, const C& aConstraint ) {

  if ( getPath() != getId() ) {
    // The Command has already been registered. Parameters list cannot be modified
    throw CommandConstraintRegistrationFailed("Adding constraints outside constructors is not allowed");
  }

  if ( mConstraints.count(aName) ) {
    std::ostringstream lExc;
    lExc << "Command " << getId() << " - " 
      << "Constraint " << aName << " is already registered";
    throw CommandConstraintRegistrationFailed(lExc.str());
  }

  core::Match lMatch = false;
  try {
    lMatch = aConstraint(mDefaultParams);
  } catch (const CommandParameterCheckFailed& lCheckFailed) {
    std::ostringstream lExc;
    lExc << "Command " << getId() << " - " 
         << "Default parameters do not satisfy the requirement for constraint " 
         << aName << "[" << aConstraint << "]" 
         << std::endl;
    lExc << lCheckFailed.what();
    throw CommandConstraintRegistrationFailed(lExc.str());
  }

  if ( !lMatch.ok ) {
    // Complain here
    std::ostringstream lExc;
    lExc << "Default parameters do not comply with constraint " << aName << "[" << aConstraint << "]";
    throw CommandConstraintRegistrationFailed(lExc.str());
    
  }
  
  mConstraints[aName] = std::shared_ptr<C>(new C(aConstraint));
}


// ----------------------------------------------------------------------------
template<typename T>
void Command::addExecutionDetails(const std::string& aId, const T& aInfoItem)
{
  boost::unique_lock<boost::mutex> lLock(mMutex);
  if (mExecutionDetails.has(aId))
    mExecutionDetails.erase(aId);
  mExecutionDetails.add<T>(aId, aInfoItem);
}


} // namespace action
} // namespace swatch

#endif	/* __SWATCH_ACTION_COMMAND_HXX__ */
