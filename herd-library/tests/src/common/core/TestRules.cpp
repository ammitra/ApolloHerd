#include <boost/test/unit_test.hpp>
#include <iostream>

// boost headers
#include "boost/lexical_cast.hpp"
#include <boost/filesystem.hpp>

// SWATCH headers
#include "swatch/core/rules/None.hpp"
#include "swatch/core/rules/FiniteNumber.hpp"
#include "swatch/core/rules/FiniteVector.hpp"
#include "swatch/core/rules/IsAmong.hpp"
#include "swatch/core/rules/GreaterThan.hpp"
#include "swatch/core/rules/LesserThan.hpp"
#include "swatch/core/rules/InRange.hpp"
#include "swatch/core/rules/OutOfRange.hpp"
#include "swatch/core/rules/OfSize.hpp"
#include "swatch/core/rules/And.hpp"
#include "swatch/core/rules/Or.hpp"
#include "swatch/core/rules/Not.hpp"
#include "swatch/core/rules/PathExists.hpp"


namespace swatch {
namespace core {
namespace test {

struct RuleTestSetup {

  int intOK;
  uint32_t uintOK;

  float floatNaN;
  float floatOK;
  double doubleNaN;
  double doubleOK;

  //---
  std::vector<int> vIntEmpty;
  std::vector<int> vIntOK;

  std::vector<uint32_t> vUintEmpty;
  std::vector<uint32_t> vUintOK;

  std::vector<float> vFloatEmpty;
  std::vector<float> vFloatNaN;
  std::vector<float> vFloatOK;

  std::vector<double> vDoubleEmpty;
  std::vector<double> vDoubleNaN;
  std::vector<double> vDoubleOK;

  std::string strEmpty;
  std::string strFull;
  std::string strOptA;
  std::string strOptD;

  RuleTestSetup() : 
    intOK(-123),
    uintOK(56.),
    floatNaN(NAN),
    floatOK(-5.),
    doubleNaN(NAN),
    doubleOK(90.),

    vIntEmpty(),
    vUintEmpty(),
    vFloatEmpty(),
    vDoubleEmpty(),

    strEmpty(),
    strFull("I am a string"),
    strOptA("A"),
    strOptD("D")
  {
    for ( int i : { 1, -2, 3 } ) {
       vIntOK.push_back(i);
    }

    for ( int u : { 10, 20, 30 } ) {
       vUintOK.push_back(u);
    }

    for ( int f : { 5., 6., -7. } ) {
       vFloatNaN.push_back(f);
       vFloatOK.push_back(f);
    }
    vFloatNaN.push_back(NAN);

    for ( int d : { 50., 60., -70. } ) {
       vDoubleNaN.push_back(d);
       vDoubleOK.push_back(d);
    }
    vDoubleNaN.push_back(NAN);
  }

};


BOOST_AUTO_TEST_SUITE(RuleTestSuite)

BOOST_FIXTURE_TEST_CASE(SimpleNumericTest, RuleTestSetup)
{
  rules::FiniteNumber<float> lFloatRule;
  BOOST_CHECK(lFloatRule.type() == typeid(float));
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(lFloatRule), "isFinite(x)");


  BOOST_CHECK_EQUAL(lFloatRule(floatNaN), false);
  BOOST_CHECK_EQUAL(lFloatRule(floatOK), true);

  BOOST_CHECK_THROW(lFloatRule(uintOK), RuleTypeMismatch);
  BOOST_CHECK_THROW(lFloatRule(intOK), RuleTypeMismatch);
  BOOST_CHECK_THROW(lFloatRule(doubleNaN), RuleTypeMismatch);
  BOOST_CHECK_THROW(lFloatRule(doubleOK), RuleTypeMismatch);
  BOOST_CHECK_THROW(lFloatRule(strFull), RuleTypeMismatch);
  BOOST_CHECK_THROW(lFloatRule(vIntEmpty), RuleTypeMismatch);
}


BOOST_FIXTURE_TEST_CASE(SimpleVectorTest, RuleTestSetup)
{
  rules::FiniteVector<std::vector<float>> lFloatVecRule;
  BOOST_CHECK(lFloatVecRule.type() == typeid(std::vector<float>));
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(lFloatVecRule), "all(x,isFinite)");

  BOOST_CHECK_EQUAL(lFloatVecRule(vFloatEmpty), true);
  BOOST_CHECK_EQUAL(lFloatVecRule(vFloatOK), true);

  BOOST_CHECK_THROW(lFloatVecRule(floatNaN), RuleTypeMismatch);
  BOOST_CHECK_THROW(lFloatVecRule(floatOK), RuleTypeMismatch);
  BOOST_CHECK_THROW(lFloatVecRule(doubleNaN), RuleTypeMismatch);
  BOOST_CHECK_THROW(lFloatVecRule(doubleOK), RuleTypeMismatch);
  BOOST_CHECK_THROW(lFloatVecRule(strFull), RuleTypeMismatch);
  BOOST_CHECK_THROW(lFloatVecRule(vIntEmpty), RuleTypeMismatch);

  rules::OfSize<std::vector<uint32_t>> lUintVecOsSizeRule(3);


  // vUintOK has size 3
  BOOST_CHECK_EQUAL(lUintVecOsSizeRule(vUintOK), true);
}


BOOST_FIXTURE_TEST_CASE(NoneTest, RuleTestSetup)
{
  rules::None<double> lNoDoubleRule;
  BOOST_CHECK(lNoDoubleRule.type() == typeid(double));
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(lNoDoubleRule), "true");

  BOOST_CHECK_EQUAL(lNoDoubleRule(doubleNaN), true);
  BOOST_CHECK_EQUAL(lNoDoubleRule(doubleOK), true);

  BOOST_CHECK_THROW(lNoDoubleRule(floatNaN), RuleTypeMismatch);
  BOOST_CHECK_THROW(lNoDoubleRule(floatOK), RuleTypeMismatch);
  BOOST_CHECK_THROW(lNoDoubleRule(strFull), RuleTypeMismatch);
  BOOST_CHECK_THROW(lNoDoubleRule(vIntEmpty), RuleTypeMismatch);
  BOOST_CHECK_THROW(lNoDoubleRule(vDoubleEmpty), RuleTypeMismatch);
}


BOOST_FIXTURE_TEST_CASE(IsAmongTest, RuleTestSetup)
{
  rules::IsAmong lRule({"A", "B", "C"});
  BOOST_CHECK(lRule.type() == typeid(std::string));
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(lRule), "x in {A, B, C}");  

  BOOST_CHECK_EQUAL(lRule(strOptA), true);
  BOOST_CHECK_EQUAL(lRule(strOptD), false);

}


BOOST_FIXTURE_TEST_CASE(GreaterThanTest, RuleTestSetup)
{

  rules::GreaterThan<uint32_t> lUintGT(100);
  
  BOOST_CHECK(lUintGT.type() == typeid(uint32_t));
  BOOST_CHECK_EQUAL(lUintGT(uint32_t(120)), true);
  BOOST_CHECK_EQUAL(lUintGT(uint32_t(80)), false);

  rules::GreaterThan<int> lIntGT(-100);

  BOOST_CHECK(lIntGT.type() == typeid(int));
  BOOST_CHECK_EQUAL(lIntGT(int(-80)), true);
  BOOST_CHECK_EQUAL(lIntGT(int(-120)), false);

  rules::GreaterThan<float> lFloatGT(-100);

  BOOST_CHECK(lFloatGT.type() == typeid(float));
  BOOST_CHECK_EQUAL(lFloatGT(float(-80)), true);
  BOOST_CHECK_EQUAL(lFloatGT(float(-120)), false);

  rules::GreaterThan<double> lDoubleGT(-100);

  BOOST_CHECK(lDoubleGT.type() == typeid(double));
  BOOST_CHECK_EQUAL(lDoubleGT(double(-80)), true);
  BOOST_CHECK_EQUAL(lDoubleGT(double(-120)), false);
}


BOOST_FIXTURE_TEST_CASE(LesserThanTest, RuleTestSetup)
{

  rules::LesserThan<uint32_t> lUintLT(100);
  
  BOOST_CHECK(lUintLT.type() == typeid(uint32_t));
  BOOST_CHECK_EQUAL(lUintLT(uint32_t(120)), false);
  BOOST_CHECK_EQUAL(lUintLT(uint32_t(80)), true);

  rules::LesserThan<int> lIntLT(-100);

  BOOST_CHECK(lIntLT.type() == typeid(int));
  BOOST_CHECK_EQUAL(lIntLT(int(-80)), false);
  BOOST_CHECK_EQUAL(lIntLT(int(-120)), true);

  rules::LesserThan<float> lFloatLT(-100);

  BOOST_CHECK(lFloatLT.type() == typeid(float));
  BOOST_CHECK_EQUAL(lFloatLT(float(-80)), false);
  BOOST_CHECK_EQUAL(lFloatLT(float(-120)), true);

  rules::LesserThan<double> lDoubleLT(-100);

  BOOST_CHECK(lDoubleLT.type() == typeid(double));
  BOOST_CHECK_EQUAL(lDoubleLT(double(-80)), false);
  BOOST_CHECK_EQUAL(lDoubleLT(double(-120)), true);
}

BOOST_FIXTURE_TEST_CASE(InRangeTest, RuleTestSetup)
{

  rules::InRange<uint32_t> lUintRng(100,200);
  
  BOOST_CHECK(lUintRng.type() == typeid(uint32_t));
  BOOST_CHECK_EQUAL(lUintRng(uint32_t(120)), true);
  BOOST_CHECK_EQUAL(lUintRng(uint32_t(80)), false);

  rules::InRange<int> lIntRng(-200, -100);

  BOOST_CHECK(lIntRng.type() == typeid(int));
  BOOST_CHECK_EQUAL(lIntRng(int(-80)), false);
  BOOST_CHECK_EQUAL(lIntRng(int(-120)), true);

  rules::InRange<float> lFloatRng(-200, -100);

  BOOST_CHECK(lFloatRng.type() == typeid(float));
  BOOST_CHECK_EQUAL(lFloatRng(float(-80)), false);
  BOOST_CHECK_EQUAL(lFloatRng(float(-120)), true);

  rules::InRange<double> lDoubleRng(-200, -100);

  BOOST_CHECK(lDoubleRng.type() == typeid(double));
  BOOST_CHECK_EQUAL(lDoubleRng(double(-80)), false);
  BOOST_CHECK_EQUAL(lDoubleRng(double(-120)), true);
}


// ----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(OutOfRangeTest, RuleTestSetup)
{

  rules::OutOfRange<uint32_t> lUintRng(100,200);
  
  BOOST_CHECK(lUintRng.type() == typeid(uint32_t));
  BOOST_CHECK_EQUAL(lUintRng(uint32_t(120)), false);
  BOOST_CHECK_EQUAL(lUintRng(uint32_t(80)), true);

  rules::OutOfRange<int> lIntRng(-200, -100);

  BOOST_CHECK(lIntRng.type() == typeid(int));
  BOOST_CHECK_EQUAL(lIntRng(int(-80)), true);
  BOOST_CHECK_EQUAL(lIntRng(int(-120)), false);

  rules::OutOfRange<float> lFloatRng(-200, -100);

  BOOST_CHECK(lFloatRng.type() == typeid(float));
  BOOST_CHECK_EQUAL(lFloatRng(float(-80)), true);
  BOOST_CHECK_EQUAL(lFloatRng(float(-120)), false);

  rules::OutOfRange<double> lDoubleRng(-200, -100);

  BOOST_CHECK(lDoubleRng.type() == typeid(double));
  BOOST_CHECK_EQUAL(lDoubleRng(double(-80)), true);
  BOOST_CHECK_EQUAL(lDoubleRng(double(-120)), false);
}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(AndTest, RuleTestSetup)
{
  rules::GreaterThan<uint32_t> lUintGT100(100);
  rules::LesserThan<uint32_t> lUintLT200(200);

  rules::And<uint32_t> lAnd(lUintGT100,lUintLT200);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(lAnd), "(x > 100 && x < 200)");

  BOOST_CHECK_EQUAL(lAnd(uint32_t(120)), true);
  BOOST_CHECK_EQUAL(lAnd(uint32_t(80)), false);
  BOOST_CHECK_EQUAL(lAnd(uint32_t(250)), false);

  // TODO: Improve this test
  rules::And<uint32_t> lAnd2(lAnd, lAnd);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(lAnd2), "((x > 100 && x < 200) && (x > 100 && x < 200))");

}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(OrTest, RuleTestSetup)
{
  rules::GreaterThan<uint32_t> lUintGT100(100);
  rules::LesserThan<uint32_t> lUintLT200(200);

  rules::Or<uint32_t> lOr(lUintGT100,lUintLT200);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(lOr), "(x > 100 || x < 200)");

  BOOST_CHECK_EQUAL(lOr(uint32_t(120)), true);
  BOOST_CHECK_EQUAL(lOr(uint32_t(80)), true);
  BOOST_CHECK_EQUAL(lOr(uint32_t(250)), true);

  // TODO: Improve this test
  rules::Or<uint32_t> lOr2(lOr, lOr);
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(lOr2), "((x > 100 || x < 200) || (x > 100 || x < 200))");

}
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(NotTest, RuleTestSetup)
{
  rules::GreaterThan<int> lUintGT100(100);
  rules::Not<int> lNotUintGT100(lUintGT100);

  BOOST_CHECK_EQUAL(lNotUintGT100(int(120)), false);

}
// ----------------------------------------------------------------------------

 
// ----------------------------------------------------------------------------
BOOST_FIXTURE_TEST_CASE(PathExistsTest, RuleTestSetup)
{
  // char* lEnvVar = std::getenv("SWATCH_ROOT");
  std::string lSwatchRoot = std::getenv("HERD_ROOT");
  boost::filesystem::path lBasePath(lSwatchRoot);
  lBasePath /= "tests";

  std::cout << "base path " << lBasePath << std::endl;
  rules::PathExists lExistsPlain, lExistsInTest(lBasePath.string()), lShExistsInTest(lBasePath.string(), "sh");

  // Existsing paths
  for ( const auto& lPath : {lBasePath, lBasePath/"env.sh"} ) {
    BOOST_CHECK_EQUAL(lExistsPlain(lPath.string()), true);
  }

  // Non-existing paths
  for ( const auto& lPath : { lBasePath/"nowhere/", lBasePath/"nothing"} ) {
    BOOST_CHECK_EQUAL(lExistsPlain(lPath.string()), false);
  }
  
  // Existing in test
  for ( const auto& lPath : {"env.sh"} ) {
    BOOST_CHECK_EQUAL(lExistsInTest(std::string(lPath)), true);
  }

  // Non existing in test
  for ( const auto& lPath : {"nowhere/", "nothing"} ) {
    BOOST_CHECK_EQUAL(lExistsInTest(std::string(lPath)), false);
  }

  // Existing in test with extension sh
  for ( const auto& lPath : {"env"} ) {
    BOOST_CHECK_EQUAL(lShExistsInTest(std::string(lPath)), true);
  }
}
// ----------------------------------------------------------------------------


BOOST_AUTO_TEST_SUITE_END() // XRuleTestSuite
// ----------------------------------------------------------------------------

} /* namespace test */
} /* namespace core */
} /* namespace swatch */
