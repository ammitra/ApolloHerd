/**
 * @file    Functionoid.hpp
 * @author  arose
 * @brief
 * @date    May 2015
 *
 */

#ifndef __SWATCH_ACTION_FUNCTIONOID_HPP__
#define __SWATCH_ACTION_FUNCTIONOID_HPP__


#include <iosfwd>                       // for ostream
#include <string>                       // for string

// SWATCH headers
#include "swatch/core/exception.hpp"
#include "swatch/core/LeafObject.hpp"


namespace swatch {
namespace action {


class ActionableObject;

class Functionoid : public core::LeafObject {
protected:
  /// Constructor
  Functionoid( const std::string& aId, const std::string& aAlias, ActionableObject& aDevice );

public:
  /// Destructor
  virtual ~Functionoid();

  //! Represents state of action's execution
  enum State {
    kInitial,
    kScheduled,
    kRunning,
    kError,
    kWarning,
    kDone
  };

  struct IdAliasPair {
    IdAliasPair(const std::string& aPath, const std::string& aAlias) :
      idPath(aPath),
      alias(aAlias)
    {}

    IdAliasPair(const LeafObject& aObj) :
      idPath(aObj.getPath()),
      alias(aObj.getAlias())
    {}

    std::string idPath, alias;
  };

  const ActionableObject& getActionable() const;

  ActionableObject& getActionable();

  template<typename T> const T& getActionable() const ;

  template<typename T> T& getActionable();

private:
  ActionableObject& mActionable;
};


std::ostream& operator<<(std::ostream& aStream, swatch::action::Functionoid::State aState);


//! Abstract base class for snapshots of the progress/status of an action (e.g. Command, Transition)
class ActionSnapshot {
public:
  typedef Functionoid::State State;
  typedef Functionoid::IdAliasPair IdAliasPair;

protected:
  ActionSnapshot(const IdAliasPair& aAction, const IdAliasPair& aActionable, State aState, float aRunningTime);

public:
  virtual ~ActionSnapshot();

  //! Returns the path of the action (command, sequence, transition, ...)
  const std::string& getActionPath() const;

  //! Returns the ID string of the action
  std::string getActionId() const;

  //! Returns the ID string of the actionable
  const std::string& getActionableId() const;

  //! Returns state of execution (scheduled, running, done/warning/error)
  State getState() const;

  //! Returns action's running time in seconds
  float getRunningTime() const;

  //! Returns fractional progress of command; range [0,1]
  virtual float getProgress() const = 0;

private:
  std::string mPath;
  std::string mAlias;
  std::string mActionableId;
  std::string mActionableAlias;
  State mState;
  float mRunningTime;
};


SWATCH_DEFINE_EXCEPTION(InvalidResource)

} /* namespace action */
} /* namespace swatch */


#include "swatch/action/Functionoid.hxx"

#endif /* __SWATCH_ACTION_FUNCTIONOID_HPP__ */
