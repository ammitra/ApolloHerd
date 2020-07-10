/**
 * @file    DummyCommand.hpp
 * @author  Tom Williams
 * @date    August 2015
 *
 */

#ifndef __SWATCH_ACTION_TEST_DUMMYCOMMAND_HPP__
#define __SWATCH_ACTION_TEST_DUMMYCOMMAND_HPP__


#include "swatch/action/Command.hpp"
#include "swatch/action/ActionableObject.hpp"


namespace swatch {
namespace action {
namespace test {
  
//! Dummy constraint class, to be used only in unit tests; all instances of this class return the same value, which can be set by the static 'setResult' method
class DummyConstraint : public core::PSetConstraint {
public:
  DummyConstraint(const std::map<std::string, std::type_index>& aParameters);
  ~DummyConstraint() {}

  static void setResult(const core::Match& aResult) { sResult = aResult; }

  static const std::string sDescription;
  
private:
  core::Match verify(const core::ParameterSet& aParams) const { return sResult; }

  void describe(std::ostream& aStream) const { aStream << sDescription; }

  static core::Match sResult;
};



//! Dummy parameter rule class, to be used only in unit tests; all instances of this class return the same value, which can be set by the static 'setResult' method
template<typename T>
class DummyRule : public core::Rule<T> {
public:
  DummyRule() {}
  ~DummyRule() {}

  static void setResult(const core::Match& aResult) { sResult = aResult; }

  static const std::string sDescription;
  
private:
  virtual core::Match verify( const T& aValue ) const { return sResult; }

  virtual void describe(std::ostream& aStream) const { aStream << sDescription; }

  static core::Match sResult;
};

template <typename T>
const std::string DummyRule<T>::sDescription = "Dummy rule for unit tests";

template <typename T>
core::Match DummyRule<T>::sResult = true;



//! Dummy command whose code method is used to test the implementation of the command base class
class DummyCommand : public Command {
public:
  DummyCommand( const std::string& aId, ActionableObject& aActionable );
  virtual ~DummyCommand();

  //! Sets the detailed info that will be registered by the code method the next time that it is run
  void setExecutionDetails(const core::ParameterSet& aInfoSet);

  static const boost::any kDefaultResult;
  static const std::string kParamX;
  static const std::string kParamToDo;
  static const std::string kFinalMsgUseResource;
  static const std::string kFinalMsgSleep;

private:

  virtual Command::State code( const core::ParameterSet& aParams);

  core::ParameterSet mNextExecutionDetails;
};


//! Dummy command written for unit tests; code method is a no-op that returns warning after setting status.
class DummyWarningCommand : public Command {
public:
  explicit DummyWarningCommand(const std::string& aId, ActionableObject& aActionable);
  virtual ~DummyWarningCommand();

  //! Sets the detailed info that will be registered by the code method the next time that it is run
  void setExecutionDetails(const core::ParameterSet& aInfoSet);

  virtual Command::State code(const core::ParameterSet& aParams);

  static const boost::any kDefaultResult;
  static const std::string kFinalMsg;

private:
  core::ParameterSet mNextExecutionDetails;
};


//! Dummy command written for unit tests; code method is a no-op that returns error after setting status.
class DummyErrorCommand : public Command {
public:
  explicit DummyErrorCommand(const std::string& aId, ActionableObject& aActionable);
  virtual ~DummyErrorCommand();

  //! Sets the detailed info that will be registered by the code method the next time that it is run
  void setExecutionDetails(const core::ParameterSet& aInfoSet);

  virtual Command::State code(const core::ParameterSet& aParams);

  static const boost::any kDefaultResult;
  static const std::string kFinalMsg;
  static const float kFinalProgress;

private:
  core::ParameterSet mNextExecutionDetails;
};


//! Dummy command written for unit tests; code method is a no-op that throws after setting progress.
class DummyThrowCommand : public Command {
public:
  explicit DummyThrowCommand(const std::string& aId, ActionableObject& aActionable);
  virtual ~DummyThrowCommand();

  //! Sets the detailed info that will be registered by the code method the next time that it is run
  void setExecutionDetails(const core::ParameterSet& aInfoSet);

  virtual Command::State code(const core::ParameterSet& aParams);

  static const boost::any kDefaultResult;
  static const std::string kExceptionMsg;
  static const float kFinalProgress;

private:
  core::ParameterSet mNextExecutionDetails;
};
} /* namespace test */
} /* namespace action */
} /* namespace swatch */

#endif /* SWATCH_ACTION_TEST_DUMMYCOMMAND_HPP */
