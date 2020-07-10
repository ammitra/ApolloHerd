#include <boost/test/unit_test.hpp>


#include "boost/lexical_cast.hpp"

#include "swatch/core/ParameterSet.hpp"
#include "swatch/core/ParameterSet.hpp"
#include "swatch/core/PSetConstraint.hpp"


namespace swatch {
namespace core {
namespace test {



class DummyNullConstraint : public PSetConstraint
{
public:
  DummyNullConstraint() = default; 
  ~DummyNullConstraint() = default;
  
  // Expose 'define' method as public method for unit tests
  template <typename T>
  void require(const std::string& aName) { this->PSetConstraint::require<T>(aName);}
  
private:
  void describe(std::ostream& aStream) const override
  {
    aStream << "null constraint";
  }
  
  Match verify(const ParameterSet& aParams) const override
  {
    return true;
  }
};

// ----------------------------------------------------------------------------
class DummySumSmallerThan : public PSetConstraint
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
  
  Match verify(const ParameterSet& aParams) const override
  {
    return ( aParams.get<uint32_t>(a) + aParams.get<uint32_t>(b) ) < max;
  }

  const std::string a;  
  const std::string b;
  const size_t max;

};
// ----------------------------------------------------------------------------



BOOST_AUTO_TEST_SUITE(ConstraintTestSuite)

BOOST_AUTO_TEST_CASE(TestStreamOperator)
{
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(DummyNullConstraint()), "null constraint");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(DummySumSmallerThan("a", "b", 42)), "(a + b) < 42");
  BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(DummySumSmallerThan("param1", "anotherParam", 56)), "(param1 + anotherParam) < 56");
}


BOOST_AUTO_TEST_CASE(TestGetParameterNames)
{
  std::set<std::string> lExpectedNames = {"param1", "anotherParam"};
  std::set<std::string> lReturnedNames = DummySumSmallerThan("param1", "anotherParam", 42).getParameterNames();
  BOOST_CHECK_EQUAL_COLLECTIONS(lReturnedNames.begin(), lReturnedNames.end(), lExpectedNames.begin(), lExpectedNames.end());

  DummyNullConstraint lNullConstraint;
  lReturnedNames = lNullConstraint.getParameterNames();
  lExpectedNames = {};
  BOOST_CHECK_EQUAL_COLLECTIONS(lReturnedNames.begin(), lReturnedNames.end(), lExpectedNames.begin(), lExpectedNames.end());
  
  lNullConstraint.require<uint32_t>("someNumber");
  lReturnedNames = lNullConstraint.getParameterNames();
  lExpectedNames = {"someNumber"};
  BOOST_CHECK_EQUAL_COLLECTIONS(lReturnedNames.begin(), lReturnedNames.end(), lExpectedNames.begin(), lExpectedNames.end());

  lNullConstraint.require<uint32_t>("anotherNumber");
  lReturnedNames = lNullConstraint.getParameterNames();
  lExpectedNames.insert("anotherNumber");
  BOOST_CHECK_EQUAL_COLLECTIONS(lReturnedNames.begin(), lReturnedNames.end(), lExpectedNames.begin(), lExpectedNames.end());

  lNullConstraint.require<uint32_t>("andAString");
  lReturnedNames = lNullConstraint.getParameterNames();
  lExpectedNames.insert("andAString");
  BOOST_CHECK_EQUAL_COLLECTIONS(lReturnedNames.begin(), lReturnedNames.end(), lExpectedNames.begin(), lExpectedNames.end());  
}


BOOST_AUTO_TEST_CASE(TestFunctionCallOperator)
{
  const DummyNullConstraint lNullConstraint;
  const DummySumSmallerThan lSmallerThan50("paramA", "paramB", 50);
  ParameterSet lParamSet;

  // Check that correct value is returned if given the correct types
  BOOST_CHECK_EQUAL(lNullConstraint(lParamSet), true);

  lParamSet.add("paramA", uint32_t(40));
  lParamSet.add("paramB", uint32_t(4));
  BOOST_CHECK_EQUAL(lSmallerThan50(lParamSet), true);
  lParamSet.erase("paramB");
  lParamSet.add("paramB", uint32_t(14));
  BOOST_CHECK_EQUAL(lSmallerThan50(lParamSet), false);
  
  // Check that exception is thrown if items in input XParameterSet are of incorrect type
  lParamSet.erase("paramB");
  lParamSet.add<std::string>("paramB", "Hello there!");
  BOOST_CHECK_THROW(lSmallerThan50(lParamSet), ConstraintIncompatibleParameter);

  // Check that exception is thrown if some parameters missing
  BOOST_CHECK_THROW(lSmallerThan50(ParameterSet()), ConstraintIncompatibleParameter);
  lParamSet.erase("paramA");
  lParamSet.erase("paramB");
  BOOST_REQUIRE_EQUAL(lParamSet.size(), size_t(0));
  BOOST_CHECK_THROW(lSmallerThan50(lParamSet), ConstraintIncompatibleParameter);
  lParamSet.add("paramA", uint32_t(30));
  BOOST_CHECK_THROW(lSmallerThan50(lParamSet), ConstraintIncompatibleParameter);
  lParamSet.add("ParamB", uint32_t(3));
  BOOST_CHECK_THROW(lSmallerThan50(lParamSet), ConstraintIncompatibleParameter);  
}


BOOST_AUTO_TEST_SUITE_END() // ConstraintTestSuite


} /* namespace test */
} /* namespace core */
} /* namespace swatch */
