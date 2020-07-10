#include "swatch/action/Command.hpp"


// Standard headers
#include <stddef.h>                     // for NULL
#include <exception>                    // for exception
#include <set>                          // for set
#include <sstream>                      // for operator<<, basic_ostream, etc
#include <stdexcept>                    // for out_of_range
#include <typeinfo>                     // for type_info

// boost headers
#include "boost/lexical_cast.hpp"
#include "boost/thread/lock_types.hpp"

// SWATCH headers
#include "swatch/action/ActionableObject.hpp"
#include "swatch/action/BusyGuard.hpp"
#include "swatch/action/ThreadPool.hpp"
#include "swatch/core/utilities.hpp"
#include "swatch/core/Match.hpp"

// log4cplus headers
#include <log4cplus/loggingmacros.h>



namespace swatch {
namespace action {


//------------------------------------------------------------------------------------
Command::~Command()
{
}


//------------------------------------------------------------------------------------
void
Command::exec( const core::ParameterSet& aParams, bool aUseThreadPool )
{
  exec(NULL, aParams, aUseThreadPool);
}


//------------------------------------------------------------------------------------
void
Command::exec(const BusyGuard* aOuterBusyGuard, const core::ParameterSet& aParams, bool aUseThreadPool )
{
  
  // Merge user-supplied parameter values with default values
  core::ParameterSet lRunningParams = mergeParametersWithDefaults(aParams);

  // Validate the new running parameters against rules and constraints
  std::vector<ParamRuleViolation> lRuleViolations;
  checkRulesAndConstraints(lRunningParams, lRuleViolations);
  if (!lRuleViolations.empty()) {
    std::ostringstream lMsgStream;
    lMsgStream << "Parameters [" << core::join(lRuleViolations.at(0).parameters, ", ") << "] failed rule/constraint '"
            << lRuleViolations.at(0).ruleDescription << "': " << lRuleViolations.at(0).details;
    throw CommandParameterCheckFailed(lMsgStream.str());
  }

  std::shared_ptr<BusyGuard> lBusyGuard(new BusyGuard(*this, mActionableStatus, aOuterBusyGuard));

  // Reset the command's state before doing anything else
  resetForRunning(lRunningParams);

  // Execute the command protected by a very generic try/catch
  try {
    // if threadpool is to be used
    if ( aUseThreadPool ) {
      boost::unique_lock<boost::mutex> lock(mMutex);
      mState = kScheduled;

      ThreadPool& pool = ThreadPool::getInstance();
      pool.addTask<Command,BusyGuard>(this, &Command::runCode, lBusyGuard, mRunningParams);
    }
    else {
      // otherwise execute in same thread
      this->runCode(lBusyGuard, mRunningParams);
    }
  }
  catch ( const std::exception& e ) {
    LOG4CPLUS_ERROR(getActionable().getLogger(), "Exception thrown when executing command '" << getId() << "' : " << ( e.what() != 0x0 ? e.what() : "" ));

    // Then rethrow the exception on to the higher layers of code.
    throw;
  }
}


//------------------------------------------------------------------------------------
void Command::runCode(std::shared_ptr<BusyGuard> aActionGuard, const core::ParameterSet& aParams)
{
  // 1) Declare that I'm running
  {
    boost::unique_lock<boost::mutex> lock(mMutex);
    mState = kRunning;
    gettimeofday(&mExecStartTime, NULL);
  }

  // 2) Run the code, handling any exceptions
  try {
    State s = this->code(aParams);

    boost::unique_lock<boost::mutex> lock(mMutex);
    gettimeofday(&mExecEndTime, NULL);
    switch (s) {
      case State::kWarning :
        LOG4CPLUS_WARN(getActionable().getLogger(), "Command '" << getId() << "' returned warning. Last status message: " << mStatusMsg);
        mState = s;
        mProgress = 1.0;
        break;
      case State::kDone :
        LOG4CPLUS_INFO(getActionable().getLogger(), "Command '" << getId() << "' completed successfully. Last status message: " << mStatusMsg);
        mState = s;
        mProgress = 1.0;
        break;
      case State::kError :
        LOG4CPLUS_ERROR(getActionable().getLogger(), "Command '" << getId() << "' returned error. Progress = " << mProgress << "; last status message: " << mStatusMsg);
        mState = s;
        break;
      default :
        mState = State::kError;
        mStatusMsg = "Command::code() method returned invalid Status enum value '" + boost::lexical_cast<std::string>(s) + "'   \n Original status message was: " + mStatusMsg;
        LOG4CPLUS_ERROR(getActionable().getLogger(), "Command '" << getId() << "' : " << mStatusMsg);
    }
  }
  catch (const std::exception& e) {
    boost::unique_lock<boost::mutex> lock(mMutex);
    gettimeofday(&mExecEndTime, NULL);
    mState = State::kError;
    mStatusMsg = std::string("An exception of type '" + core::demangleName(typeid(e).name()) + "' was thrown in Command::code(): ") + e.what();
    LOG4CPLUS_ERROR(getActionable().getLogger(), "Command '" << getId() << " : " << mStatusMsg);
  }

  // 3) Release control of the resource - by destruction of the ActionGuard.
}


//------------------------------------------------------------------------------------
void Command::resetForRunning(const core::ParameterSet& aParams)
{
  boost::unique_lock<boost::mutex> lock(mMutex);
  mState = kScheduled;
  mProgress = 0.0;
  mStatusMsg = "";

  mExecutionDetails = core::ParameterSet();
  mResult.reset(new boost::any(mDefaultResult));

  mRunningParams = aParams;
}


//------------------------------------------------------------------------------------
Functionoid::State Command::getState() const
{
  boost::unique_lock<boost::mutex> lock(mMutex);
  return mState;
}


//------------------------------------------------------------------------------------
CommandSnapshot Command::getStatus() const
{
  boost::unique_lock<boost::mutex> lock(mMutex);

  // Only let user see the result once the command has completed (since its contents can change before then) ...
  std::shared_ptr<const boost::any> result(new boost::any());
  if ( (mState == kDone) || (mState == kWarning) || (mState == kError))
    result = mResult;

  float runningTime = 0.0;
  switch (mState) {
    case kInitial :
    case kScheduled :
      break;
    default:
      timeval endTime;
      if (mState == kRunning)
        gettimeofday(&endTime, NULL);
      else
        endTime = mExecEndTime;

      runningTime = double(endTime.tv_sec - mExecStartTime.tv_sec);
      runningTime += float(double(endTime.tv_usec) - double(mExecStartTime.tv_usec))/1e6;
      break;
  }

  return CommandSnapshot(IdAliasPair(*this), IdAliasPair(getActionable()), mState, runningTime, mProgress, mStatusMsg, mRunningParams, mExecutionDetails, result);
}


//------------------------------------------------------------------------------------
void
Command::setProgress(float aProgress)
{
  if ( aProgress < 0. or aProgress > 1.) {
    std::ostringstream err;
    err << "Progress must be in the [0.,1.] range. " << aProgress;
    // TODO: should this not throw a SWATCH exception?
    throw OutOfRange(err.str());
  }

  boost::unique_lock<boost::mutex> lock(mMutex);
  mProgress = aProgress;
}


//------------------------------------------------------------------------------------
void
Command::setProgress(float aProgress, const std::string& aMsg )
{
  if ( aProgress < 0. or aProgress > 1.) {
    std::ostringstream lExc;
    lExc << "Progress must be in the [0.,1.] range. " << aProgress;
    throw OutOfRange(lExc.str());
  }

  boost::unique_lock<boost::mutex> lock(mMutex);
  mProgress = aProgress;
  mStatusMsg = aMsg;

  LOG4CPLUS_DEBUG(getActionable().getLogger(), "Command '" << getId() << "' : " << aMsg);
}


//------------------------------------------------------------------------------------
void Command::setResult( const boost::any& aResult )
{
  boost::unique_lock<boost::mutex> lock(mMutex);
  mResult.reset(new boost::any(aResult));
}


//------------------------------------------------------------------------------------
void Command::setStatusMsg(const std::string& aMsg)
{
  boost::unique_lock<boost::mutex> lock(mMutex);
  mStatusMsg = aMsg;

  LOG4CPLUS_DEBUG(getActionable().getLogger(), "Command '" << getId() << "' : " << aMsg);
}


//------------------------------------------------------------------------------------
const Command::RuleMap_t& Command::getRules() const
{
  return mRules;
}


//------------------------------------------------------------------------------------
void Command::unregisterParameter(const std::string aName)
{
  if ( getPath() != getId() ) {
    // The Command has already been registered. Parameters list cannot be modified
    throw CommandParameterRegistrationFailed("Revoking parameter outside constructors is not allowed");
  }

  for ( const auto& lConstr : mConstraints ) {
    std::set<std::string> lDependants = lConstr.second->getParameterNames();
    if( std::find(lDependants.begin(), lDependants.end(), aName) == lDependants.end() )
      continue;
    std::ostringstream lExc;
    lExc << "Command '" << getId() << "' - Parameter '" << aName << "' is used by constraint "
         << lConstr.first << "[" << *(lConstr.second) << "]. The constraint must be removed before the parameter.";

    throw CommandParameterRegistrationFailed(lExc.str());
  }

  mDefaultParams.erase(aName);
  mRules.erase(aName);
}


//------------------------------------------------------------------------------------
void Command::removeConstraint(const std::string& aName)
{
  if ( getPath() != getId() ) {
    // The Command has already been registered. Parameters list cannot be modified
    throw CommandConstraintRegistrationFailed("Revoking constraints outside constructors is not allowed");
  }

  if ( mConstraints.erase(aName) == 0 ) {
    throw CommandConstraintRegistrationFailed("Constraint removal failed: Constraint '" + aName + "' not found");
  }
}


//------------------------------------------------------------------------------------
const Command::ConstraintMap_t& Command::getConstraints() const
{
  return mConstraints;
}


//------------------------------------------------------------------------------------
Command::ParamRuleViolation::ParamRuleViolation(const std::vector<std::string>& aParams, const std::string& aDescription, const std::string& aDetails) :
  parameters(aParams),
  ruleDescription(aDescription),
  details(aDetails)
{
}

//------------------------------------------------------------------------------------
void Command::checkRulesAndConstraints(const core::ParameterSet& aParams, std::vector<ParamRuleViolation>& aRuleViolations) const
{
  for ( const auto& lParName : aParams.keys() ) {
    const boost::any& lPar = aParams[lParName];
    const core::AbstractRule& lRule = *mRules.at(lParName);
    if ( ! (lPar.type() == lRule.type()) ) {
      throw core::InvalidArgument("C++ type '" + core::demangleName(lPar.type().name()) + "' of parameter '" + lParName + "' received for command '" + this->getActionable().getId() + "." + getId() + "'' does not match registered type '" + core::demangleName(lRule.type().name()) + "'");
    }
  
    core::Match lResult = lRule(lPar);
    if ( ! lResult.ok )
      aRuleViolations.push_back( ParamRuleViolation({lParName}, boost::lexical_cast<std::string>(lRule), lResult.details) );
  }

  for ( const auto& lConstrPair : mConstraints ) {

    core::Match lResult = (*lConstrPair.second)(aParams);
    if ( lResult.ok )
      continue;

    aRuleViolations.push_back( ParamRuleViolation({}, boost::lexical_cast<std::string>(*lConstrPair.second), lResult.details) );
    for ( const std::string& lParName : lConstrPair.second->getParameterNames() )
      aRuleViolations.back().parameters.push_back(lParName);
  }
}


//------------------------------------------------------------------------------------
const core::ParameterSet& Command::getDefaultParams() const
{
  return mDefaultParams;
}


//------------------------------------------------------------------------------------
const boost::any& Command::getDefaultResult() const
{
  return mDefaultResult;
}


//------------------------------------------------------------------------------------
core::ParameterSet Command::mergeParametersWithDefaults( const core::ParameterSet& aParams ) const
{
  core::ParameterSet merged = core::ParameterSet(aParams);

  std::set<std::string> keys = mDefaultParams.keys();
  for(const std::string& k : keys) {
    if ( ! merged.has(k))
      merged.adopt(k, mDefaultParams);
  }

  return merged;
}


//------------------------------------------------------------------------------------
CommandSnapshot::CommandSnapshot(const IdAliasPair& aCommand, const IdAliasPair& aActionable, State aState, float aRunningTime, float aProgress, const std::string& aStatusMsg, const core::ParameterSet& aParams, const core::ParameterSet& aExecutionDetails, const std::shared_ptr<const boost::any>& aResult) :
  ActionSnapshot(aCommand, aActionable, aState, aRunningTime),
  mProgress(aProgress),
  mStatusMsg(aStatusMsg),
  mParams(aParams),
  mExecutionDetails(aExecutionDetails),
  mResult(aResult)
{
}


//------------------------------------------------------------------------------------
float
CommandSnapshot::getProgress() const
{
  return mProgress;
}


//------------------------------------------------------------------------------------
const
std::string& CommandSnapshot::getStatusMsg() const
{
  return mStatusMsg;
}


//------------------------------------------------------------------------------------
const core::ParameterSet& CommandSnapshot::getParameters() const
{
  return mParams;
}


//------------------------------------------------------------------------------------
const core::ParameterSet& CommandSnapshot::getExecutionDetails() const
{
  return mExecutionDetails;
}

//------------------------------------------------------------------------------------
const boost::any& CommandSnapshot::getResult() const
{
  return *mResult;
}


} // namespace action
} // namespace swatch
