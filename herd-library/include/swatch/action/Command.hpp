/**
 * @file    Command.hpp
 * @author  Alessandro Thea, Tom Williams
 * @date    February 2015
 *
 */

#ifndef __SWATCH_ACTION_COMMAND_HPP__
#define __SWATCH_ACTION_COMMAND_HPP__

// C++ Headers
#include <memory>
#include <string>
#include <sys/time.h>                   // for timeval
#include <unordered_map>

// boost headers
#include "boost/thread/pthread/mutex.hpp"  // for mutex

// SWATCH headers
#include "swatch/core/exception.hpp"
#include "swatch/core/ParameterSet.hpp"
#include "swatch/action/Functionoid.hpp"
#include "swatch/action/DefaultCommandParRules.hpp"


namespace swatch {

namespace core {
class ParameterSet;
class AbstractRule;
class PSetConstraint;
}

namespace action {

class ActionableStatus;
class BusyGuard;
class CommandSnapshot;


//! Represents a one-shot action on a resource 
class Command : public Functionoid {
public:

  //! Container for parameter rules
  typedef std::unordered_map<std::string, std::shared_ptr<const core::AbstractRule> > RuleMap_t;

  //! Container for parameter rules
  typedef std::unordered_map<std::string, std::shared_ptr<const core::PSetConstraint> > ConstraintMap_t;

  virtual ~Command();

 /**
   * @brief      Run this command, using the supplied set of parameters
   *
   * @param[in]  aParams         Map of parameter values; any default parameters
   *                             for this command that aren't included in this
   *                             argument will be merged into the set supplied
   *                             to the code method
   * @param[in]  aUseThreadPool  aUseThreadPool Run the command asynchronously
   *                             in the swatch::action::ThreadPool ; if equals
   *                             false, then the command is run synchronously
   */
  void exec( const core::ParameterSet& aParams , bool aUseThreadPool = true );

  /**
   * @brief      Run this command, using the supplied set of parameters, from
   *             another functionoid that already has control of resource
   *
   * @param[in]  aOuterBusyGuard  A outer busy guard
   * @param      aParams          Map of parameter values; any default
   *                              parameters for this command that aren't
   *                              included in this argument will be merged into
   *                              the set supplied to the code method
   * @param      aUseThreadPool   Run the command asynchronously in the
   *                              swatch::action::ThreadPool ; if equals false,
   *                              then the command is run synchronously
   */
  void exec(const BusyGuard* aOuterBusyGuard, const core::ParameterSet& aParams , bool aUseThreadPool = true );

  /**
   * @brief      Returns current state of this command
   *
   * @return     The current state.
   */
  State getState() const;

  /**
   * @brief      Returns snapshot of this command's current status (state flag
   *             value, running time, progress fraction, status message and
   *             result) as a CommandStatus instance
   *
   * @return     A snapshot of the current status.
   */
  CommandSnapshot getStatus() const;

  
  /**
   * @brief      registers a new parameter for this command
   *
   * @param[in]  aName          The name of the new parameter
   * @param[in]  aDefaultValue  Default value for this command
   *
   * @tparam     T              Type of the parameter
   */
  template<typename T, typename R = typename DefaultCmdParRule<T>::type >
  void registerParameter(const std::string& aName, const T& aDefaultValue, R aRule = R()); 

  /**
   * @brief      Gets the default parameters.
   *
   * @return     The default parameters.
   */
  const core::ParameterSet& getDefaultParams() const;

  /**
   * @brief      Gets the default result.
   *
   * @return     The default result.
   */
  const boost::any& getDefaultResult() const;

  const RuleMap_t& getRules() const;

  /**
   * @brief      De-register a parameter
   *
   * @param[in]  aName  Name of the parameter to de-register.
   */
  void unregisterParameter( const std::string aName );

  /**
   * @brief      Adds a new input parameter constraint.
   *
   * @param[in]  aName        Name of the new constraint.
   * @param[in]  aConstraint  Constraint object.
   *
   * @tparam     C            Constraint object class.
   */
  template<typename C>
  void addConstraint( const std::string& aName, const C& aConstraint );


  /**
   * @brief      Removes a parameter constraint.
   *
   * @param[in]  aName  Name of the constraint.
   */
  void removeConstraint( const std::string& aName );

  const ConstraintMap_t& getConstraints() const;

  struct ParamRuleViolation {
    ParamRuleViolation(const std::vector<std::string>& aParams, const std::string& aRuleDescription, const std::string& aDetails);

    std::vector<std::string> parameters;
    //! Static description of the rule/constraint
    std::string ruleDescription;
    //! Details of rule violation from the Match object
    std::string details;
  };

  void checkRulesAndConstraints(const core::ParameterSet& aParams, std::vector<ParamRuleViolation>& aRuleViolations) const;

protected:

  /**
   * @brief      User-defined code for execution
   *
   * @param[in]  aParams  Input parameters.
   *
   * @return     Command execution status.
   */
  virtual State code( const core::ParameterSet& aParams ) = 0;

  /**
   * @brief      Templated command construcotr.
   *
   * @param[in]  aId        Name of the new command.
   * @param      aResource  Reference of the command target.
   * @param[in]  aDefault   Command default result.
   *
   * @tparam     T          Class of the command result object.
   */
  template<typename T>
  Command( const std::string& aId , ActionableObject& aResource, const T& aDefault );

  /**
   * @brief      Templated command construcotr.
   *
   * @param[in]  aId        Name of the new command.
   * @param[in]  aAlias     Alias for the new command (no character restrictions)
   * @param      aResource  Reference of the command target.
   * @param[in]  aDefault   Command default result.
   *
   * @tparam     T          Class of the command result object.
   */
  template<typename T>
  Command( const std::string& aId , const std::string& aAlias, ActionableObject& aResource, const T& aDefault );

  /**
   * @brief      Set command's execution progress.
   *
   * @param[in]  aProgress  Progress level, valid range [0,1]
   */
  void setProgress( float aProgress );

  /**
   * @brief      Set command's execution progress, and status message.
   *
   * @param[in]  aProgress  Progress level, valid range [0,1]
   * @param[in]  aMsg       Status message
   */
  void setProgress( float aProgress, const std::string& aMsg );

  /**
   * @brief      Set value of result from current command execution.
   *
   * @param[in]  aResult  Result object.
   */
  void setResult( const boost::any& aResult );

  /**
   * @brief      Set command's status message
   *
   * @param[in]  aMsg  A message
   */
  void setStatusMsg( const std::string& aMsg );

  /**
   * @brief      Add values to command's "detailed info" ParameterSet for this invocation
   *
   * @param[in]  aId        A identifier
   * @param[in]  aInfoItem  A information item
   *
   * @tparam     T          Info object class.
   */
  template<typename T>
  void addExecutionDetails(const std::string& aId, const T& aInfoItem);

private:

  /**
   * @brief      Reset state, with state set to scheduled, and user-supplied
   *             parameter values used in next execution of command's 'code' method
   *
   * @param[in]  aParams  Parameters that will be used when running command
   */
  void resetForRunning(const core::ParameterSet& aParams);

  /**
   * @brief      Merges a parameter set with the default parameter set. Default
   *             values are only used if not present in params.
   *
   * @param[in]  aParams  Set of external parameters to merge with defaults
   *
   * @return     Merged parameter set
   */
  core::ParameterSet mergeParametersWithDefaults(const core::ParameterSet& aParams) const;

  //! thread safe exception catching wrapper for code()
  void runCode(std::shared_ptr<BusyGuard> aGuard, const core::ParameterSet& aParams );

  //! Reference to the Command's resource status
  ActionableStatus& mActionableStatus;

  //! Set of default parameters
  core::ParameterSet mDefaultParams;

  //! Set of parameters used for running
  core::ParameterSet mRunningParams;

  RuleMap_t mRules;

  ConstraintMap_t mConstraints;

  //! Default command's return value. 
  const boost::any mDefaultResult;

  // Current state of the command
  State mState;

  //! 
  timeval mExecStartTime;
  timeval mExecEndTime;

  //! Progress of the current command.
  float mProgress;

  //! Status string.
  std::string mStatusMsg;

  //! Result of the command execution
  std::shared_ptr<boost::any> mResult;

  //! Detailed information about the command execution
  core::ParameterSet mExecutionDetails;

  //! Mutex for thread safety of read-/write-access of member data
  mutable boost::mutex mMutex;
  };


//! Provides a snapshot of the progress/status of a swatch::action::Command
class CommandSnapshot : public ActionSnapshot {
public:
  CommandSnapshot(const IdAliasPair& aCommand, const IdAliasPair& aActionable, State aState, float aRunningTime, float aProgress, const std::string& aStatusMsg, const core::ParameterSet& aParamSet, const core::ParameterSet& aExecutionDetails, const std::shared_ptr<const boost::any>& aResult);

  //! Returns fractional progress of command; range [0,1]
  float getProgress() const;

  //! Returns command's status message
  const std::string& getStatusMsg() const;

  //! Returns parameter set used for running the command.
  const core::ParameterSet& getParameters() const;

  //! Returns the list of detailed info
  const core::ParameterSet& getExecutionDetails() const;

  //! Returns command's result; equal to NULL if command has not already completed execution
  const boost::any& getResult() const;

private:

  //! Progress status of the command
  float mProgress;

  //! Status message
  std::string mStatusMsg;

  //! List of parameters used to run the command
  core::ParameterSet mParams;

  //! Additional details on the command status
  core::ParameterSet mExecutionDetails;

  //! Result of the command execution
  std::shared_ptr<const boost::any> mResult;
};


SWATCH_DEFINE_EXCEPTION(ParameterNotFound);
SWATCH_DEFINE_EXCEPTION(CommandParameterRegistrationFailed);
SWATCH_DEFINE_EXCEPTION(CommandConstraintRegistrationFailed);
SWATCH_DEFINE_EXCEPTION(CommandParameterCheckFailed);
SWATCH_DEFINE_EXCEPTION(OutOfRange);


} // namespace action
} // namespace swatch


#include "swatch/action/Command.hxx"


#endif /* __SWATCH_ACTION_COMMAND_HPP__ */
