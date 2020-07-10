/**
 * @file    DummyActionableObject.hpp
 * @author  Alessandro Thea
 * @date    February 2015
 *
 */

#ifndef __SWATCH_ACTION_TEST_DUMMYACTIONABLEOBJECT_HPP__
#define __SWATCH_ACTION_TEST_DUMMYACTIONABLEOBJECT_HPP__


#include "swatch/action/ActionableObject.hpp"
#include "swatch/action/Command.hpp"


namespace swatch {
namespace action {
namespace test {

class DummyActionableObject: public ActionableObject {
public:

  class WaitCommand : public Command {
  public:
    WaitCommand(const std::string& aId, ActionableObject& aActionable);
    ~WaitCommand();
  protected:
    State code(const core::ParameterSet& aParams);
  };


  DummyActionableObject(const std::string& aId, const std::string& aAlias = "");

  virtual ~DummyActionableObject();

  virtual const std::vector<std::string>& getGateKeeperContexts() const;

  uint32_t getNumber() const;

  void setNumber(uint32_t aNumber);

  // Expose registerFunctionoid template method as public for tests
  template< typename T>
  T& registerCommand( const std::string& aId )
  {
    return ActionableObject::registerCommand<T>(aId);
  }

  // Expose registerCommandSequence method as public for tests
  CommandSequence& registerSequence(const std::string& aId, const std::string& aFirstCmdId);

  // Expose registerCommandSequence method as public for tests
  CommandSequence& registerSequence(const std::string& aId, Command& aFirstCmd);

  // Expose registerStateMachine method as public for tests
  StateMachine& registerStateMachine(const std::string& aId, const std::string& aInitialState, const std::string& aErrorState);

  //! Tells object to wait each time the "wait" method is called; e.g. in WaitCommand
  void pleaseWait();
  //! Returns id of thread that's currently waiting; if no thread is currently executing the "wait" method
  boost::thread::id getWaitingThread();

  //! Tells other thread in "wait" method to
  void pleaseContinue();

private:

  void wait() const;

  uint32_t mNumber;

  bool mWait;
  mutable bool mContinue;
  mutable boost::thread::id mWaitingThread;
  mutable boost::mutex mMutex;
  mutable boost::condition_variable mConditionVar;

  mutable std::vector<std::string> mGateKeeperTables;

  DummyActionableObject( const DummyActionableObject& ); // non copyable
  DummyActionableObject& operator=( const DummyActionableObject& ); // non copyable
};


} /* namespace test */
} /* namespace action */
} /* namespace swatch */

#endif /* SWATCH_ACTION_TEST_DUMMYACTIONABLEOBJECT_HPP */
