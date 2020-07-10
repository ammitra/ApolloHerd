/*
 * File:   ActionableStatus.hpp
 * Author: tom
 *
 * Created on 16 November 2015, 14:08
 */

#ifndef __SWATCH_ACTION_ACTIONABLESTATUS_HPP__
#define __SWATCH_ACTION_ACTIONABLESTATUS_HPP__


// STL headers
#include <iterator>                     // for iterator_traits
#include <memory>
#include <stddef.h>                     // for size_t, NULL
#include <string>
#include <utility>                      // for pair
#include <vector>

// boost headers
#include "boost/static_assert.hpp"
#include "boost/thread/pthread/condition_variable_fwd.hpp"
#include "boost/type_traits/is_base_of.hpp"
#include "boost/type_traits/is_convertible.hpp"  // for is_convertible

// SWATCH headers
#include "swatch/core/exception.hpp"


namespace swatch {
namespace action {

class Functionoid;
class ActionableStatus;


//! Represents the current aggregate state (engaged FSM, FSM state, running actions, ...) of an ActionableObject/ActionableSystem
class ActionableSnapshot {
public:

  ActionableSnapshot();

  /**
   * @brief      The object is alive i.e. it is not bound for destruction
   *
   * @return     True if alive
   */
  bool isAlive() const;

  /**
   * @brief      The object is engaged with a State Machine
   *
   * @return     True of engaged
   */
  bool isEngaged() const;

  /**
   * @brief      One or more actions are currently running
   *
   * @return     True if one or more actions/functionoid are running
   */
  bool isRunning() const;

  //!
  const std::string& getState() const;

  //!
  const std::vector<const Functionoid*>& getRunningActions() const;


  const Functionoid* getLastRunningAction() const;

  //!
  template<class T>
  const T* getFirstRunningActionOfType() const;

  const std::string& getStateMachineId() const;

  static const std::string kNullStateMachineId;

  static const std::string kNullStateId;

private:

  //! Indicates whether or not actions are allowed on this resource anymore (actions become disabled once the deleter is)
  bool mAlive;

  std::string mStateMachineId;

  std::string mState;

  //! Indicates which functionoids (Command/CommandSequence/(System)Transition) are currently active; NULL value indicates that no functionoids are currently active.
  std::vector<const Functionoid*> mRunningActions;

  friend class ActionableStatus;
};


template<class T>
const T* ActionableSnapshot::getFirstRunningActionOfType() const
{
  BOOST_STATIC_ASSERT( (boost::is_base_of<swatch::action::Functionoid,T>::value) );

  for (std::vector<const Functionoid*>::const_iterator lIt=mRunningActions.begin(); lIt!=mRunningActions.end(); lIt++) {
    if (const T* lAction = dynamic_cast<const T*>(*lIt) )
      return lAction;
  }

  return NULL;
}


class ActionableStatus;

class ActionableStatusGuard {
public:
  ActionableStatusGuard() = delete;
  ActionableStatusGuard(const ActionableStatusGuard&) = delete;
  ActionableStatusGuard& operator=(const ActionableStatusGuard& aGuard) = delete;

  ActionableStatusGuard(const ActionableStatus& aStatus);
  ActionableStatusGuard(const ActionableStatus& aStatus, boost::adopt_lock_t);
  virtual ~ActionableStatusGuard();

  bool isCorrectGuard(const ActionableStatus& aStatus) const;

private:
  const ActionableStatus& mStatus;
  boost::unique_lock<boost::mutex> mLockGuard;

  friend class ActionableStatus;
};


SWATCH_DEFINE_EXCEPTION(IncorrectActionableGuard)


class ActionableStatus {
public:
  ActionableStatus();
  ~ActionableStatus();

  ActionableSnapshot getSnapshot(const ActionableStatusGuard& aGuard) const;

  bool isAlive(const ActionableStatusGuard& aGuard) const;

  bool isEngaged(const ActionableStatusGuard& aGuard) const;

  const std::string& getStateMachineId(const ActionableStatusGuard& aGuard) const;

  const std::string& getState(const ActionableStatusGuard& aGuard) const;

  bool isBusy(const ActionableStatusGuard& aGuard) const;

  const std::vector<const Functionoid*>& getRunningActions(const ActionableStatusGuard& aGuard) const;

  const Functionoid* getLastRunningAction(const ActionableStatusGuard& aGuard) const;

  /**
   * @brief      Gets the first running action of a given type.
   *
   * @param[in]  aGuard  A guard
   *
   * @tparam     T       { description }
   *
   * @return     The first running action of type.
   */
  template<class T>
  const T* getFirstRunningActionOfType(const ActionableStatusGuard& aGuard) const;

  void setStateMachine(const std::string& aStateMachine, const std::string& aState, const ActionableStatusGuard& aGuard);

  void setNoStateMachine(const ActionableStatusGuard& aGuard);

  void setState(const std::string& aState, const ActionableStatusGuard& aGuard);

  void addAction(const Functionoid& aAction, const ActionableStatusGuard& aGuard);

  void popAction(const ActionableStatusGuard& aGuard);

  //! Disables all future actions from running on this resource
  void kill(const ActionableStatusGuard& aGuard);

private:

  boost::unique_lock<boost::mutex>& getUniqueLock(ActionableStatusGuard& aGuard) const;

  //! Throws if guard is not for this status instance
  void throwIfWrongGuard(const ActionableStatusGuard& aGuard) const;

  mutable boost::mutex mMutex;
  ActionableSnapshot mStatus;
  boost::condition_variable mConditionVar;

  friend class ActionableStatusGuard;
};


template<class T>
const T* ActionableStatus::getFirstRunningActionOfType(const ActionableStatusGuard& aGuard) const
{
  throwIfWrongGuard(aGuard);
  return mStatus.getFirstRunningActionOfType<T>();
}


} //end ns: action
} //end ns: swatch


#endif  /* __SWATCH_ACTION_ACTIONABLESTATUS_HPP__ */

