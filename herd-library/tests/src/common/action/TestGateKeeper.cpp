/**
 * \brief Test suite for swatch/action/GateKeeper
 * \author kreczko
 */
// Boost Unit Test includes
#include <boost/test/unit_test.hpp>


#include <boost/any.hpp>

// swatch headers
#include "swatch/action/GateKeeper.hpp"
#include "swatch/action/test/DummyGateKeeper.hpp"


namespace swatch {
namespace action {
namespace test {

struct TestGateKeeperSetup {
  TestGateKeeperSetup() :
    gk()
  {
    typedef std::shared_ptr<const boost::any> ParamPtr_t;
    typedef GateKeeper::Parameters_t Parameters_t;

    GateKeeper::ParametersContext_t lCommonParams(new Parameters_t{
      {"hello", ParamPtr_t(new boost::any(std::string("World")))},
      {"answer", ParamPtr_t(new boost::any(int(42)))}
    });
    gk.addContext("dummy_sys.common", lCommonParams);

    GateKeeper::ParametersContext_t lChildA1Params(new Parameters_t{
      {"hello", ParamPtr_t(new boost::any(std::string("World! (childA1)")))}
    });
    gk.addContext("dummy_sys.childA1", lChildA1Params);

    GateKeeper::ParametersContext_t lChildTypeAParams(new GateKeeper::Parameters_t{
      {"sequence_1.command_1.parameter_1", ParamPtr_t(new boost::any(std::string("sequence")))},
      {"command_1.parameter_1", ParamPtr_t(new boost::any(std::string("command")))}
    });
    gk.addContext("dummy_sys.childTypeA", lChildTypeAParams);
  }

  DummyGateKeeper gk;
};


BOOST_AUTO_TEST_SUITE( TestGateKeeper )


BOOST_FIXTURE_TEST_CASE ( TestCommonParameters, TestGateKeeperSetup )
{
  std::vector<std::string> lContextsToLookIn;
  lContextsToLookIn.push_back("dummy_sys.common");

  GateKeeper::Parameter_t lParam(gk.get("", "", "hello", lContextsToLookIn));
  BOOST_REQUIRE(lParam != NULL);
  BOOST_CHECK_EQUAL(boost::any_cast<std::string>(*lParam), "World");

  GateKeeper::Parameter_t lParam2(gk.get("", "", "answer", lContextsToLookIn));
  BOOST_REQUIRE(lParam2 != NULL);
  BOOST_CHECK_EQUAL(boost::any_cast<int>(*lParam2), 42);
}

BOOST_FIXTURE_TEST_CASE ( TestChildParameters, TestGateKeeperSetup )
{
  std::vector<std::string> lContextsToLookIn {"dummy_sys.childA1", "dummy_sys.common"};

  GateKeeper::Parameter_t p(gk.get("", "", "hello", lContextsToLookIn));
  BOOST_REQUIRE(p != NULL);
  BOOST_CHECK_EQUAL(boost::any_cast<std::string>(*p), "World! (childA1)");
}

BOOST_FIXTURE_TEST_CASE ( TestSequenceParameters, TestGateKeeperSetup )
{
  std::vector<std::string> lContextsToLookIn;
  lContextsToLookIn.push_back("dummy_sys.childTypeA");

  GateKeeper::Parameter_t p(
    gk.get("sequence_1", "command_1", "parameter_1", lContextsToLookIn));
  BOOST_REQUIRE(p != NULL);
  BOOST_CHECK_EQUAL(boost::any_cast<std::string>(*p), "sequence");

  GateKeeper::Parameter_t p2(
    gk.get("", "command_1", "parameter_1", lContextsToLookIn));
  BOOST_REQUIRE(p2 != NULL);
  BOOST_CHECK_EQUAL(boost::any_cast<std::string>(*p2), "command");
}


BOOST_AUTO_TEST_SUITE_END() // TestGateKeeper

} //ns: test
} //ns: action
} //ns: swatch
