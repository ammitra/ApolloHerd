// Boost Unit Test includes
#include <boost/test/unit_test.hpp>

// Class under test ...
#include "swatch/core/ParameterSet.hpp"

// boost headers
#include "boost/lexical_cast.hpp"
#include <boost/test/test_tools.hpp>


namespace swatch {
namespace core {
namespace test {


BOOST_AUTO_TEST_SUITE( ParSetsTestSuite )

BOOST_AUTO_TEST_CASE ( EmptySetTest )
{
  ParameterSet lPSet;

  BOOST_CHECK_EQUAL(lPSet.size(), size_t(0));
  BOOST_CHECK_EQUAL(lPSet.keys().size(), size_t(0));
  BOOST_CHECK_THROW(lPSet["aKey"], ParameterNotFound);
  BOOST_CHECK_THROW(lPSet.get("aKey"), ParameterNotFound);
  BOOST_CHECK_THROW(lPSet.get<std::string>("aKey"), ParameterNotFound);
  BOOST_CHECK_EQUAL(lPSet.has("aKey"), false);
}


struct ParTestSetup {
  ParTestSetup();
  ~ParTestSetup();

  std::shared_ptr<boost::any> str;
  std::shared_ptr<boost::any> intA;
  std::shared_ptr<boost::any> intB;
};

ParTestSetup::ParTestSetup() :
  str(new boost::any(std::string("aStringValue"))),
  intA(new boost::any(int(42))),
  intB(new boost::any(int(-1)))
{
}

ParTestSetup::~ParTestSetup() {}


BOOST_FIXTURE_TEST_CASE ( SimpleAdoptGetEraseTest, ParTestSetup)
{
  /*  ---  adopt method  ---  */
  {
    ParameterSet pset;
    pset.adopt("aString", str);
    pset.adopt("intA", intA);
    pset.adopt("intB", intB);

    BOOST_CHECK_EQUAL(pset.size(), size_t(3));
    std::set<std::string> expectedKeys = {"aString", "intA", "intB"};
    std::set<std::string> returnedKeys = pset.keys();
    BOOST_CHECK_EQUAL_COLLECTIONS(returnedKeys.begin(), returnedKeys.end(), expectedKeys.begin(), expectedKeys.end() );

    BOOST_CHECK_EQUAL(pset.has("aString"), true);
    BOOST_CHECK_EQUAL(pset.has("intA"), true);
    BOOST_CHECK_EQUAL(pset.has("intB"), true);
    BOOST_CHECK_EQUAL(pset.has("someOtherItem"), false);

    // Check that parameter values are correct
    BOOST_CHECK_EQUAL( pset.get<std::string>("aString"), boost::any_cast<std::string>(*str));
    BOOST_CHECK_EQUAL( pset.get<int>("intA"), boost::any_cast<int>(*intA));
    BOOST_CHECK_EQUAL( pset.get<int>("intB"), boost::any_cast<int>(*intB));

    // Check that parameter values weren't copied
    BOOST_CHECK_EQUAL( & pset.get("aString") , str.get() );
    BOOST_CHECK_EQUAL( & pset.get("intA") , intA.get() );
    BOOST_CHECK_EQUAL( & pset.get("intB") , intB.get() );
    BOOST_CHECK_EQUAL( & pset["aString"] , str.get() );
    BOOST_CHECK_EQUAL( & pset["intA"] , intA.get() );
    BOOST_CHECK_EQUAL( & pset["intB"] , intB.get() );

    // Check that exception thrown when cast to incorrect type
    BOOST_CHECK_THROW( pset.get<bool>("aString") , ParameterFailedCast);
    BOOST_CHECK_THROW( pset.get<bool>("intA") , ParameterFailedCast);
    BOOST_CHECK_THROW( pset.get<std::string>("intB") , ParameterFailedCast);
  }


  /*  ---  add method ---  */
  {
    ParameterSet pset;
    pset.add("aString", boost::any_cast<std::string>(*str));
    pset.add("intA", boost::any_cast<int>(*intA));
    pset.add("intB", boost::any_cast<int>(*intB));

    BOOST_CHECK_EQUAL(pset.size(), size_t(3));
    std::set<std::string> expectedKeys = {"aString", "intA", "intB"};
    std::set<std::string> returnedKeys = pset.keys();
    BOOST_CHECK_EQUAL_COLLECTIONS(returnedKeys.begin(), returnedKeys.end(), expectedKeys.begin(), expectedKeys.end() );

    BOOST_CHECK_EQUAL(pset.has("aString"), true);
    BOOST_CHECK_EQUAL(pset.has("intA"), true);
    BOOST_CHECK_EQUAL(pset.has("intB"), true);
    BOOST_CHECK_EQUAL(pset.has("someOtherItem"), false);
    
    // Check that cannot overwrite existing entries using 'add' method
    BOOST_CHECK_THROW(pset.add("aString", std::string("A string that should not enter the set")), ParameterExists);
    BOOST_CHECK_THROW(pset.add("intA", int(42)), ParameterExists);
    BOOST_CHECK_THROW(pset.add("intB", int(99)), ParameterExists);

    // Check that parameter values are correct
    BOOST_CHECK_EQUAL( pset.get<std::string>("aString"), boost::any_cast<std::string>(*str));
    BOOST_CHECK_EQUAL( pset.get<int>("intA"), boost::any_cast<int>(*intA));
    BOOST_CHECK_EQUAL( pset.get<int>("intB"), boost::any_cast<int>(*intB));

    // Check that parameter values WERE copied
    BOOST_CHECK_NE ( & pset.get("aString") , str.get() );
    BOOST_CHECK_NE ( & pset.get("intA") , intA.get() );
    BOOST_CHECK_NE ( & pset.get("intB") , intB.get() );
    BOOST_CHECK_NE ( & pset["aString"] , str.get() );
    BOOST_CHECK_NE ( & pset["intA"] , intA.get() );
    BOOST_CHECK_NE ( & pset["intB"] , intB.get() );

    // Check that exception thrown when cast to incorrect type
    BOOST_CHECK_THROW( pset.get<bool>("aString") , ParameterFailedCast);
    BOOST_CHECK_THROW( pset.get<bool>("intA") , ParameterFailedCast);
    BOOST_CHECK_THROW( pset.get<std::string>("intB") , ParameterFailedCast);
  }
}



BOOST_FIXTURE_TEST_CASE ( ShallowCopyTest, ParTestSetup)
{
  // Create the "source" set
  std::unique_ptr<ParameterSet> originalPSet( new ParameterSet() );
  originalPSet->adopt("aString", str);
  originalPSet->adopt("intA", intA);
  originalPSet->adopt("intB", intB);

  // Copy the "source" set - creating instance that will be tested
  ParameterSet pset(*originalPSet);

  // Delete the original set before testing ...
  originalPSet.reset(NULL);

  // Check the basics
  BOOST_CHECK_EQUAL(pset.size(), size_t(3));
  std::set<std::string> expectedKeys = {"aString", "intA", "intB"};
  std::set<std::string> returnedKeys = pset.keys();
  BOOST_CHECK_EQUAL_COLLECTIONS(returnedKeys.begin(), returnedKeys.end(), expectedKeys.begin(), expectedKeys.end() );

  BOOST_CHECK_EQUAL(pset.has("aString"), true);
  BOOST_CHECK_EQUAL(pset.has("intA"), true);
  BOOST_CHECK_EQUAL(pset.has("intB"), true);
  BOOST_CHECK_EQUAL(pset.has("someOtherItem"), false);

  // Check that parameter values are correct
  BOOST_CHECK_EQUAL( pset.get<std::string>("aString"), boost::any_cast<std::string>(*str));
  BOOST_CHECK_EQUAL( pset.get<int>("intA"), boost::any_cast<int>(*intA));
  BOOST_CHECK_EQUAL( pset.get<int>("intB"), boost::any_cast<int>(*intB));

  // Check that parameter values weren't copied
  BOOST_CHECK_EQUAL( & pset.get("aString") , str.get() );
  BOOST_CHECK_EQUAL( & pset.get("intA") , intA.get() );
  BOOST_CHECK_EQUAL( & pset.get("intB") , intB.get() );
  BOOST_CHECK_EQUAL( & pset["aString"] , str.get() );
  BOOST_CHECK_EQUAL( & pset["intA"] , intA.get() );
  BOOST_CHECK_EQUAL( & pset["intB"] , intB.get() );

  // Check that exception thrown when cast to incorrect type
  BOOST_CHECK_THROW( pset.get<bool>("aString") , ParameterFailedCast);
  BOOST_CHECK_THROW( pset.get<bool>("intA") , ParameterFailedCast);
  BOOST_CHECK_THROW( pset.get<std::string>("intB") , ParameterFailedCast);
}


BOOST_FIXTURE_TEST_CASE ( DeepCopyTest, ParTestSetup)
{
  // Create the "source" set
  std::unique_ptr<ParameterSet> originalPSet( new ParameterSet() );
  originalPSet->adopt("aString", str);
  originalPSet->adopt("intA", intA);
  originalPSet->adopt("intB", intB);

  // Copy the "source" set - creating instance that will be tested
  ParameterSet pset;
  pset.deepCopyFrom(*originalPSet);

  // Delete the original set before testing ...
  originalPSet.reset(NULL);

  // Check the basics
  BOOST_CHECK_EQUAL(pset.size(), size_t(3));
  std::set<std::string> expectedKeys = {"aString", "intA", "intB"};
  std::set<std::string> returnedKeys = pset.keys();
  BOOST_CHECK_EQUAL_COLLECTIONS(returnedKeys.begin(), returnedKeys.end(), expectedKeys.begin(), expectedKeys.end() );

  BOOST_CHECK_EQUAL(pset.has("aString"), true);
  BOOST_CHECK_EQUAL(pset.has("intA"), true);
  BOOST_CHECK_EQUAL(pset.has("intB"), true);
  BOOST_CHECK_EQUAL(pset.has("someOtherItem"), false);

  // Check that parameter values are correct
  BOOST_CHECK_EQUAL( pset.get<std::string>("aString"), boost::any_cast<std::string>(*str));
  BOOST_CHECK_EQUAL( pset.get<int>("intA"), boost::any_cast<int>(*intA));
  BOOST_CHECK_EQUAL( pset.get<int>("intB"), boost::any_cast<int>(*intB));

  // Check that parameter values WERE copied
  BOOST_CHECK_NE( & pset.get("aString") , str.get() );
  BOOST_CHECK_NE( & pset.get("intA") , intA.get() );
  BOOST_CHECK_NE( & pset.get("intB") , intB.get() );
  BOOST_CHECK_NE( & pset["aString"] , str.get() );
  BOOST_CHECK_NE( & pset["intA"] , intA.get() );
  BOOST_CHECK_NE( & pset["intB"] , intB.get() );

  // Check that exception thrown when cast to incorrect type
  BOOST_CHECK_THROW( pset.get<bool>("aString") , ParameterFailedCast);
  BOOST_CHECK_THROW( pset.get<bool>("intA") , ParameterFailedCast);
  BOOST_CHECK_THROW( pset.get<std::string>("intB") , ParameterFailedCast);
}



BOOST_AUTO_TEST_SUITE_END() // ParsTestSuite


} //ns: test
} //ns: core
} //ns: swatch
