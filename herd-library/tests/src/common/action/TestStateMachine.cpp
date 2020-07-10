
#include <boost/test/unit_test.hpp>


// SWATCH headers
#include "swatch/action/CommandSequence.hpp"
#include "swatch/action/StateMachine.hpp"
#include "swatch/action/test/DummyActionableObject.hpp"
#include "swatch/action/test/DummyCommand.hpp"
#include "swatch/action/test/DummyGateKeeper.hpp"


namespace swatch {
namespace action {
namespace test {

struct StateMachineTestSetup {
  StateMachineTestSetup() :
    obj(new DummyActionableObject("dummyObj"), ActionableObject::Deleter()),
    cmdNormal1( obj->registerCommand<DummyCommand>("cmdNormal1") ),
    cmdNormal2( obj->registerCommand<DummyCommand>("cmdNormal2") ),
    cmdWarning( obj->registerCommand<DummyWarningCommand>("cmdWarning") ),
    cmdError( obj->registerCommand<DummyErrorCommand>("cmdError") ),
    cmdSeq( obj->registerSequence(kCmdSeqId, cmdNormal1).then(cmdNormal2) ),
    testFSM( obj->registerStateMachine("myTestFSM" , kState0, kStateError) )
  {
    typedef std::shared_ptr<const boost::any> ParamPtr_t;

    testFSM.addState(kStateA);
    testFSM.addState(kStateB);
    transitionItoA = & testFSM.addTransition("t1", kState0, kStateA);
    transitionAtoB = & testFSM.addTransition("t2", kStateA, kStateB);
    transitionBtoI = & testFSM.addTransition("t2", kStateB, kState0);

    GateKeeper::ParametersContext_t tbl(new GateKeeper::Parameters_t{
      {DummyCommand::kParamToDo, ParamPtr_t(new boost::any(std::string("")))},
      {DummyCommand::kParamX, ParamPtr_t(new boost::any(int(42)))}
    });
    gk.addContext("common", tbl);

    DummyConstraint::setResult(true);
    DummyRule<int>::setResult(true);
    DummyRule<std::string>::setResult(true);
  }

  ~StateMachineTestSetup()
  {
    // Reset result of dummy constraints & rules to 'true', to ensure clean environment for next test case
    DummyConstraint::setResult(true);
    DummyRule<int>::setResult(true);
    DummyRule<std::string>::setResult(true);
  }

  std::shared_ptr<DummyActionableObject> obj;
  Command& cmdNormal1;
  Command& cmdNormal2;
  Command& cmdWarning;
  Command& cmdError;
  CommandSequence& cmdSeq;
  StateMachine& testFSM;
  Transition* transitionItoA;
  Transition* transitionAtoB;
  Transition* transitionBtoI;
  DummyGateKeeper gk;


  static const std::string kCmdSeqId;
  static const std::string kState0, kStateError, kStateA, kStateB;
};

const std::string StateMachineTestSetup::kCmdSeqId = "cmdSeqA";

const std::string StateMachineTestSetup::kState0 = "s0";
const std::string StateMachineTestSetup::kStateError = "sErr";
const std::string StateMachineTestSetup::kStateA = "sA";
const std::string StateMachineTestSetup::kStateB = "sB";



BOOST_AUTO_TEST_SUITE( StateMachineTestSuite )


/* ------------------------------------------------------------------------- */
/*   PART 1: CONSTRUCTION & INITIALISATION (defining states & transitions)   */
/* ------------------------------------------------------------------------- */

BOOST_AUTO_TEST_CASE(TestConstruction)
{
  DummyActionableObject obj("dummy");
  StateMachine& fsm = obj.registerStateMachine("anOp", "someState", "myErrState");

  BOOST_CHECK_EQUAL(fsm.getId(), "anOp");
  BOOST_CHECK_EQUAL(fsm.getPath(), obj.getId()+"."+fsm.getId());
  BOOST_CHECK_EQUAL(&obj.getStateMachine(fsm.getId()), &fsm);
  BOOST_CHECK_EQUAL(&obj.getObj(fsm.getId()), (core::Object*) &fsm);

  BOOST_CHECK_EQUAL(&fsm.getActionable(), &obj);
  BOOST_CHECK_EQUAL(fsm.getInitialState(), "someState");
  BOOST_CHECK_EQUAL(fsm.getErrorState(), "myErrState");

  std::vector<std::string> expectedStates = {"someState", "myErrState"};
  BOOST_CHECK_EQUAL_COLLECTIONS( fsm.getStates().begin(), fsm.getStates().end(), expectedStates.begin(), expectedStates.end() );
}


BOOST_AUTO_TEST_CASE(TestAddState)
{
  DummyActionableObject obj("dummy");
  StateMachine& fsm = obj.registerStateMachine("anOp", "someState", "myErrState");

  std::vector<std::string> expectedStates = {"someState", "myErrState"};
  BOOST_REQUIRE_EQUAL_COLLECTIONS( fsm.getStates().begin(), fsm.getStates().end(), expectedStates.begin(), expectedStates.end() );

  // addState should throw if state already defined
  BOOST_CHECK_THROW(fsm.addState("someState"), StateAlreadyDefined);
  BOOST_CHECK_THROW(fsm.addState("myErrState"), StateAlreadyDefined);

  // addState should add the state if not already defined
  BOOST_CHECK_NO_THROW(fsm.addState("anotherState"));
  expectedStates.push_back("anotherState");
  BOOST_CHECK_EQUAL_COLLECTIONS( fsm.getStates().begin(), fsm.getStates().end(), expectedStates.begin(), expectedStates.end() );
}


BOOST_AUTO_TEST_CASE(TestAddTransition)
{
  DummyActionableObject obj("dummy");
  StateMachine& fsm = obj.registerStateMachine("aFSM", "state0", "errState");
  fsm.addState("state1");

  BOOST_CHECK_EQUAL( fsm.getTransitions("state0").size(), size_t(0) );
  BOOST_CHECK_EQUAL( fsm.getTransitions("state1").size(), size_t(0) );

  // 1) Add transition
  Transition& tA = fsm.addTransition("transitionA", "state0", "state1");

  BOOST_CHECK_EQUAL(tA.getId(), "transitionA");
  BOOST_CHECK_EQUAL(tA.getPath(), fsm.getPath()+".state0.transitionA");
  BOOST_CHECK_EQUAL(tA.getEndState(), "state1");
  BOOST_CHECK_EQUAL(&tA.getStateMachine(), &fsm);

  BOOST_CHECK_EQUAL(fsm.getTransitions("state0").size(), size_t(1));
  BOOST_CHECK_EQUAL(fsm.getTransitions("state0").find("transitionA")->second, &tA);
  BOOST_CHECK_EQUAL(fsm.getTransitions("state1").size(), size_t(0));

  // 2) Check that exception is thrown in each error case
  BOOST_CHECK_THROW( fsm.addTransition("newId", "s", "state1"), StateNotDefined);
  BOOST_CHECK_THROW( fsm.addTransition("newId", "state1", "s"), StateNotDefined);
  BOOST_CHECK_EQUAL( fsm.getTransitions("state1").size(), size_t(0));
  fsm.addState("state2");
  BOOST_CHECK_THROW( fsm.addTransition("transitionA", "state0", "state2"), TransitionAlreadyDefined);
  BOOST_CHECK_EQUAL( fsm.getTransitions("state0").size(), size_t(1));
}


BOOST_AUTO_TEST_CASE(TestAddTransitionSteps)
{
  DummyActionableObject obj("dummy");
  Command& cmd = obj.registerCommand<DummyCommand>("aCmd");
  CommandSequence& cmdSeq = obj.registerSequence("aCmdSeq", cmd);
  StateMachine& fsm = obj.registerStateMachine("anOp", "state0", "errState");
  fsm.addState("state1");

  // 1) Add transition, no steps yet
  Transition& tA = fsm.addTransition("transitionA", "state0", "state1");

  BOOST_CHECK_EQUAL(tA.size(), size_t(0));
  BOOST_CHECK(tA.begin() == tA.end());

  // 2) Add command to transition
  BOOST_CHECK_EQUAL(&(tA.add(cmd)), &tA);
  BOOST_CHECK_EQUAL(tA.size(), size_t(1));
  BOOST_CHECK_EQUAL(&tA.begin()->get(), &cmd);
  // BOOST_CHECK_EQUAL( tA.begin()->getAlias(), "");
  BOOST_CHECK_EQUAL( tA.begin()->getNamespace(), "");

  // 3) Add command to transition, using different namespace
  BOOST_CHECK_EQUAL(&(tA.add(cmd, "dummyNamespace")), &tA);
  BOOST_CHECK_EQUAL(tA.size(), size_t(2));
  BOOST_CHECK_EQUAL(&(tA.begin()+1)->get(), &cmd);
  // BOOST_CHECK_EQUAL( (tA.begin()+1)->getAlias(), "dummyAlias");
  BOOST_CHECK_EQUAL( (tA.begin()+1)->getNamespace(), "dummyNamespace");

  // 3) Add command sequence to transition
  BOOST_CHECK_EQUAL(&(tA.add(cmdSeq)), &tA);
  BOOST_CHECK_EQUAL(tA.size(), size_t(2 + cmdSeq.size()));
  CommandVec::const_iterator lIt1 = cmdSeq.begin();
  for (auto lIt2=(tA.begin()+2); lIt1!=cmdSeq.end(); lIt1++, lIt2++) {
    BOOST_CHECK_EQUAL(&lIt1->get(), &lIt2->get());
    // BOOST_CHECK_EQUAL(lIt1->getAlias(), lIt2->getAlias());
    BOOST_CHECK_EQUAL(lIt1->getNamespace(), lIt2->getNamespace());
  }

  // 4) Adding command from other object: Should throw, and leave transition unchanged
  DummyActionableObject obj2("dummy2");
  Command& cmd2 = obj2.registerCommand<DummyCommand>("aCmd");
  BOOST_CHECK_THROW(tA.add(cmd2), InvalidResource);
  BOOST_CHECK_EQUAL(tA.size(), size_t(2 + cmdSeq.size()));
}



/* ---------------------------------------------------------------- */
/*   PART 2: ENGAGING & DISENGAGING                                 */
/* ---------------------------------------------------------------- */


BOOST_FIXTURE_TEST_CASE(TestEngageFSM, StateMachineTestSetup)
{
  // Confirm correct construction before staring tests
  BOOST_REQUIRE_EQUAL( obj->getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId );
  BOOST_REQUIRE_EQUAL( obj->getStatus().getState(), ActionableSnapshot::kNullStateId );

  // Engaging FSM should put object into FSM's initial state
  BOOST_CHECK_NO_THROW( testFSM.engage() );
  BOOST_CHECK_EQUAL( obj->getStatus().getStateMachineId(), testFSM.getId() );
  BOOST_CHECK_EQUAL( obj->getStatus().getState(), kState0 );

  // Trying to engage FSM again shouldn't work
  BOOST_CHECK_THROW( testFSM.engage(), ResourceInWrongStateMachine );
  BOOST_CHECK_EQUAL( obj->getStatus().getStateMachineId(), testFSM.getId() );
  BOOST_CHECK_EQUAL( obj->getStatus().getState(), kState0 );

  // Trying to engage another FSM also shouldn't work
  StateMachine& otherFSM = obj->registerStateMachine("anotherFSM", "otherInitialState", "otherErrorState");
  BOOST_CHECK_THROW( otherFSM.engage(), ResourceInWrongStateMachine );
  BOOST_CHECK_EQUAL( obj->getStatus().getStateMachineId(), testFSM.getId() );
  BOOST_CHECK_EQUAL( obj->getStatus().getState(), kState0 );
}


BOOST_AUTO_TEST_CASE(TestDisengageFSM)
{
  DummyActionableObject obj("dummy");
  const std::string initialState = "myInitialState";
  StateMachine& testFSM = obj.registerStateMachine("myFSM", initialState, "myErrorState");

  // Confirm correct construction before testing disengage method
  BOOST_REQUIRE_EQUAL( obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId );
  BOOST_REQUIRE_EQUAL( obj.getStatus().getState(), ActionableSnapshot::kNullStateId );

  // Can't disengage if FSM isn't yet engaged
  BOOST_CHECK_THROW( testFSM.disengage(), ResourceInWrongStateMachine);
  BOOST_CHECK_EQUAL( obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( obj.getStatus().getState(), ActionableSnapshot::kNullStateId);

  // Engage FSM before continuing unit tests for disengage
  testFSM.engage();
  BOOST_REQUIRE_EQUAL( obj.getStatus().getStateMachineId(), testFSM.getId() );
  BOOST_REQUIRE_EQUAL( obj.getStatus().getState(), initialState );

  // Disengaging another FSM: Should throw, and leave state unchanged
  StateMachine& otherFSM = obj.registerStateMachine("anotherFSM", "otherInitialState", "otherErrorState");
  BOOST_CHECK_THROW( otherFSM.disengage(), ResourceInWrongStateMachine );
  BOOST_CHECK_EQUAL( obj.getStatus().getStateMachineId(), testFSM.getId() );
  BOOST_CHECK_EQUAL( obj.getStatus().getState(), initialState );

  // Disengaging the engaged FSM
  BOOST_CHECK_NO_THROW( testFSM.disengage() );
  BOOST_CHECK_EQUAL( obj.getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId );
  BOOST_CHECK_EQUAL( obj.getStatus().getState(), ActionableSnapshot::kNullStateId );
}


/* ---------------------------------------------------------------- */
/*   PART 3: RUNNING TRANSITIONS                                    */
/* ---------------------------------------------------------------- */

BOOST_FIXTURE_TEST_CASE(TestRunTransitionDisengagedFSM, StateMachineTestSetup)
{
  DummyGateKeeper lEmptyGK;

  // Running transition before FSM is engaged: should throw, and leave state/TransitionStatus unchanged
  BOOST_CHECK_THROW( transitionItoA->exec(lEmptyGK, false), ResourceInWrongState);
  BOOST_CHECK_EQUAL( transitionItoA->getState(), Functionoid::State::kInitial);
  BOOST_CHECK_EQUAL( transitionItoA->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_CHECK_EQUAL( obj->getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId);
  BOOST_CHECK_EQUAL( obj->getStatus().getState(), ActionableSnapshot::kNullStateId);

  // Running transition with other FSM engaged: should throw, and leave state/TransitionStatus unchanged
  StateMachine& otherFSM = obj->registerStateMachine("anotherFSM", kState0, kStateError);
  otherFSM.engage();
  BOOST_REQUIRE_EQUAL( obj->getStatus().getStateMachineId(), otherFSM.getId() );
  BOOST_CHECK_THROW( transitionItoA->exec(lEmptyGK, false), ResourceInWrongState );
  BOOST_CHECK_EQUAL( transitionItoA->getState(), Functionoid::State::kInitial);
  BOOST_CHECK_EQUAL( transitionItoA->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_CHECK_EQUAL( obj->getStatus().getStateMachineId(), otherFSM.getId());
  BOOST_CHECK_EQUAL( obj->getStatus().getState(), kState0);
}


BOOST_FIXTURE_TEST_CASE(TestRunEmptyTransitions, StateMachineTestSetup)
{
  DummyGateKeeper emptyGK;

  // Engage state machine and confirm initial state to prepare for testing
  BOOST_REQUIRE_NO_THROW( testFSM.engage() );
  BOOST_REQUIRE_EQUAL( obj->getStatus().getStateMachineId(), testFSM.getId() );
  BOOST_REQUIRE_EQUAL( obj->getStatus().getState(), kState0 );

  // Check that invalid transitions not executed
  BOOST_CHECK_THROW( transitionAtoB->exec(emptyGK, false), ResourceInWrongState);
  BOOST_CHECK_EQUAL( transitionAtoB->getState(), Functionoid::State::kInitial);
  BOOST_CHECK_EQUAL( transitionAtoB->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_CHECK_EQUAL( obj->getStatus().getState(), kState0 );

  BOOST_CHECK_THROW( transitionBtoI->exec(emptyGK, false), ResourceInWrongState);
  BOOST_CHECK_EQUAL( transitionBtoI->getState(), Functionoid::State::kInitial);
  BOOST_CHECK_EQUAL( transitionBtoI->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_CHECK_EQUAL( obj->getStatus().getState(), kState0 );

  // Execute state transition: 'initial' -> A
  BOOST_CHECK_NO_THROW( transitionItoA->exec(emptyGK, false) );
  BOOST_CHECK_EQUAL( obj->getStatus().getState(), kStateA );
  BOOST_CHECK_EQUAL( transitionItoA->getState(), Functionoid::State::kDone );
  TransitionSnapshot s = transitionItoA->getStatus();
  BOOST_CHECK_EQUAL( s.getActionPath(), transitionItoA->getPath() );
  BOOST_CHECK_EQUAL(s.getActionId(), transitionItoA->getId());
  BOOST_CHECK_EQUAL(s.getActionableId(), obj->getId());
  BOOST_CHECK( s.begin() == s.end());
  BOOST_CHECK_EQUAL( s.end() - s.begin(), 0);
  BOOST_CHECK_EQUAL( s.getNumberOfCompletedCommands(), size_t(0));
  BOOST_CHECK_EQUAL( s.getProgress(), 1.0);
  BOOST_CHECK( s.getResults().empty() );
  BOOST_CHECK( s.getRunningTime() >= 0.0 );
  BOOST_CHECK_EQUAL( s.getState(), Functionoid::State::kDone);
  BOOST_CHECK_EQUAL( s.getTotalNumberOfCommands(), size_t(0));

  // Check that invalid transitions not executed
  BOOST_CHECK_THROW( transitionItoA->exec(emptyGK, false), ResourceInWrongState);
  BOOST_CHECK_EQUAL( obj->getStatus().getState(), kStateA );

  BOOST_CHECK_THROW( transitionBtoI->exec(emptyGK, false), ResourceInWrongState);
  BOOST_CHECK_EQUAL( transitionBtoI->getStatus().getState(), Functionoid::State::kInitial);
  BOOST_CHECK_EQUAL( transitionBtoI->getState(), Functionoid::State::kInitial);
  BOOST_CHECK_EQUAL( obj->getStatus().getState(), kStateA );

  // Execute state transition: A -> B
  BOOST_CHECK_NO_THROW( transitionAtoB->exec(emptyGK, false) );
  BOOST_CHECK_EQUAL( obj->getStatus().getState(), kStateB );
  s = transitionAtoB->getStatus();
  BOOST_CHECK_EQUAL( s.getActionPath(), transitionAtoB->getPath() );
  BOOST_CHECK_EQUAL(s.getActionId(), transitionAtoB->getId());
  BOOST_CHECK_EQUAL(s.getActionableId(), obj->getId());
  BOOST_CHECK( s.begin() == s.end());
  BOOST_CHECK_EQUAL( s.end() - s.begin(), 0);
  BOOST_CHECK_EQUAL( s.getNumberOfCompletedCommands(), size_t(0));
  BOOST_CHECK_EQUAL( s.getProgress(), 1.0);
  BOOST_CHECK( s.getResults().empty() );
  BOOST_CHECK( s.getRunningTime() >= 0.0 );
  BOOST_CHECK_EQUAL( s.getState(), Functionoid::State::kDone);
  BOOST_CHECK_EQUAL( s.getTotalNumberOfCommands(), size_t(0));
}


BOOST_FIXTURE_TEST_CASE(TestTransitionMissingParams, StateMachineTestSetup)
{
  DummyGateKeeper lEmptyGK;
  transitionItoA->add(cmdNormal1);
  transitionItoA->add(cmdSeq);

  // Engage state machine, and confirm object's state is ready for following tests
  testFSM.engage();
  BOOST_REQUIRE_EQUAL( obj->getStatus().getStateMachineId(), testFSM.getId());
  BOOST_REQUIRE_EQUAL( obj->getStatus().getState(), kState0);

  // Running transition without parameter values defined: Should throw, and leave state unchanged
  BOOST_CHECK_THROW( transitionItoA->exec(lEmptyGK, false), ParameterNotFound );
  BOOST_CHECK_EQUAL( transitionItoA->getState(), Functionoid::State::kInitial );
  BOOST_CHECK_EQUAL( transitionItoA->getStatus().getState(), Functionoid::State::kInitial );
  BOOST_CHECK_EQUAL( obj->getStatus().getStateMachineId(), testFSM.getId());
  BOOST_CHECK_EQUAL( obj->getStatus().getState(), kState0);

  // checkMissingParams method should return correct parameters
  typedef CommandVec::MissingParam MissingParam_t;
  std::vector<MissingParam_t> result, expected;
  expected.push_back(MissingParam_t("", cmdNormal1.getId(), DummyCommand::kParamToDo));
  expected.push_back(MissingParam_t("", cmdNormal1.getId(), DummyCommand::kParamX));
  expected.push_back(MissingParam_t(cmdSeq.getId(), cmdNormal1.getId(), DummyCommand::kParamToDo));
  expected.push_back(MissingParam_t(cmdSeq.getId(), cmdNormal1.getId(), DummyCommand::kParamX));
  expected.push_back(MissingParam_t(cmdSeq.getId(), cmdNormal2.getId(), DummyCommand::kParamToDo));
  expected.push_back(MissingParam_t(cmdSeq.getId(), cmdNormal2.getId(), DummyCommand::kParamX));

  std::vector<core::ParameterSet> paramSets;
  transitionItoA->checkForMissingParameters(lEmptyGK, paramSets, result);
  BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(), result.end(), expected.begin(), expected.end());
  BOOST_REQUIRE_EQUAL( paramSets.size(), size_t(3) );
}


BOOST_FIXTURE_TEST_CASE(TestTransitionInvalidParams, StateMachineTestSetup)
{
  transitionItoA->add(cmdNormal1);
  transitionItoA->add(cmdNormal2);

  // Engage state machine, and confirm object's state is ready for following tests
  testFSM.engage();
  BOOST_REQUIRE_EQUAL( obj->getStatus().getStateMachineId(), testFSM.getId());
  BOOST_REQUIRE_EQUAL( obj->getStatus().getState(), kState0);

  // Running transition while constraints and/or rules fail: Should throw, and leave state unchanged
  DummyConstraint::setResult(false);
  BOOST_CHECK_THROW( transitionItoA->exec(gk, false), CommandParameterCheckFailed );
  DummyConstraint::setResult(true);
  DummyRule<int>::setResult(false);
  BOOST_CHECK_THROW( transitionItoA->exec(gk, false), CommandParameterCheckFailed );
  DummyRule<int>::setResult(true);
  DummyRule<std::string>::setResult(false);
  BOOST_CHECK_THROW( transitionItoA->exec(gk, false), CommandParameterCheckFailed );  
  BOOST_CHECK_EQUAL( transitionItoA->getState(), Functionoid::State::kInitial );
  BOOST_CHECK_EQUAL( transitionItoA->getStatus().getState(), Functionoid::State::kInitial );
  BOOST_CHECK_EQUAL( cmdNormal1.getState(), Functionoid::kInitial );
  BOOST_CHECK_EQUAL( cmdNormal2.getState(), Functionoid::kInitial );
  BOOST_CHECK_EQUAL( obj->getStatus().getStateMachineId(), testFSM.getId());
  BOOST_CHECK_EQUAL( obj->getStatus().getState(), kState0);

  // checkForInvalidParameters method should return correct information
  std::vector<core::ParameterSet> lParamSets;
  std::vector<CommandVec::MissingParam> lMissingParams;
  transitionItoA->checkForMissingParameters(gk, lParamSets, lMissingParams);
  BOOST_REQUIRE( lMissingParams.empty() );

  typedef Command::ParamRuleViolation RuleViolation_t;
  typedef CommandVec::ParamRuleViolationList RuleViolationList_t;
  std::vector<RuleViolationList_t> lResult, lExpected;
  DummyConstraint::setResult(true);
  DummyRule<std::string>::setResult(true);
  transitionItoA->checkForInvalidParameters(lParamSets, lResult);
  BOOST_CHECK_EQUAL(lResult.size(), size_t(0));
  
  DummyConstraint::setResult( core::Match(false, "Details about constraint failure") );
  DummyRule<std::string>::setResult( core::Match(false, "Details about rule failure!") );
  transitionItoA->checkForInvalidParameters(lParamSets, lResult);
  core::ParameterSet lParamSet;
  lParamSet.add<std::string>(DummyCommand::kParamToDo, "");
  lParamSet.add<int>(DummyCommand::kParamX, 42);
  lExpected.push_back(RuleViolationList_t(cmdNormal1.getId(), 0, lParamSet, {}));
  lExpected.back().violations.push_back( RuleViolation_t( {DummyCommand::kParamToDo}, DummyRule<std::string>::sDescription, "Details about rule failure!"));
  lExpected.back().violations.push_back( RuleViolation_t( {DummyCommand::kParamToDo, DummyCommand::kParamX}, DummyConstraint::sDescription, "Details about constraint failure"));
  lExpected.back().violations.push_back( RuleViolation_t( {DummyCommand::kParamX}, DummyConstraint::sDescription, "Details about constraint failure"));
  lExpected.push_back(RuleViolationList_t(cmdNormal2.getId(), 1, lParamSet, {}));
  lExpected.back().violations.push_back( RuleViolation_t( {DummyCommand::kParamToDo}, DummyRule<std::string>::sDescription, "Details about rule failure!"));
  lExpected.back().violations.push_back( RuleViolation_t( {DummyCommand::kParamToDo, DummyCommand::kParamX}, DummyConstraint::sDescription, "Details about constraint failure"));
  lExpected.back().violations.push_back( RuleViolation_t( {DummyCommand::kParamX}, DummyConstraint::sDescription, "Details about constraint failure"));
  BOOST_CHECK_EQUAL_COLLECTIONS(lResult.begin(), lResult.end(), lExpected.begin(), lExpected.end());
}


BOOST_FIXTURE_TEST_CASE(TestRunErrorTransition, StateMachineTestSetup)
{
  transitionItoA->add(cmdNormal1);
  transitionItoA->add(cmdError);
  transitionItoA->add(cmdNormal2);

  // Engage state machine, and confirm object's state is ready for following tests
  testFSM.engage();
  BOOST_REQUIRE_EQUAL( obj->getStatus().getStateMachineId(), testFSM.getId());
  BOOST_REQUIRE_EQUAL( obj->getStatus().getState(), kState0);

  // Running transition with error command: Object should go to error state, and subsequent commands shouldn't run
  BOOST_REQUIRE_NO_THROW(transitionItoA->exec(gk, false));
  BOOST_CHECK_EQUAL(obj->getStatus().getStateMachineId(), testFSM.getId());
  BOOST_CHECK_EQUAL(obj->getStatus().getState(), kStateError);
  BOOST_CHECK_EQUAL(transitionItoA->getState(), Functionoid::State::kError);

  TransitionSnapshot s = transitionItoA->getStatus();
  BOOST_CHECK_EQUAL(s.getActionPath(), transitionItoA->getPath());
  BOOST_CHECK_EQUAL(s.getActionId(), transitionItoA->getId());
  BOOST_CHECK_EQUAL(s.getActionableId(), obj->getId());
  BOOST_CHECK_EQUAL(s.getNumberOfCompletedCommands(), size_t(2));
  BOOST_CHECK_EQUAL(s.getTotalNumberOfCommands(), size_t(3));
  BOOST_CHECK_CLOSE(s.getProgress(), (1.0 + DummyErrorCommand::kFinalProgress)/3.0, 0.0001);
  BOOST_CHECK_LT(s.getProgress(), 0.66);
  BOOST_CHECK_GT(s.getRunningTime(), 0.0);
  BOOST_CHECK_EQUAL(s.getState(), Functionoid::State::kError);
  BOOST_REQUIRE((s.begin()+2) == s.end());
  BOOST_REQUIRE_EQUAL(s.end() - s.begin(), 2);
  BOOST_CHECK_EQUAL(s.begin()->getActionPath(), cmdNormal1.getPath());
  BOOST_CHECK_EQUAL(s.begin()->getActionId(), cmdNormal1.getId());
  BOOST_CHECK_EQUAL(s.begin()->getActionableId(), obj->getId());
  BOOST_CHECK_EQUAL(s.begin()->getState(), Functionoid::State::kDone);
  BOOST_CHECK_EQUAL((s.begin()+1)->getActionPath(), cmdError.getPath());
  BOOST_CHECK_EQUAL((s.begin()+1)->getActionId(), cmdError.getId());
  BOOST_CHECK_EQUAL((s.begin()+1)->getActionableId(), obj->getId());
  BOOST_CHECK_EQUAL((s.begin()+1)->getState(), Functionoid::State::kError);

  BOOST_REQUIRE_EQUAL(s.getResults().size(), size_t(2));
  BOOST_CHECK_EQUAL(s.getResults().at(0) , &cmdNormal1.getStatus().getResult() );
  BOOST_CHECK_EQUAL(s.getResults().at(1) , &cmdError.getStatus().getResult() );

  BOOST_CHECK_EQUAL(cmdNormal2.getState(), Functionoid::State::kInitial);
  BOOST_CHECK_EQUAL(cmdNormal2.getStatus().getState(), Functionoid::State::kInitial);
}


BOOST_FIXTURE_TEST_CASE(TestRunWarningTransition, StateMachineTestSetup)
{
  transitionItoA->add(cmdNormal1);
  transitionItoA->add(cmdWarning);
  transitionItoA->add(cmdNormal2);

  // Engage state machine, and confirm object's state is ready for following tests
  testFSM.engage();
  BOOST_REQUIRE_EQUAL( obj->getStatus().getStateMachineId(), testFSM.getId());
  BOOST_REQUIRE_EQUAL( obj->getStatus().getState(), kState0);

  // Running transition with warning command: Object should go to state A, and all commands shouldt run
  BOOST_REQUIRE_NO_THROW(transitionItoA->exec(gk, false));
  BOOST_CHECK_EQUAL(obj->getStatus().getStateMachineId(), testFSM.getId());
  BOOST_CHECK_EQUAL(obj->getStatus().getState(), kStateA);
  BOOST_CHECK_EQUAL(transitionItoA->getState(), Functionoid::State::kWarning);

  TransitionSnapshot s = transitionItoA->getStatus();
  BOOST_CHECK_EQUAL(s.getActionPath(), transitionItoA->getPath());
  BOOST_CHECK_EQUAL(s.getActionId(), transitionItoA->getId());
  BOOST_CHECK_EQUAL(s.getActionableId(), obj->getId());
  BOOST_CHECK_EQUAL(s.getNumberOfCompletedCommands(), size_t(3));
  BOOST_CHECK_EQUAL(s.getTotalNumberOfCommands(), size_t(3));
  BOOST_CHECK_EQUAL(s.getProgress(), 1.0);
  BOOST_CHECK_EQUAL(s.getState(), Functionoid::State::kWarning);
  BOOST_REQUIRE((s.begin()+3) == s.end());
  BOOST_REQUIRE_EQUAL(s.end() - s.begin(), 3);
  BOOST_CHECK_EQUAL(s.begin()->getActionPath(), cmdNormal1.getPath());
  BOOST_CHECK_EQUAL(s.begin()->getActionId(), cmdNormal1.getId());
  BOOST_CHECK_EQUAL(s.begin()->getActionableId(), obj->getId());
  BOOST_CHECK_EQUAL(s.begin()->getState(), Functionoid::State::kDone);
  BOOST_CHECK_EQUAL((s.begin()+1)->getActionPath(), cmdWarning.getPath());
  BOOST_CHECK_EQUAL((s.begin()+1)->getActionId(), cmdWarning.getId());
  BOOST_CHECK_EQUAL((s.begin()+1)->getActionableId(), obj->getId());
  BOOST_CHECK_EQUAL((s.begin()+1)->getState(), Functionoid::State::kWarning);
  BOOST_CHECK_EQUAL((s.begin()+2)->getActionPath(), cmdNormal2.getPath());
  BOOST_CHECK_EQUAL((s.begin()+2)->getActionId(), cmdNormal2.getId());
  BOOST_CHECK_EQUAL((s.begin()+2)->getActionableId(), obj->getId());
  BOOST_CHECK_EQUAL((s.begin()+2)->getState(), Functionoid::State::kDone);

  BOOST_REQUIRE_EQUAL(s.getResults().size(), size_t(3));
  BOOST_CHECK_EQUAL(s.getResults().at(0) , &cmdNormal1.getStatus().getResult() );
  BOOST_CHECK_EQUAL(s.getResults().at(1) , &cmdWarning.getStatus().getResult() );
  BOOST_CHECK_EQUAL(s.getResults().at(2) , &cmdNormal2.getStatus().getResult() );
}


BOOST_FIXTURE_TEST_CASE(TestRunGoodTransition, StateMachineTestSetup)
{
  transitionItoA->add(cmdNormal1);
  transitionItoA->add(cmdNormal2);

  // Engage state machine, and confirm object's state is ready for following tests
  testFSM.engage();
  BOOST_REQUIRE_EQUAL( obj->getStatus().getStateMachineId(), testFSM.getId());
  BOOST_REQUIRE_EQUAL( obj->getStatus().getState(), kState0);

  // Running transition with warning command: Object should go to state A, and all commands shouldt run
  BOOST_REQUIRE_NO_THROW(transitionItoA->exec(gk, false));
  BOOST_CHECK_EQUAL(obj->getStatus().getStateMachineId(), testFSM.getId());
  BOOST_CHECK_EQUAL(obj->getStatus().getState(), kStateA);
  BOOST_CHECK_EQUAL(transitionItoA->getState(), Functionoid::State::kDone);

  TransitionSnapshot s = transitionItoA->getStatus();
  BOOST_CHECK_EQUAL(s.getActionPath(), transitionItoA->getPath());
  BOOST_CHECK_EQUAL(s.getActionId(), transitionItoA->getId());
  BOOST_CHECK_EQUAL(s.getActionableId(), obj->getId());
  BOOST_CHECK_EQUAL(s.getNumberOfCompletedCommands(), size_t(2));
  BOOST_CHECK_EQUAL(s.getTotalNumberOfCommands(), size_t(2));
  BOOST_CHECK_EQUAL(s.getProgress(), 1.0);
  BOOST_CHECK_EQUAL(s.getState(), Functionoid::State::kDone);
  BOOST_REQUIRE((s.begin()+2) == s.end());
  BOOST_REQUIRE_EQUAL(s.end() - s.begin(), 2);
  BOOST_CHECK_EQUAL(s.begin()->getActionPath(), cmdNormal1.getPath());
  BOOST_CHECK_EQUAL(s.begin()->getActionId(), cmdNormal1.getId());
  BOOST_CHECK_EQUAL(s.begin()->getActionableId(), obj->getId());
  BOOST_CHECK_EQUAL(s.begin()->getState(), Functionoid::State::kDone);
  BOOST_CHECK_EQUAL((s.begin()+1)->getActionPath(), cmdNormal2.getPath());
  BOOST_CHECK_EQUAL((s.begin()+1)->getActionId(), cmdNormal2.getId());
  BOOST_CHECK_EQUAL((s.begin()+1)->getActionableId(), obj->getId());
  BOOST_CHECK_EQUAL((s.begin()+1)->getState(), Functionoid::State::kDone);

  BOOST_REQUIRE_EQUAL(s.getResults().size(), size_t(2));
  BOOST_CHECK_EQUAL(s.getResults().at(0) , &cmdNormal1.getStatus().getResult() );
  BOOST_CHECK_EQUAL(s.getResults().at(1) , &cmdNormal2.getStatus().getResult() );
}



/* ---------------------------------------------------------------- */
/*   PART 4: RESETTING                                              */
/* ---------------------------------------------------------------- */


BOOST_FIXTURE_TEST_CASE(TestResetFSM, StateMachineTestSetup)
{
  // Engage state machine and confirm initial state to prepare for following tests
  BOOST_REQUIRE_NO_THROW( testFSM.engage() );
  BOOST_REQUIRE_EQUAL( obj->getStatus().getStateMachineId(), testFSM.getId() );
  BOOST_REQUIRE_EQUAL( obj->getStatus().getState(), kState0 );

  // Reset state machine
  BOOST_CHECK_NO_THROW( testFSM.reset() );
  BOOST_CHECK_EQUAL( obj->getStatus().getStateMachineId(), testFSM.getId() );
  BOOST_CHECK_EQUAL( obj->getStatus().getState(), kState0 );

  // Run a transition to prepare for following tests
  BOOST_REQUIRE_NO_THROW( transitionItoA->exec(DummyGateKeeper(), false) );
  BOOST_REQUIRE_EQUAL( obj->getStatus().getStateMachineId(), testFSM.getId() );
  BOOST_REQUIRE_EQUAL( obj->getStatus().getState(), kStateA );

  // Resetting another FSM: Should throw, and leave state unchanged
  StateMachine& otherFSM = obj->registerStateMachine("anotherFSM", "otherInitialState", "otherErrorState");
  BOOST_CHECK_THROW(otherFSM.reset(), ResourceInWrongStateMachine);
  BOOST_CHECK_EQUAL( obj->getStatus().getStateMachineId(), testFSM.getId() );
  BOOST_CHECK_EQUAL( obj->getStatus().getState(), kStateA );

  // Reset correct state machine
  BOOST_CHECK_NO_THROW( testFSM.reset() );
  BOOST_CHECK_EQUAL( obj->getStatus().getStateMachineId(), testFSM.getId() );
  BOOST_CHECK_EQUAL( obj->getStatus().getState(), kState0 );
}


BOOST_FIXTURE_TEST_CASE(TestResetDisengagedFSM, StateMachineTestSetup)
{
  // Resetting FSM before engaged: Should throw, and leave state unchanged
  BOOST_CHECK_THROW( testFSM.reset(), ResourceInWrongStateMachine);
  BOOST_CHECK_EQUAL( obj->getStatus().getStateMachineId(), ActionableSnapshot::kNullStateMachineId );
  BOOST_CHECK_EQUAL( obj->getStatus().getState(), ActionableSnapshot::kNullStateId );
}


BOOST_AUTO_TEST_SUITE_END() // StateMachineTestSuite
} /* namespace test */
} /* namespace action */
} /* namespace swatch */
