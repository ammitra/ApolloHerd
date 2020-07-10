#include "swatch/action/test/DummyCommand.hpp"

#include <string>

#include "boost/foreach.hpp"
#include "boost/chrono.hpp"

#include "swatch/action/test/DummyActionableObject.hpp"


namespace swatch {
namespace action {
namespace test {

DummyConstraint::DummyConstraint(const std::map<std::string, std::type_index>& aParameters)
{
  for (const auto& lItem : aParameters) {
    if (lItem.second == std::type_index(typeid(int)))
      require<int>(lItem.first);
    else if (lItem.second == std::type_index(typeid(uint32_t)))
      require<uint32_t>(lItem.first);
    else if (lItem.second == std::type_index(typeid(float)))
      require<float>(lItem.first);
    else if (lItem.second == std::type_index(typeid(std::string)))
      require<std::string>(lItem.first);
    else
      assert(false);
  }
}

const std::string DummyConstraint::sDescription = "Dummy constraint for unit tests";
core::Match DummyConstraint::sResult = true;



const boost::any DummyCommand::kDefaultResult(int(-1));

const std::string DummyCommand::kParamX("x");
const std::string DummyCommand::kParamToDo("todo");

const std::string DummyCommand::kFinalMsgUseResource("Dummy command successfully used resource");
const std::string DummyCommand::kFinalMsgSleep("Dummy command finished sleeping");


DummyCommand::DummyCommand(const std::string& aId, ActionableObject& aActionable) :
  Command(aId, aActionable, kDefaultResult)
{
  registerParameter<int>(kParamX, 15, DummyRule<int>());
  registerParameter<std::string>(kParamToDo, "", DummyRule<std::string>());

  addConstraint("constraintA", DummyConstraint({{DummyCommand::kParamX, std::type_index(typeid(int))}}));
  addConstraint("constraintB", DummyConstraint({{DummyCommand::kParamX, std::type_index(typeid(int))}, {DummyCommand::kParamToDo, std::type_index(typeid(std::string))}}));
}


DummyCommand::~DummyCommand()
{
}


void DummyCommand::setExecutionDetails(const core::ParameterSet& aInfoSet)
{
  mNextExecutionDetails = aInfoSet;
}


Command::State DummyCommand::code(const core::ParameterSet& aParams)
{
  // Filling the detailed info XParameterSet: Fill up each entry with a dummy value, then reset to the requested value
  // (set each entry twice so unit tests will check that initial value is used )
  std::set<std::string> lItemNames = mNextExecutionDetails.keys();
  for (auto lIt=lItemNames.begin(); lIt != lItemNames.end(); lIt++) {
    this->addExecutionDetails(*lIt, int(-99));
    const boost::any& lDataItem = mNextExecutionDetails.get(*lIt);
    if (const bool* lBool = boost::any_cast<bool>(&lDataItem))
      addExecutionDetails(*lIt, bool(*lBool));
    else if (const uint32_t* lUnsigned = boost::any_cast<uint32_t>(&lDataItem))
      addExecutionDetails(*lIt, uint32_t(*lUnsigned));
    else if (const int* lInteger = boost::any_cast<int>(&lDataItem))
      addExecutionDetails(*lIt, int(*lInteger));
    else if (const std::string* lString = boost::any_cast<std::string>(&lDataItem))
      addExecutionDetails(*lIt, std::string(*lString));
    else
      assert(false);
  }

  // Now actually use the resource ...
  DummyActionableObject& res = getActionable<DummyActionableObject>();

  const std::string& todo = aParams.get<std::string>("todo");
  setResult(aParams.get("x"));

  if (todo == "useResource") {

    res.setNumber(54);
    setProgress(0.1);

    setProgress(0.99, kFinalMsgUseResource);
    return State::kDone;
  }
  if (todo == "sleep") {
    setProgress(0.01, "Dummy command just started");
    unsigned int milliseconds(aParams.get<int>("milliseconds"));

    for (unsigned int i = 0; i < milliseconds; ++i) {
      boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
      setProgress(0.01 + 0.99 * float(i) / milliseconds, "Dummy command progressed");
    }

    setStatusMsg(kFinalMsgSleep);
    return State::kDone;
  }
  else {
    return State::kDone;
  }
}


//-----------------------//
/*  DummyWarningCommand  */

const boost::any DummyWarningCommand::kDefaultResult(int(-1));
const std::string DummyWarningCommand::kFinalMsg("Dummy command did something, but ended up in warning");

DummyWarningCommand::DummyWarningCommand(const std::string& aId, ActionableObject& aActionable) :
  Command(aId, aActionable, kDefaultResult)
{
}


DummyWarningCommand::~DummyWarningCommand()
{
}


void DummyWarningCommand::setExecutionDetails(const core::ParameterSet& aInfoSet)
{
  mNextExecutionDetails = aInfoSet;
}


Command::State DummyWarningCommand::code(const core::ParameterSet& aParams)
{
  // Filling the detailed info XParameterSet: Fill up each entry with a dummy value, then reset to the requested value
  // (set each entry twice so unit tests will check that initial value is used )
  std::set<std::string> lItemNames = mNextExecutionDetails.keys();
  for (auto lIt=lItemNames.begin(); lIt != lItemNames.end(); lIt++) {
    this->addExecutionDetails(*lIt, int(-99));
    const boost::any& lDataItem = mNextExecutionDetails.get(*lIt);
    if (const bool* lBool = boost::any_cast<bool>(&lDataItem))
      addExecutionDetails(*lIt, bool(*lBool));
    else if (const uint32_t* lUnsigned = boost::any_cast<uint32_t>(&lDataItem))
      addExecutionDetails(*lIt, uint32_t(*lUnsigned));
    else if (const int* lInteger = boost::any_cast<int>(&lDataItem))
      addExecutionDetails(*lIt, int(*lInteger));
    else if (const std::string* lString = boost::any_cast<std::string>(&lDataItem))
      addExecutionDetails(*lIt, std::string(*lString));
    else
      assert(false);
  }

  // Now actually use the resource ...
  setProgress(0.5049, kFinalMsg);
  return State::kWarning;
}



//---------------------//
/*  DummyErrorCommand  */

const boost::any DummyErrorCommand::kDefaultResult(int(-1));
const std::string DummyErrorCommand::kFinalMsg("Dummy command did something, but ended up in error");
const float DummyErrorCommand::kFinalProgress(0.5049);

DummyErrorCommand::DummyErrorCommand(const std::string& aId, ActionableObject& aActionable) :
  Command(aId, aActionable, kDefaultResult)
{
}

DummyErrorCommand::~DummyErrorCommand()
{
}

void DummyErrorCommand::setExecutionDetails(const core::ParameterSet& aInfoSet)
{
  mNextExecutionDetails = aInfoSet;
}

Command::State DummyErrorCommand::code(const core::ParameterSet& aParams)
{
  // Filling the detailed info XParameterSet: Fill up each entry with a dummy value, then reset to the requested value
  // (set each entry twice so unit tests will check that initial value is used )
  std::set<std::string> lItemNames = mNextExecutionDetails.keys();
  for (auto lIt=lItemNames.begin(); lIt != lItemNames.end(); lIt++) {
    this->addExecutionDetails(*lIt, int(-99));
    const boost::any& lDataItem = mNextExecutionDetails.get(*lIt);
    if (const bool* lBool = boost::any_cast<bool>(&lDataItem))
      addExecutionDetails(*lIt, bool(*lBool));
    else if (const uint32_t* lUnsigned = boost::any_cast<uint32_t>(&lDataItem))
      addExecutionDetails(*lIt, uint32_t(*lUnsigned));
    else if (const int* lInteger = boost::any_cast<int>(&lDataItem))
      addExecutionDetails(*lIt, int(*lInteger));
    else if (const std::string* lString = boost::any_cast<std::string>(&lDataItem))
      addExecutionDetails(*lIt, std::string(*lString));
    else
      assert(false);
  }

  // Now actually use the resource ...
  setProgress(kFinalProgress, kFinalMsg);
  return State::kError;
}


//---------------------//
/*  DummyThrowCommand  */

const boost::any DummyThrowCommand::kDefaultResult(int(-1));
const std::string DummyThrowCommand::kExceptionMsg("My test exception message");
const float DummyThrowCommand::kFinalProgress(0.4032);

DummyThrowCommand::DummyThrowCommand(const std::string& aId, ActionableObject& aActionable) :
  Command(aId, aActionable, kDefaultResult)
{
}

DummyThrowCommand::~DummyThrowCommand()
{
}

void DummyThrowCommand::setExecutionDetails(const core::ParameterSet& aInfoSet)
{
  mNextExecutionDetails = aInfoSet;
}

Command::State DummyThrowCommand::code(const core::ParameterSet& aParams)
{
  // Filling the detailed info XParameterSet: Fill up each entry with a dummy value, then reset to the requested value
  // (set each entry twice so unit tests will check that initial value is used )
  std::set<std::string> lItemNames = mNextExecutionDetails.keys();
  for (auto lIt=lItemNames.begin(); lIt != lItemNames.end(); lIt++) {
    this->addExecutionDetails(*lIt, int(-99));
    const boost::any& lDataItem = mNextExecutionDetails.get(*lIt);
    if (const bool* lBool = boost::any_cast<bool>(&lDataItem))
      addExecutionDetails(*lIt, bool(*lBool));
    else if (const uint32_t* lUnsigned = boost::any_cast<uint32_t>(&lDataItem))
      addExecutionDetails(*lIt, uint32_t(*lUnsigned));
    else if (const int* lInteger = boost::any_cast<int>(&lDataItem))
      addExecutionDetails(*lIt, int(*lInteger));
    else if (const std::string* lString = boost::any_cast<std::string>(&lDataItem))
      addExecutionDetails(*lIt, std::string(*lString));
    else
      assert(false);
  }

  // Now actually use the resource ...
  setProgress(kFinalProgress);
  throw core::RuntimeError(kExceptionMsg);
}


} /* namespace test */
} /* namespace action */
} /* namespace swatch */
