/*
 * TestCommand.cpp
 *
 *  Created on: August 2015
 *      Author: Tom Williams
 */

#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>

// SWATCH headers
#include "swatch/action/Command.hpp"
#include "swatch/action/test/DummyCommand.hpp"
#include "swatch/action/test/DummyActionableObject.hpp"


using namespace swatch::core;


namespace swatch {
namespace action {
namespace test {

struct CommandTestSetup {
  CommandTestSetup();
  ~CommandTestSetup() = default;

  boost::shared_ptr<DummyActionableObject> obj;
  Command& cmd;
  Command& warningCmd;
  Command& errorCmd;
  Command& throwCmd;
  core::ParameterSet params;
};

CommandTestSetup::CommandTestSetup():
  obj( new DummyActionableObject("dummyObj"), ActionableObject::Deleter() ),
  cmd( obj->registerCommand<DummyCommand>("dummy_cmd") ),
  warningCmd( obj->registerCommand<DummyWarningCommand>("dummy_warning_cmd") ),
  errorCmd( obj->registerCommand<DummyErrorCommand>("dummy_error_cmd") ),
  throwCmd( obj->registerCommand<DummyThrowCommand>("dummy_throw_cmd") )
{
}

//CommandTestSetup::~CommandTestSetup()
//{
//}


BOOST_AUTO_TEST_SUITE(CommandTestSuite)


BOOST_AUTO_TEST_CASE(TestConstruction)
{
  DummyActionableObject obj("dummy");
  Command& cmd = obj.registerCommand<DummyCommand>("myTestCommand");

  BOOST_CHECK_EQUAL(cmd.getId(), "myTestCommand");
  BOOST_CHECK_EQUAL(cmd.getPath(), obj.getId()+"."+cmd.getId());
  BOOST_CHECK_EQUAL(&obj.getCommand(cmd.getId()), &cmd);
  BOOST_CHECK_EQUAL(&obj.getObj(cmd.getId()), (Object*) &cmd);

  BOOST_CHECK_EQUAL(&cmd.getActionable(), &obj);
}


BOOST_FIXTURE_TEST_CASE(TestDefaultParams, CommandTestSetup)
{
  BOOST_CHECK_EQUAL(cmd.getDefaultParams().get<int>("x") , 15);
  BOOST_CHECK_EQUAL(cmd.getDefaultParams().get<std::string>("todo") , "");
}



BOOST_FIXTURE_TEST_CASE(TestUnregisterThrows, CommandTestSetup)
{
  BOOST_CHECK_THROW(cmd.unregisterParameter("dummy_cmd"), CommandParameterRegistrationFailed);
}


BOOST_FIXTURE_TEST_CASE(TestCommandInitialState,  CommandTestSetup)
{
  const std::vector<const Command*> cmds = { &cmd, &warningCmd, &errorCmd, &throwCmd};

  for (auto it=cmds.begin(); it != cmds.end(); it++) {
    BOOST_CHECK_EQUAL( (*it)->getState(), Functionoid::kInitial );

    CommandSnapshot s = (*it)->getStatus();
    BOOST_CHECK_EQUAL(s.getActionPath(), obj->getPath() + "." + (*it)->getId());
    BOOST_CHECK_EQUAL(s.getActionId(), (*it)->getId());
    BOOST_CHECK_EQUAL(s.getActionableId(), obj->getId());
    BOOST_CHECK_EQUAL(s.getState(), Functionoid::kInitial);
    BOOST_CHECK_EQUAL(s.getProgress(), 0.0);
    BOOST_CHECK_EQUAL(s.getRunningTime(), 0.0);
    BOOST_CHECK_EQUAL(s.getStatusMsg(), "initialised");
    BOOST_CHECK_EQUAL(s.getParameters().size(), size_t(0));
    BOOST_CHECK_EQUAL(s.getResult().empty(), true);
    BOOST_CHECK_EQUAL(s.getExecutionDetails().size(), size_t(0));
  }
}


BOOST_FIXTURE_TEST_CASE(TestSuccessfulCommand,  CommandTestSetup)
{
  BOOST_CHECK_NE(obj->getNumber(), size_t(54));

  params.add<std::string>("todo", "useResource");
  params.add<int>("x", 42);
  BOOST_REQUIRE_NE(params.get<int>("x"), boost::any_cast<int>(DummyCommand::kDefaultResult));
  core::ParameterSet lExecutionDetails;
  lExecutionDetails.add<bool>("itemTheFirst", false);
  lExecutionDetails.add<int>("item2", -42);
  lExecutionDetails.add<std::string>("thirdThing", "Hello World!");
  dynamic_cast<DummyCommand&>(cmd).setExecutionDetails(lExecutionDetails);

  cmd.exec(params);
  do {
    boost::this_thread::sleep_for( boost::chrono::milliseconds(1) );
  }
  while ( (cmd.getState() == Functionoid::kScheduled) || (cmd.getState() == Functionoid::kRunning) );

  BOOST_CHECK_EQUAL(obj->getNumber(), uint32_t(54));

  CommandSnapshot s = cmd.getStatus();
  BOOST_CHECK_EQUAL(s.getActionPath(), obj->getPath() + "." + cmd.getId());
  BOOST_CHECK_EQUAL(s.getActionId(), cmd.getId());
  BOOST_CHECK_EQUAL(s.getActionableId(), obj->getId());
  BOOST_CHECK_EQUAL(s.getState(), Functionoid::kDone);
  BOOST_CHECK_EQUAL(s.getProgress(), 1.0);
  BOOST_CHECK_EQUAL(s.getStatusMsg(), DummyCommand::kFinalMsgUseResource);
  BOOST_CHECK_EQUAL(s.getParameters().size(), size_t(2));
  BOOST_CHECK_EQUAL(s.getParameters().get<int>("x"), 42);
  BOOST_CHECK_EQUAL(s.getParameters().get<std::string>("todo"), "useResource");
  BOOST_REQUIRE(not s.getResult().empty());
  BOOST_CHECK_EQUAL(boost::any_cast<int>(s.getResult()), params.get<int>("x"));
  BOOST_CHECK_EQUAL(s.getExecutionDetails().size(), size_t(3));
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<bool>("itemTheFirst"), false);
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<int>("item2"), -42);
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<std::string>("thirdThing"), "Hello World!");
}


BOOST_FIXTURE_TEST_CASE(TestCommandWarning,  CommandTestSetup)
{
  core::ParameterSet lExecutionDetails;
  lExecutionDetails.add<bool>("itemTheFirst", false);
  lExecutionDetails.add<int>("item2", -42);
  lExecutionDetails.add<std::string>("thirdThing", "Hello World!");
  dynamic_cast<DummyWarningCommand&>(warningCmd).setExecutionDetails(lExecutionDetails);

  warningCmd.exec(params);
  do {
    boost::this_thread::sleep_for( boost::chrono::milliseconds(1) );
  }
  while ( (warningCmd.getState() == Functionoid::kScheduled) || (warningCmd.getState() == Functionoid::kRunning) );

  CommandSnapshot s = warningCmd.getStatus();
  BOOST_CHECK_EQUAL(s.getActionPath(), obj->getPath() + "." + warningCmd.getId());
  BOOST_CHECK_EQUAL(s.getActionId(), warningCmd.getId());
  BOOST_CHECK_EQUAL(s.getActionableId(), obj->getId());
  BOOST_CHECK_EQUAL(s.getState(), Functionoid::kWarning);
  BOOST_CHECK_EQUAL(s.getProgress(), 1.0);
  BOOST_CHECK_EQUAL(s.getStatusMsg(), DummyWarningCommand::kFinalMsg);
  BOOST_CHECK_EQUAL(s.getParameters().size(), size_t(0));
  BOOST_REQUIRE(not s.getResult().empty());
  BOOST_CHECK_EQUAL(boost::any_cast<int>(s.getResult()), boost::any_cast<int>(DummyWarningCommand::kDefaultResult));
  BOOST_CHECK_EQUAL(s.getExecutionDetails().size(), size_t(3));
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<bool>("itemTheFirst"), false);
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<int>("item2"), -42);
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<std::string>("thirdThing"), "Hello World!");
}


BOOST_FIXTURE_TEST_CASE(TestCommandError,  CommandTestSetup)
{
  core::ParameterSet lExecutionDetails;
  lExecutionDetails.add<bool>("itemTheFirst", false);
  lExecutionDetails.add<int>("item2", -42);
  lExecutionDetails.add<std::string>("thirdThing", "Hello World!");
  dynamic_cast<DummyErrorCommand&>(errorCmd).setExecutionDetails(lExecutionDetails);

  errorCmd.exec(params);
  do {
    boost::this_thread::sleep_for( boost::chrono::milliseconds(1) );
  }
  while ( (errorCmd.getState() == Functionoid::kScheduled) || (errorCmd.getState() == Functionoid::kRunning) );

  CommandSnapshot s = errorCmd.getStatus();
  BOOST_CHECK_EQUAL(s.getActionPath(), obj->getPath() + "." + errorCmd.getId());
  BOOST_CHECK_EQUAL(s.getActionId(), errorCmd.getId());
  BOOST_CHECK_EQUAL(s.getActionableId(), obj->getId());
  BOOST_CHECK_EQUAL(s.getState(), Functionoid::kError);
  BOOST_CHECK_EQUAL(s.getProgress(), DummyErrorCommand::kFinalProgress);
  BOOST_CHECK_EQUAL(s.getStatusMsg(), DummyErrorCommand::kFinalMsg);
  BOOST_CHECK_EQUAL(s.getParameters().size(), size_t(0));
  BOOST_REQUIRE(not s.getResult().empty());
  BOOST_CHECK_EQUAL(boost::any_cast<int>(s.getResult()), boost::any_cast<int>(DummyErrorCommand::kDefaultResult));
  BOOST_CHECK_EQUAL(s.getExecutionDetails().size(), size_t(3));
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<bool>("itemTheFirst"), false);
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<int>("item2"), -42);
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<std::string>("thirdThing"), "Hello World!");
}


BOOST_FIXTURE_TEST_CASE(TestThrowingCommand,  CommandTestSetup)
{
  core::ParameterSet lExecutionDetails;
  lExecutionDetails.add<bool>("itemTheFirst", false);
  lExecutionDetails.add<int>("item2", -42);
  lExecutionDetails.add<std::string>("thirdThing", "Hello World!");
  dynamic_cast<DummyThrowCommand&>(throwCmd).setExecutionDetails(lExecutionDetails);

  throwCmd.exec(params);
  do {
    boost::this_thread::sleep_for( boost::chrono::milliseconds(1) );
  }
  while ( (throwCmd.getState() == Functionoid::kScheduled) || (throwCmd.getState() == Functionoid::kRunning) );

  CommandSnapshot s = throwCmd.getStatus();
  BOOST_CHECK_EQUAL(s.getActionPath(), obj->getPath() + "." + throwCmd.getId());
  BOOST_CHECK_EQUAL(s.getActionId(), throwCmd.getId());
  BOOST_CHECK_EQUAL(s.getActionableId(), obj->getId());
  BOOST_CHECK_EQUAL(s.getState(), Functionoid::kError);
  BOOST_CHECK_EQUAL(s.getProgress(), DummyThrowCommand::kFinalProgress);
  BOOST_CHECK_EQUAL(s.getStatusMsg(), "An exception of type 'swatch::core::RuntimeError' was thrown in Command::code(): " + DummyThrowCommand::kExceptionMsg);
  BOOST_CHECK_EQUAL(s.getParameters().size(), size_t(0));
  BOOST_REQUIRE(not s.getResult().empty());
  BOOST_CHECK_EQUAL(boost::any_cast<int>(s.getResult()), boost::any_cast<int>(DummyThrowCommand::kDefaultResult));
  BOOST_CHECK_EQUAL(s.getExecutionDetails().size(), size_t(3));
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<bool>("itemTheFirst"), false);
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<int>("item2"), -42);
  BOOST_CHECK_EQUAL(s.getExecutionDetails().get<std::string>("thirdThing"), "Hello World!");
}


// ----- N.B. Migrated from TestConstraint, may need cleanup --------------- // 

// ----------------------------------------------------------------------------
struct ConstraintTestSetup
{
  boost::shared_ptr<DummyActionableObject> obj;

  ConstraintTestSetup() :
    obj( new DummyActionableObject("dummyObj"), ActionableObject::Deleter() )  {
  }
};
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
class DummySumSmallerThan : public core::PSetConstraint
{
public:
  DummySumSmallerThan( const std::string& a, const std::string& b, size_t max) :
    a(a),
    b(b),
    max(max)
  {
    require<uint32_t>(a);
    require<uint32_t>(b);
  }
  virtual ~DummySumSmallerThan() = default;

private:
  void describe(std::ostream& aStream) const override
  {
    aStream << "(" << a << " + " << b << ") < " << max;
  }
 
  Match verify(const core::ParameterSet& aParams) const override
  {
    return ( aParams.get<uint32_t>(a) + aParams.get<uint32_t>(b) ) < max;
  }

  const std::string a;
  const std::string b;
  const size_t max;

};

// ----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(ConstraintRegistrationTest, ConstraintTestSetup) {

  // Instatntiate a dummy command
  DummyCommand aCommand("abc", *obj);

  // Add 2 commands
  BOOST_CHECK_NO_THROW(aCommand.registerParameter<uint32_t>("first", 5));
  BOOST_CHECK_NO_THROW(aCommand.registerParameter<uint32_t>("second", 10));

  // Add a parameter that doesn't satisfy the default float rule
  BOOST_CHECK_THROW(aCommand.registerParameter("third", float(NAN)), CommandParameterCheckFailed);

  // Add a constraint
  BOOST_CHECK_NO_THROW(aCommand.addConstraint("constr1",DummySumSmallerThan("first","second",20)));

  // Add a constraint with the same name
  BOOST_CHECK_THROW(aCommand.addConstraint("constr1",DummySumSmallerThan("first","second",5)), CommandConstraintRegistrationFailed);

  //
  BOOST_CHECK_THROW(aCommand.addConstraint("constr2",DummySumSmallerThan("first","second",5)), CommandConstraintRegistrationFailed);

  BOOST_CHECK_THROW(aCommand.unregisterParameter("second"), CommandParameterRegistrationFailed);

}

// ----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(RuntimeParameterCheckTest, ConstraintTestSetup) {


  DummyCommand aCommand("abc", *obj);
  aCommand.registerParameter<uint32_t>("first", 5);
  aCommand.registerParameter<uint32_t>("second", 10);
  aCommand.addConstraint("constr1",DummySumSmallerThan("first","second", 20));

  core::ParameterSet lParams;
  lParams.add<uint32_t>("first", 10);
  lParams.add<uint32_t>("second", 15);

  BOOST_CHECK_THROW(aCommand.exec(lParams, false), CommandParameterCheckFailed);
}
// ----------------------------------------------------------------------------


BOOST_AUTO_TEST_SUITE_END() // CommandTestSuite

} /* namespace test */
} /* namespace action */
} /* namespace swatch */

