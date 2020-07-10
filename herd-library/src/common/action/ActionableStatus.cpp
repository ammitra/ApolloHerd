
#include "swatch/action/ActionableStatus.hpp"


// Standard C++ headers
#include <assert.h>
#include <stdexcept>

// boost headers
#include "boost/iterator/indirect_iterator.hpp"
#include "boost/iterator/iterator_facade.hpp"  // for operator==, etc
#include "boost/lexical_cast.hpp"
#include "boost/thread/lock_algorithms.hpp"  // for lock
#include "boost/thread/lock_options.hpp"  // for adopt_lock_t
#include "boost/thread/lock_types.hpp"  // for unique_lock
#include "boost/thread/pthread/condition_variable.hpp"
#include "boost/thread/pthread/mutex.hpp"  // for mutex



namespace swatch {
namespace action {


// STATIC MEMBERS INITIALIZATION
const std::string ActionableSnapshot::kNullStateMachineId = "";
const std::string ActionableSnapshot::kNullStateId = "";


//------------------------------------------------------------------------------------
ActionableSnapshot::ActionableSnapshot() :
  mAlive(true),
  mStateMachineId(kNullStateMachineId),
  mState(kNullStateId)
{
}


//------------------------------------------------------------------------------------
bool ActionableSnapshot::isAlive() const
{
  return mAlive;
}


//------------------------------------------------------------------------------------
bool ActionableSnapshot::isEngaged() const
{
  return mStateMachineId != kNullStateMachineId;
}


//------------------------------------------------------------------------------------
bool ActionableSnapshot::isRunning() const
{
  return !mRunningActions.empty();
}


//------------------------------------------------------------------------------------
const std::string& ActionableSnapshot::getState() const
{
  return mState;
}

//------------------------------------------------------------------------------------
const Functionoid* ActionableSnapshot::getLastRunningAction() const
{
  if (mRunningActions.empty())
    return NULL;
  else
    return mRunningActions.back();
}


//------------------------------------------------------------------------------------
const std::vector<const Functionoid*>& ActionableSnapshot::getRunningActions() const
{
  return mRunningActions;
}


//------------------------------------------------------------------------------------
const std::string& ActionableSnapshot::getStateMachineId() const
{
  return mStateMachineId;
}




ActionableStatusGuard::ActionableStatusGuard(const ActionableStatus& aStatus) :
  mStatus(aStatus),
  mLockGuard(aStatus.mMutex)
{
}

//------------------------------------------------------------------------------------
ActionableStatusGuard::ActionableStatusGuard(const ActionableStatus& aStatus, boost::adopt_lock_t) :
  mStatus(aStatus),
  mLockGuard(aStatus.mMutex, boost::adopt_lock_t())
{
}

//------------------------------------------------------------------------------------
ActionableStatusGuard::~ActionableStatusGuard()
{
}

//------------------------------------------------------------------------------------
bool ActionableStatusGuard::isCorrectGuard(const ActionableStatus& aStatus) const
{
  return (&mStatus == &aStatus);
}



//------------------------------------------------------------------------------------
ActionableStatus::ActionableStatus() :
  mStatus()
{
}

//------------------------------------------------------------------------------------
ActionableStatus::~ActionableStatus()
{
}

//------------------------------------------------------------------------------------
ActionableSnapshot ActionableStatus::getSnapshot(const ActionableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mStatus;
}

//------------------------------------------------------------------------------------
bool ActionableStatus::isAlive(const ActionableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mStatus.isAlive();
}

//------------------------------------------------------------------------------------
bool ActionableStatus::isEngaged(const ActionableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mStatus.isEngaged();
}

//------------------------------------------------------------------------------------
const std::string& ActionableStatus::getStateMachineId(const ActionableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mStatus.getStateMachineId();
}

//------------------------------------------------------------------------------------
const std::string& ActionableStatus::getState(const ActionableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mStatus.getState();
}

//------------------------------------------------------------------------------------
bool ActionableStatus::isBusy(const ActionableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mStatus.isRunning();
}

//------------------------------------------------------------------------------------
const std::vector<const Functionoid*>& ActionableStatus::getRunningActions(const ActionableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mStatus.getRunningActions();
}

//------------------------------------------------------------------------------------
const Functionoid* ActionableStatus::getLastRunningAction(const ActionableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mStatus.getLastRunningAction();
}


//------------------------------------------------------------------------------------
void ActionableStatus::setStateMachine(const std::string& aStateMachine, const std::string& aState, const ActionableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);
  mStatus.mStateMachineId = aStateMachine;
  mStatus.mState = aState;
}

//------------------------------------------------------------------------------------
void ActionableStatus::setNoStateMachine(const ActionableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);
  mStatus.mStateMachineId = ActionableSnapshot::kNullStateMachineId;
  mStatus.mState = ActionableSnapshot::kNullStateId;
}

//------------------------------------------------------------------------------------
void ActionableStatus::setState(const std::string& aState, const ActionableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);
  mStatus.mState = aState;
}

//------------------------------------------------------------------------------------
void ActionableStatus::addAction(const Functionoid& aAction, const ActionableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);
  mStatus.mRunningActions.push_back(&aAction);
}

//------------------------------------------------------------------------------------
void ActionableStatus::popAction(const ActionableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);
  mStatus.mRunningActions.pop_back();
  if ( mStatus.mRunningActions.empty() )
    mConditionVar.notify_all();
}

//------------------------------------------------------------------------------------
void ActionableStatus::kill(const ActionableStatusGuard& aGuard)
{
  throwIfWrongGuard(aGuard);
  mStatus.mAlive = false;
}


boost::unique_lock<boost::mutex>& ActionableStatus::getUniqueLock(ActionableStatusGuard& aGuard) const
{
  return aGuard.mLockGuard;
}


//------------------------------------------------------------------------------------
void ActionableStatus::throwIfWrongGuard(const ActionableStatusGuard& aGuard) const
{
  if ( ! aGuard.isCorrectGuard(*this) )
    throw IncorrectActionableGuard("");
}


} // end ns action
} // end ns swatch
