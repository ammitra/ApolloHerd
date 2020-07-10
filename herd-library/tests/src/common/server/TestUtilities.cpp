
#include <boost/test/unit_test.hpp>


#include <tuple>

#include <boost/lexical_cast.hpp>

// SWATCH headers
#include "swatch/action/Board.hpp"
#include "swatch/action/Command.hpp"
#include "swatch/action/DeviceStub.hpp"
#include "swatch/action/StateMachine.hpp"
#include "swatch/server/utilities.hpp"

#include "swatch/action/test/DummyCommand.hpp"
#include "swatch/action/test/DummyDevice.hpp"


namespace swatch {
namespace server {
namespace test {

struct UtilitiesTestSetup {
  UtilitiesTestSetup() :
    board(kDeviceStubs),
    deviceA(dynamic_cast<action::test::DummyDevice&>(board.getDevice("deviceA"))),
    deviceB(dynamic_cast<action::test::DummyDevice&>(board.getDevice("deviceB"))),
    commandA1(deviceA.registerCommand<action::test::DummyCommand>("dummyCommand")),
    commandA2(deviceA.registerCommand<action::test::DummyWarningCommand>("dummyCommand2"))
  {
    // Register 2 commands on device A, 1 on device B
    deviceB.registerCommand<action::test::DummyCommand>("myCommand");

    // Create 1 FSM on device A
    action::StateMachine& lFSM = deviceA.registerStateMachine("myFSM", "state0", "errorState");

    lFSM.addState("alpha");
    lFSM.addState("beta");
    lFSM.addTransition("hardReset", "state0", "state0").add(commandA1);
    lFSM.addTransition("null", "state0", "state0");
    lFSM.addTransition("configurePart1", "state0", "alpha").add(commandA1, "aNamespace").add(commandA1);
    lFSM.addTransition("configurePart2", "alpha", "beta").add(commandA2, "otherNamespace").add(commandA1);
  }

  ~UtilitiesTestSetup()
  {
  }

  static const std::vector<action::DeviceStub> kDeviceStubs;
  action::Board board;
  action::test::DummyDevice& deviceA;
  action::test::DummyDevice& deviceB;
  action::Command& commandA1;
  action::Command& commandA2;
};

const std::vector<action::DeviceStub> UtilitiesTestSetup::kDeviceStubs = {
  { {"deviceA"}, "swatch::action::test::DummyDevice", "aRole", "aUri", "addrTable.xml"},
  { {"deviceB"}, "swatch::action::test::DummyDevice", "otherRole", "uri2", "addrTableB.xml"}  
};


BOOST_AUTO_TEST_SUITE( ServerTestSuite )


BOOST_AUTO_TEST_CASE(TestEncodeServerError)
{
  msgpack::sbuffer lBuffer;
  encodeServerError("some message", lBuffer);

  msgpack::object_handle lHandle = msgpack::unpack(lBuffer.data(), lBuffer.size());
  msgpack::object lObj = lHandle.get();

  BOOST_REQUIRE_EQUAL(lObj.as<std::vector<std::string>>().size(), 2);
  BOOST_CHECK_EQUAL(lObj.as<std::vector<std::string>>().at(0), "error");
  BOOST_CHECK_EQUAL(lObj.as<std::vector<std::string>>().at(1), "some message");
}


BOOST_FIXTURE_TEST_CASE(TestEncodeDeviceInfo, UtilitiesTestSetup)
{
  msgpack::sbuffer lBuffer;

  // 1) 'Empty' board object
  encodeDeviceInfo(action::Board(std::vector<action::DeviceStub>()), lBuffer);

  msgpack::object_handle lHandle = msgpack::unpack(lBuffer.data(), lBuffer.size());
  msgpack::object lObj = lHandle.get();

  BOOST_REQUIRE_EQUAL(lObj.as<std::vector<msgpack::object>>().size(), 2);
  BOOST_CHECK_EQUAL(lObj.as<std::vector<msgpack::object>>().at(0).as<std::string>(), "ok");

  std::map<std::string, std::map<std::string, msgpack::object>> lDeviceInfoMap;
  lObj.as<std::vector<msgpack::object>>().at(1).convert(lDeviceInfoMap);
  BOOST_CHECK(lDeviceInfoMap.empty());

  // 2) Board containing two devices
  lBuffer.clear();
  encodeDeviceInfo(action::Board(kDeviceStubs), lBuffer);

  lHandle = msgpack::unpack(lBuffer.data(), lBuffer.size());
  lObj = lHandle.get();

  
  BOOST_REQUIRE_EQUAL(lObj.as<std::vector<msgpack::object>>().size(), 2);
  BOOST_CHECK_EQUAL(lObj.as<std::vector<msgpack::object>>().at(0).as<std::string>(), "ok");

  lObj.as<std::vector<msgpack::object>>().at(1).convert(lDeviceInfoMap);
  BOOST_CHECK_EQUAL(lDeviceInfoMap.size(), 2);

  BOOST_CHECK_EQUAL(lDeviceInfoMap.count("deviceA"), 1);
  BOOST_CHECK_EQUAL(lDeviceInfoMap.at("deviceA").size(), 6);
  BOOST_CHECK_EQUAL(lDeviceInfoMap.at("deviceA").at("role").as<std::string>(), "aRole");
  BOOST_CHECK_EQUAL(lDeviceInfoMap.at("deviceA").at("type").as<std::string>(), "swatch::action::test::DummyDevice");
  BOOST_CHECK_EQUAL(lDeviceInfoMap.at("deviceA").at("uri").as<std::string>(), "aUri");
  BOOST_CHECK_EQUAL(lDeviceInfoMap.at("deviceA").at("addressTable").as<std::string>(), "addrTable.xml");
  BOOST_CHECK_EQUAL(lDeviceInfoMap.at("deviceA").at("fsm").is_nil(), true);
  BOOST_CHECK_EQUAL(lDeviceInfoMap.at("deviceA").at("state").is_nil(), true);

  BOOST_CHECK_EQUAL(lDeviceInfoMap.count("deviceB"), 1);
  BOOST_CHECK_EQUAL(lDeviceInfoMap.at("deviceB").size(), 6);
  BOOST_CHECK_EQUAL(lDeviceInfoMap.at("deviceB").at("role").as<std::string>(), "otherRole");
  BOOST_CHECK_EQUAL(lDeviceInfoMap.at("deviceB").at("type").as<std::string>(), "swatch::action::test::DummyDevice");
  BOOST_CHECK_EQUAL(lDeviceInfoMap.at("deviceB").at("uri").as<std::string>(), "uri2");
  BOOST_CHECK_EQUAL(lDeviceInfoMap.at("deviceB").at("addressTable").as<std::string>(), "addrTableB.xml");
  BOOST_CHECK_EQUAL(lDeviceInfoMap.at("deviceA").at("fsm").is_nil(), true);
  BOOST_CHECK_EQUAL(lDeviceInfoMap.at("deviceA").at("state").is_nil(), true);
}


BOOST_FIXTURE_TEST_CASE(TestEncodeCommandInfo, UtilitiesTestSetup)
{
  msgpack::sbuffer lBuffer;

  encodeCommandInfo(board, lBuffer);

  msgpack::object_handle lHandle = msgpack::unpack(lBuffer.data(), lBuffer.size());
  msgpack::object lObj = lHandle.get();

  BOOST_REQUIRE_EQUAL(lObj.as<std::vector<msgpack::object>>().size(), 2);
  BOOST_CHECK_EQUAL(lObj.as<std::vector<msgpack::object>>().at(0).as<std::string>(), "ok");

  std::map<std::string, std::map<std::string, std::tuple<std::string, msgpack::object, msgpack::object>>> lCommandInfoMap;
  lObj.as<std::vector<msgpack::object>>().at(1).convert(lCommandInfoMap);

  // One top-level entry for each device
  BOOST_CHECK_EQUAL(lCommandInfoMap.size(), 2);
  BOOST_CHECK_EQUAL(lCommandInfoMap.count("deviceA"), 1);
  BOOST_CHECK_EQUAL(lCommandInfoMap.count("deviceB"), 1);

  // One 2nd-level entry for each command
  BOOST_CHECK_EQUAL(lCommandInfoMap.at("deviceA").size(), 2);
  BOOST_CHECK_EQUAL(lCommandInfoMap.at("deviceA").count("dummyCommand"), 1);
  BOOST_CHECK_EQUAL(lCommandInfoMap.at("deviceA").count("dummyCommand2"), 1);
  BOOST_CHECK_EQUAL(lCommandInfoMap.at("deviceB").size(), 1);
  BOOST_CHECK_EQUAL(lCommandInfoMap.at("deviceB").count("myCommand"), 1);

  // Check info given for deviceA.dummyCommand
  BOOST_CHECK_EQUAL(std::get<0>(lCommandInfoMap.at("deviceA").at("dummyCommand")), "");
  std::map<std::string, std::pair<std::string, msgpack::object>> lParameterInfoMap;
  std::get<1>(lCommandInfoMap.at("deviceA").at("dummyCommand")).convert(lParameterInfoMap);
  BOOST_CHECK_EQUAL(lParameterInfoMap.size(), 2);
  BOOST_CHECK_EQUAL(lParameterInfoMap.at("x").first, "int");
  BOOST_CHECK_EQUAL(lParameterInfoMap.at("x").second.as<std::string>(), "Dummy rule for unit tests");
  BOOST_CHECK_EQUAL(lParameterInfoMap.at("todo").first, "string");
  BOOST_CHECK_EQUAL(lParameterInfoMap.at("todo").second.as<std::string>(), "Dummy rule for unit tests");

  std::map<std::string, std::pair<std::vector<std::string>, std::string>> lConstraintInfoMap;
  std::get<2>(lCommandInfoMap.at("deviceA").at("dummyCommand")).convert(lConstraintInfoMap);
  BOOST_CHECK_EQUAL(lConstraintInfoMap.size(), 2);
  BOOST_CHECK_EQUAL(lConstraintInfoMap.at("constraintA").first.size(), 1);
  BOOST_CHECK_EQUAL(lConstraintInfoMap.at("constraintA").first.at(0), "x");
  BOOST_CHECK_EQUAL(lConstraintInfoMap.at("constraintA").second, "Dummy constraint for unit tests");
  BOOST_CHECK_EQUAL(lConstraintInfoMap.at("constraintB").first.size(), 2);
  BOOST_CHECK_EQUAL(lConstraintInfoMap.at("constraintB").first.at(0), "todo");
  BOOST_CHECK_EQUAL(lConstraintInfoMap.at("constraintB").first.at(1), "x");
  BOOST_CHECK_EQUAL(lConstraintInfoMap.at("constraintB").second, "Dummy constraint for unit tests");

  // Check info given for deviceA.dummyCommand2
  BOOST_CHECK_EQUAL(std::get<0>(lCommandInfoMap.at("deviceA").at("dummyCommand2")), "");
  std::get<1>(lCommandInfoMap.at("deviceA").at("dummyCommand2")).convert(lParameterInfoMap);
  BOOST_CHECK_EQUAL(lParameterInfoMap.size(), 0);
  std::get<2>(lCommandInfoMap.at("deviceA").at("dummyCommand2")).convert(lConstraintInfoMap);
  BOOST_CHECK_EQUAL(lConstraintInfoMap.size(), 0);

  // Check info given for deviceB.myCommand
  BOOST_CHECK_EQUAL(std::get<0>(lCommandInfoMap.at("deviceB").at("myCommand")), "");
  std::get<1>(lCommandInfoMap.at("deviceB").at("myCommand")).convert(lParameterInfoMap);
  BOOST_CHECK_EQUAL(lParameterInfoMap.size(), 2);
  BOOST_CHECK_EQUAL(lParameterInfoMap.at("x").first, "int");
  BOOST_CHECK_EQUAL(lParameterInfoMap.at("x").second.as<std::string>(), "Dummy rule for unit tests");
  BOOST_CHECK_EQUAL(lParameterInfoMap.at("todo").first, "string");
  BOOST_CHECK_EQUAL(lParameterInfoMap.at("todo").second.as<std::string>(), "Dummy rule for unit tests");

  std::get<2>(lCommandInfoMap.at("deviceB").at("myCommand")).convert(lConstraintInfoMap);
  BOOST_CHECK_EQUAL(lConstraintInfoMap.size(), 2);
  BOOST_CHECK_EQUAL(lConstraintInfoMap.at("constraintA").first.size(), 1);
  BOOST_CHECK_EQUAL(lConstraintInfoMap.at("constraintA").first.at(0), "x");
  BOOST_CHECK_EQUAL(lConstraintInfoMap.at("constraintA").second, "Dummy constraint for unit tests");
  BOOST_CHECK_EQUAL(lConstraintInfoMap.at("constraintB").first.size(), 2);
  BOOST_CHECK_EQUAL(lConstraintInfoMap.at("constraintB").first.at(0), "todo");
  BOOST_CHECK_EQUAL(lConstraintInfoMap.at("constraintB").first.at(1), "x");
  BOOST_CHECK_EQUAL(lConstraintInfoMap.at("constraintB").second, "Dummy constraint for unit tests");
}


BOOST_FIXTURE_TEST_CASE(TestEncodeFSMs, UtilitiesTestSetup)
{
  msgpack::sbuffer lBuffer;

  encodeFsmInfo(board, lBuffer);

  msgpack::object_handle lHandle = msgpack::unpack(lBuffer.data(), lBuffer.size());
  msgpack::object lObj = lHandle.get();

  BOOST_REQUIRE_EQUAL(lObj.as<std::vector<msgpack::object>>().size(), 2);
  BOOST_CHECK_EQUAL(lObj.as<std::vector<msgpack::object>>().at(0).as<std::string>(), "ok");

  using namespace std;
  typedef map<string, map<string, tuple<string, string, vector<pair<string, string>>>>> FsmStateMap_t;
  std::map<std::string, std::map<std::string, tuple<string, string, string, FsmStateMap_t>>> lFsmInfoMap;
  lObj.as<std::vector<msgpack::object>>().at(1).convert(lFsmInfoMap);

  // One top-level entry for each device
  BOOST_CHECK_EQUAL(lFsmInfoMap.size(), 2);
  BOOST_CHECK_EQUAL(lFsmInfoMap.count("deviceA"), 1);
  BOOST_CHECK_EQUAL(lFsmInfoMap.count("deviceB"), 1);

  // One 2nd-level entries for FSMs
  BOOST_CHECK_EQUAL(lFsmInfoMap.at("deviceA").size(), 1);
  BOOST_CHECK_EQUAL(lFsmInfoMap.at("deviceA").count("myFSM"), 1);
  BOOST_CHECK_EQUAL(lFsmInfoMap.at("deviceB").size(), 0);

  // Check info for each FSM
  BOOST_CHECK_EQUAL(get<0>(lFsmInfoMap.at("deviceA").at("myFSM")), "");
  BOOST_CHECK_EQUAL(get<1>(lFsmInfoMap.at("deviceA").at("myFSM")), "state0");
  BOOST_CHECK_EQUAL(get<2>(lFsmInfoMap.at("deviceA").at("myFSM")), "errorState");

  const FsmStateMap_t& lFsmStateMap = get<3>(lFsmInfoMap.at("deviceA").at("myFSM"));
  BOOST_CHECK_EQUAL(lFsmStateMap.size(), 4 /* number of states */);

  BOOST_CHECK_EQUAL(lFsmStateMap.count("state0"), 1);
  BOOST_CHECK_EQUAL(lFsmStateMap.count("errorState"), 1);
  BOOST_CHECK_EQUAL(lFsmStateMap.count("alpha"), 1);
  BOOST_CHECK_EQUAL(lFsmStateMap.count("beta"), 1);

  BOOST_CHECK_EQUAL(lFsmStateMap.at("state0").size(), 3 /* transitions from state0 */);
  BOOST_CHECK_EQUAL(lFsmStateMap.at("state0").count("hardReset"), 1);
  BOOST_CHECK_EQUAL(get<0>(lFsmStateMap.at("state0").at("hardReset")), "");
  BOOST_CHECK_EQUAL(get<1>(lFsmStateMap.at("state0").at("hardReset")), "state0");
  BOOST_CHECK_EQUAL(get<2>(lFsmStateMap.at("state0").at("hardReset")).size(), 1);
  BOOST_CHECK_EQUAL(get<2>(lFsmStateMap.at("state0").at("hardReset")).at(0).first, "dummyCommand");
  BOOST_CHECK_EQUAL(get<2>(lFsmStateMap.at("state0").at("hardReset")).at(0).second, "");

  BOOST_CHECK_EQUAL(lFsmStateMap.at("state0").count("null"), 1);
  BOOST_CHECK_EQUAL(get<0>(lFsmStateMap.at("state0").at("null")), "");
  BOOST_CHECK_EQUAL(get<1>(lFsmStateMap.at("state0").at("null")), "state0");
  BOOST_CHECK(get<2>(lFsmStateMap.at("state0").at("null")).empty());

  BOOST_CHECK_EQUAL(lFsmStateMap.at("state0").count("configurePart1"), 1);
  BOOST_CHECK_EQUAL(get<0>(lFsmStateMap.at("state0").at("configurePart1")), "");
  BOOST_CHECK_EQUAL(get<1>(lFsmStateMap.at("state0").at("configurePart1")), "alpha");
  BOOST_CHECK_EQUAL(get<2>(lFsmStateMap.at("state0").at("configurePart1")).size(), 2);
  BOOST_CHECK_EQUAL(get<2>(lFsmStateMap.at("state0").at("configurePart1")).at(0).first, "dummyCommand");
  BOOST_CHECK_EQUAL(get<2>(lFsmStateMap.at("state0").at("configurePart1")).at(0).second, "aNamespace");
  BOOST_CHECK_EQUAL(get<2>(lFsmStateMap.at("state0").at("configurePart1")).at(1).first, "dummyCommand");
  BOOST_CHECK_EQUAL(get<2>(lFsmStateMap.at("state0").at("configurePart1")).at(1).second, "");

  BOOST_CHECK_EQUAL(lFsmStateMap.at("alpha").size(), 1);
  BOOST_CHECK_EQUAL(lFsmStateMap.at("alpha").count("configurePart2"), 1);
  BOOST_CHECK_EQUAL(get<0>(lFsmStateMap.at("alpha").at("configurePart2")), "");
  BOOST_CHECK_EQUAL(get<1>(lFsmStateMap.at("alpha").at("configurePart2")), "beta");
  BOOST_CHECK_EQUAL(get<2>(lFsmStateMap.at("alpha").at("configurePart2")).size(), 2);
  BOOST_CHECK_EQUAL(get<2>(lFsmStateMap.at("alpha").at("configurePart2")).at(0).first, "dummyCommand2");
  BOOST_CHECK_EQUAL(get<2>(lFsmStateMap.at("alpha").at("configurePart2")).at(0).second, "otherNamespace");
  BOOST_CHECK_EQUAL(get<2>(lFsmStateMap.at("alpha").at("configurePart2")).at(1).first, "dummyCommand");
  BOOST_CHECK_EQUAL(get<2>(lFsmStateMap.at("alpha").at("configurePart2")).at(1).second, "");

  BOOST_CHECK(lFsmStateMap.at("beta").empty());
  BOOST_CHECK(lFsmStateMap.at("errorState").empty());
}


BOOST_AUTO_TEST_CASE(TestEncodeCommandResult)
{
  using action::Command;
  using action::CommandSnapshot;
  using core::ParameterSet;

  CommandSnapshot::IdAliasPair lCommandIdAlias("someDevice.aCommand", "command's alias");
  CommandSnapshot::IdAliasPair lDeviceIdAlias("someDevice", "my device");
  std::shared_ptr<const boost::any> lResult(new boost::any(int(42)));

  CommandSnapshot lSnapshot(lCommandIdAlias, lDeviceIdAlias, Command::kError, 42.5, 0.85, "Some informative status message", ParameterSet(), ParameterSet(), lResult);

  msgpack::sbuffer lBuffer;
  encodeCommandResult(lSnapshot, lBuffer);

  msgpack::object_handle lHandle = msgpack::unpack(lBuffer.data(), lBuffer.size());
  msgpack::object lObj = lHandle.get();

  BOOST_REQUIRE_EQUAL(lObj.as<std::vector<msgpack::object>>().size(), 2);
  BOOST_CHECK_EQUAL(lObj.as<std::vector<msgpack::object>>().at(0).as<std::string>(), "ok");

  std::map<std::string, msgpack::object> lSnapshotInfoMap;
  lObj.as<std::vector<msgpack::object>>().at(1).convert(lSnapshotInfoMap);

  BOOST_CHECK_EQUAL(lSnapshotInfoMap.size(), 5);
  BOOST_CHECK_EQUAL(lSnapshotInfoMap.at("path").as<std::string>(), "someDevice.aCommand");
  BOOST_CHECK_EQUAL(lSnapshotInfoMap.at("state").as<std::string>(), "Error");
  BOOST_CHECK_CLOSE(lSnapshotInfoMap.at("duration").as<float>(), 42.5, 0.00001);
  BOOST_CHECK_CLOSE(lSnapshotInfoMap.at("progress").as<float>(), 0.85, 0.00001);
  BOOST_CHECK_EQUAL(lSnapshotInfoMap.at("message").as<std::string>(), "Some informative status message");
  // BOOST_CHECK_EQUAL(lSnapshotInfoMap.at("result").as<int>(), 42);
  // BOOST_CHECK_EQUAL((lSnapshotInfoMap.at("execution-details").as<std::map<std::string,msgpack::object>>().size()), 0);
}

BOOST_AUTO_TEST_CASE(TestEncodeTransitionResultEmpty)
{
  using action::Transition;
  using action::TransitionSnapshot;
  using core::ParameterSet;

  TransitionSnapshot::IdAliasPair lActionIdAlias("someDevice.aFSM.someState.aTransition", "transition's alias");
  TransitionSnapshot::IdAliasPair lDeviceIdAlias("someDevice", "my device");

  TransitionSnapshot lSnapshot(lActionIdAlias, lDeviceIdAlias, Transition::kDone, 12.3, NULL, {}, 0);

  msgpack::sbuffer lBuffer;
  encodeTransitionResult(lSnapshot, lBuffer);

  msgpack::object_handle lHandle = msgpack::unpack(lBuffer.data(), lBuffer.size());
  msgpack::object lObj = lHandle.get();

  BOOST_REQUIRE_EQUAL(lObj.as<std::vector<msgpack::object>>().size(), 2);
  BOOST_CHECK_EQUAL(lObj.as<std::vector<msgpack::object>>().at(0).as<std::string>(), "ok");

  std::map<std::string, msgpack::object> lSnapshotInfoMap;
  lObj.as<std::vector<msgpack::object>>().at(1).convert(lSnapshotInfoMap);

  BOOST_CHECK_EQUAL(lSnapshotInfoMap.size(), 7);
  BOOST_CHECK_EQUAL(lSnapshotInfoMap.at("path").as<std::string>(), "someDevice.aFSM.someState.aTransition");
  BOOST_CHECK_EQUAL(lSnapshotInfoMap.at("state").as<std::string>(), "Done");
  BOOST_CHECK_CLOSE(lSnapshotInfoMap.at("duration").as<float>(), 12.3, 0.00001);
  BOOST_CHECK_CLOSE(lSnapshotInfoMap.at("progress").as<float>(), 1.0, 0.00001);
  BOOST_CHECK_EQUAL(lSnapshotInfoMap.at("num-commands-done").as<float>(), 0);
  BOOST_CHECK_EQUAL(lSnapshotInfoMap.at("num-commands-total").as<float>(), 0);
  BOOST_CHECK_EQUAL(lSnapshotInfoMap.at("commands").as<std::vector<msgpack::object>>().size(), 0);
}


BOOST_AUTO_TEST_CASE(TestEncodeTransitionResult)
{
  using action::Command;
  using action::CommandSnapshot;
  using action::Transition;
  using action::TransitionSnapshot;
  using core::ParameterSet;

  TransitionSnapshot::IdAliasPair lActionIdAlias("someDevice.aFSM.someState.aTransition", "transition's alias");
  TransitionSnapshot::IdAliasPair lDeviceIdAlias("someDevice", "my device");

  std::vector<CommandSnapshot> lCmdSnapshots;
  std::shared_ptr<const boost::any> lResult(new boost::any(int(42)));
  lCmdSnapshots.push_back(CommandSnapshot({"someDevice.aCommand", "cmd's alias"}, {"someDevice", "my device"}, Command::kDone, 4.2, 1.0, "A status msg", ParameterSet(), ParameterSet(), lResult));
  lCmdSnapshots.push_back(CommandSnapshot({"someDevice.commandB", "my alias"}, {"someDevice", "my device"}, Command::kError, 5.6, 0.56, "Some message", ParameterSet(), ParameterSet(), lResult));

  TransitionSnapshot lSnapshot(lActionIdAlias, lDeviceIdAlias, Transition::kError, 10.1, NULL, lCmdSnapshots, 3);

  msgpack::sbuffer lBuffer;
  encodeTransitionResult(lSnapshot, lBuffer);

  msgpack::object_handle lHandle = msgpack::unpack(lBuffer.data(), lBuffer.size());
  msgpack::object lObj = lHandle.get();

  BOOST_REQUIRE_EQUAL(lObj.as<std::vector<msgpack::object>>().size(), 2);
  BOOST_CHECK_EQUAL(lObj.as<std::vector<msgpack::object>>().at(0).as<std::string>(), "ok");

  std::map<std::string, msgpack::object> lSnapshotInfoMap;
  lObj.as<std::vector<msgpack::object>>().at(1).convert(lSnapshotInfoMap);

  BOOST_CHECK_EQUAL(lSnapshotInfoMap.size(), 7);
  BOOST_CHECK_EQUAL(lSnapshotInfoMap.at("path").as<std::string>(), "someDevice.aFSM.someState.aTransition");
  BOOST_CHECK_EQUAL(lSnapshotInfoMap.at("state").as<std::string>(), "Error");
  BOOST_CHECK_CLOSE(lSnapshotInfoMap.at("duration").as<float>(), 10.1, 0.00001);
  BOOST_CHECK_CLOSE(lSnapshotInfoMap.at("progress").as<float>(), 1.56 / 3, 0.00001);
  BOOST_CHECK_EQUAL(lSnapshotInfoMap.at("num-commands-done").as<float>(), 2);
  BOOST_CHECK_EQUAL(lSnapshotInfoMap.at("num-commands-total").as<float>(), 3);
  BOOST_CHECK_EQUAL(lSnapshotInfoMap.at("commands").as<std::vector<msgpack::object>>().size(), 2);

  std::map<std::string, msgpack::object> lCmdSnapshotInfoMap;
  lSnapshotInfoMap.at("commands").as<std::vector<msgpack::object>>().at(0).convert(lCmdSnapshotInfoMap);
  BOOST_CHECK_EQUAL(lCmdSnapshotInfoMap.size(), 5);
  BOOST_CHECK_EQUAL(lCmdSnapshotInfoMap.at("path").as<std::string>(), "someDevice.aCommand");
  BOOST_CHECK_EQUAL(lCmdSnapshotInfoMap.at("state").as<std::string>(), "Done");
  BOOST_CHECK_CLOSE(lCmdSnapshotInfoMap.at("duration").as<float>(), 4.2, 0.00001);
  BOOST_CHECK_CLOSE(lCmdSnapshotInfoMap.at("progress").as<float>(), 1.0, 0.00001);
  BOOST_CHECK_EQUAL(lCmdSnapshotInfoMap.at("message").as<std::string>(), "A status msg");

  lSnapshotInfoMap.at("commands").as<std::vector<msgpack::object>>().at(1).convert(lCmdSnapshotInfoMap);
  BOOST_CHECK_EQUAL(lCmdSnapshotInfoMap.size(), 5);
  BOOST_CHECK_EQUAL(lCmdSnapshotInfoMap.at("path").as<std::string>(), "someDevice.commandB");
  BOOST_CHECK_EQUAL(lCmdSnapshotInfoMap.at("state").as<std::string>(), "Error");
  BOOST_CHECK_CLOSE(lCmdSnapshotInfoMap.at("duration").as<float>(), 5.6, 0.00001);
  BOOST_CHECK_CLOSE(lCmdSnapshotInfoMap.at("progress").as<float>(), 0.56, 0.00001);
  BOOST_CHECK_EQUAL(lCmdSnapshotInfoMap.at("message").as<std::string>(), "Some message");
}


class ManyParameterDummyCommand : public action::test::DummyCommand {
public:

  ManyParameterDummyCommand(const std::string& aId, action::ActionableObject& aActionable) :
    DummyCommand(aId, aActionable)
  {
    registerParameter<bool>("b", true);
    registerParameter<unsigned int>("u", 42);
    registerParameter<float>("f", 3.14);

    registerParameter<std::vector<int>>("vec_i", {-42});
    registerParameter<std::vector<std::string>>("vec_s", {"x"});
    registerParameter<std::vector<bool>>("vec_b", {true});
    registerParameter<std::vector<unsigned int>>("vec_u", {42});
    registerParameter<std::vector<float>>("vec_f", {3.14});
  }
};



BOOST_FIXTURE_TEST_CASE(TestExtractParameterSet, UtilitiesTestSetup)
{

  std::map<std::string, msgpack::object> lObjectMap;
  msgpack::zone lZone;

  lObjectMap["x"] = msgpack::object(int(15), lZone);
  lObjectMap["todo"] = msgpack::object(std::string("A string!"), lZone);
  lObjectMap["b"] = msgpack::object(bool(false), lZone);
  lObjectMap["u"] = msgpack::object(unsigned(99), lZone);
  lObjectMap["f"] = msgpack::object(float(1.23), lZone);

  lObjectMap["vec_i"] = msgpack::object(std::vector<int>{99, -33, 66}, lZone);
  lObjectMap["vec_s"] = msgpack::object(std::vector<std::string>{"one", "two"}, lZone);
  lObjectMap["vec_b"] = msgpack::object(std::vector<bool>{false, true, true, false}, lZone);
  lObjectMap["vec_u"] = msgpack::object(std::vector<unsigned>{42}, lZone);
  lObjectMap["vec_f"] = msgpack::object(std::vector<float>{1.23}, lZone);

  // Function should throw if msgpack object map contains a superfluous entry
  BOOST_CHECK_THROW(extractParameterSet(commandA2, msgpack::object(lObjectMap, lZone)), std::runtime_error);

  auto& command = deviceA.registerCommand<ManyParameterDummyCommand>("manyParamCommand");
  core::ParameterSet lParameterSet = extractParameterSet(command, msgpack::object(lObjectMap, lZone));

  BOOST_CHECK_EQUAL(lParameterSet.size(), 10);
  BOOST_CHECK(lParameterSet.has("x"));
  BOOST_CHECK_EQUAL(lParameterSet.get<int>("x"), 15);
  BOOST_CHECK(lParameterSet.has("todo"));
  BOOST_CHECK_EQUAL(lParameterSet.get<std::string>("todo"), "A string!");
  BOOST_CHECK(lParameterSet.has("b"));
  BOOST_CHECK_EQUAL(lParameterSet.get<bool>("b"), false);
  BOOST_CHECK(lParameterSet.has("u"));
  BOOST_CHECK_EQUAL(lParameterSet.get<unsigned>("u"), 99);
  BOOST_CHECK(lParameterSet.has("f"));
  BOOST_CHECK_CLOSE(lParameterSet.get<float>("f"), 1.23, 0.00001);

  BOOST_CHECK(lParameterSet.has("vec_i"));
  BOOST_CHECK_EQUAL(lParameterSet.get<std::vector<int>>("vec_i").size(), 3);
  BOOST_CHECK_EQUAL(lParameterSet.get<std::vector<int>>("vec_i").at(0), 99);
  BOOST_CHECK_EQUAL(lParameterSet.get<std::vector<int>>("vec_i").at(1), -33);
  BOOST_CHECK_EQUAL(lParameterSet.get<std::vector<int>>("vec_i").at(2), 66);
  BOOST_CHECK(lParameterSet.has("vec_s"));
  BOOST_CHECK_EQUAL(lParameterSet.get<std::vector<std::string>>("vec_s").size(), 2);
  BOOST_CHECK_EQUAL(lParameterSet.get<std::vector<std::string>>("vec_s").at(0), "one");
  BOOST_CHECK_EQUAL(lParameterSet.get<std::vector<std::string>>("vec_s").at(1), "two");
  BOOST_CHECK(lParameterSet.has("vec_b"));
  BOOST_CHECK_EQUAL(lParameterSet.get<std::vector<bool>>("vec_b").size(), 4);
  BOOST_CHECK_EQUAL(lParameterSet.get<std::vector<bool>>("vec_b").at(0), false);
  BOOST_CHECK_EQUAL(lParameterSet.get<std::vector<bool>>("vec_b").at(1), true);
  BOOST_CHECK_EQUAL(lParameterSet.get<std::vector<bool>>("vec_b").at(2), true);
  BOOST_CHECK_EQUAL(lParameterSet.get<std::vector<bool>>("vec_b").at(3), false);
  BOOST_CHECK(lParameterSet.has("vec_u"));
  BOOST_CHECK_EQUAL(lParameterSet.get<std::vector<unsigned>>("vec_u").size(), 1);
  BOOST_CHECK_EQUAL(lParameterSet.get<std::vector<unsigned>>("vec_u").at(0), 42);
  BOOST_CHECK(lParameterSet.has("vec_f"));
  BOOST_CHECK_EQUAL(lParameterSet.get<std::vector<float>>("vec_f").size(), 1);
  BOOST_CHECK_CLOSE(lParameterSet.get<std::vector<float>>("vec_f").at(0), 1.23, 0.00001);
}

BOOST_FIXTURE_TEST_CASE(TestExtractParameterSets, UtilitiesTestSetup)
{
  std::vector<std::map<std::string, msgpack::object>> lObjectMaps;
  msgpack::zone lZone;

  const action::Transition& lEmptyTransition = deviceA.getStateMachine("myFSM").getTransition("state0", "null");
  std::vector<core::ParameterSet> lParamSets = extractParameterSets(lEmptyTransition, msgpack::object(lObjectMaps, lZone));
  BOOST_CHECK(lParamSets.empty());

  const action::Transition& lTransition = deviceA.getStateMachine("myFSM").getTransition("alpha", "configurePart2");
  BOOST_CHECK_THROW(extractParameterSets(lTransition, msgpack::object(lObjectMaps, lZone)), std::runtime_error);

  lObjectMaps.resize(2);

  lObjectMaps.at(1)["x"] = msgpack::object(int(-42), lZone);
  lObjectMaps.at(1)["todo"] = msgpack::object(std::string("Some string!"), lZone);

  lParamSets = extractParameterSets(lTransition, msgpack::object(lObjectMaps, lZone));
  BOOST_CHECK_EQUAL(lParamSets.size(), 2);
  BOOST_CHECK_EQUAL(lParamSets.at(0).size(), 0);
  BOOST_CHECK_EQUAL(lParamSets.at(1).size(), 2);
  BOOST_CHECK(lParamSets.at(1).has("x"));
  BOOST_CHECK_EQUAL(lParamSets.at(1).get<int>("x"), -42);
  BOOST_CHECK(lParamSets.at(1).has("todo"));
  BOOST_CHECK_EQUAL(lParamSets.at(1).get<std::string>("todo"), "Some string!");

  lObjectMaps.at(0)["x"] = msgpack::object(int(-42));
  BOOST_CHECK_THROW(extractParameterSets(lTransition, msgpack::object(lObjectMaps, lZone)), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END() // ServerTestSuite

} /* namespace test */
} /* namespace server */
} /* namespace swatch */
