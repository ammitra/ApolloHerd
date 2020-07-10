#include "swatch/action/GateKeeper.hpp"


#include <iterator>                     // for insert_iterator, inserter
#include <ostream>                      // for basic_ostream, operator<<, etc
#include <set>                          // for set, etc
#include <utility>                      // for make_pair

// boost headers
#include <boost/any.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/foreach.hpp>

#include "swatch/core/utilities.hpp"


namespace swatch {
namespace action {

GateKeeper::GateKeeper(const std::string& aKey) :
  mKey(aKey),
  mParameters(),
  mUpdateTime()
{
}

GateKeeper::~GateKeeper()
{
}


GateKeeper::Parameter_t GateKeeper::get(const std::string& aParam,
                                        const std::string& aContext) const
{
  ParametersContextCache_t::const_iterator lContext(mParameters.find(aContext));

  if (lContext == mParameters.end()) {
    return Parameter_t(); //perfectly acceptable for context name to not exist, just try the context with the next highest priority
  }

  Parameters_t::const_iterator lData(lContext->second->find(aParam));

  if (lData == lContext->second->end()) {
    return Parameter_t(); //perfectly acceptable for context name to not exist, just try the context with the next highest priority
  }

  return lData->second;  //We found data!
}


GateKeeper::Parameter_t GateKeeper::get(const std::string& aNamespace,
                                        const std::string& aCommandPath, const std::string& aParameterId,
                                        const std::string& aContext) const
{
  Parameter_t lData;
  lData = get(aNamespace, aContext);
  if (lData) {
    return lData; //perfectly acceptable for specific context not hold the requested data, just try the context with the next highest priority
  }

  lData = get(aCommandPath, aContext);
  if (lData) {
    return lData; //perfectly acceptable for specific context not hold the requested data, just try the context with the next highest priority
  }

  lData = get(aParameterId, aContext);
  if (lData) {
    return lData; //perfectly acceptable for specific context not hold the requested data, just try the context with the next highest priority
  }

  return Parameter_t();
}

GateKeeper::Parameter_t GateKeeper::get(const std::string& aSequenceId,
                                        const std::string& aCommandId, const std::string& aParameterId,
                                        const std::vector<std::string>& aContextsToLookIn) const
{

  std::string lCommandPath(aCommandId + "." + aParameterId);
  std::string lSequencePath(aSequenceId + "." + lCommandPath);

  Parameter_t lData;

  for (std::vector<std::string>::const_iterator lIt(aContextsToLookIn.begin());
       lIt != aContextsToLookIn.end(); ++lIt) {
    lData = get(lSequencePath, lCommandPath, aParameterId, *lIt);
    if (lData) {
      return lData; //perfectly acceptable for specific context not hold the requested data, just try the context with the next highest priority
    }
  }

  return Parameter_t();
}


bool GateKeeper::getMask(const std::string& aObjId, const std::vector<std::string>& aContextsToLookIn) const
{
  bool lMask = false;

  for (std::vector<std::string>::const_iterator lIt(aContextsToLookIn.begin()); lIt != aContextsToLookIn.end(); ++lIt) {
    lMask = getMask(aObjId, *lIt);
    if (lMask)
      return lMask;
  }

  return false;
}


bool GateKeeper::getMask(const std::string& aObjId, const std::string& aContextToLookIn) const
{
  MasksContextCache_t::const_iterator lContext(mMasks.find(aContextToLookIn));

  if (lContext == mMasks.end()) {
    //perfectly acceptable for context name to not exist, just try the context with the next highest priority
    return false;
  }

  Masks_t::const_iterator lMaskIt = lContext->second->find(aObjId);

  if (lMaskIt == lContext->second->end()) {
    //perfectly acceptable for context name to not exist, just try the context with the next highest priority
    return false;
  }

  return true;  //We found data!
}


bool GateKeeper::isEnabled(const std::string& aObjId) const
{
  return (mDisabledObjs.find(aObjId) == mDisabledObjs.end());
}


void GateKeeper::add(const std::string& aId, ParametersContext_t aContext)
{
  ParametersContextCache_t::iterator lContextIt(mParameters.find(aId));

  if (lContextIt != mParameters.end()) {
    throw ContextWithIdAlreadyExists("Context With Id '" + aId + "' already exists");
  }

  mParameters.insert(std::make_pair(aId, aContext));
  mUpdateTime = boost::posix_time::microsec_clock::universal_time();
}


void GateKeeper::add(const std::string& aId, MasksContext_t aContext)
{
  MasksContextCache_t::const_iterator lContextIt(mMasks.find(aId));
  if (lContextIt != mMasks.end())
    throw ContextWithIdAlreadyExists("Context of masks with Id '" + aId + "' already exists");

  mMasks.insert(std::make_pair(aId, aContext));
}


void GateKeeper::addToDisabledSet ( const std::string& aId )
{
  DisabledSet_t::const_iterator lIt(mDisabledObjs.find(aId));
  if (lIt != mDisabledObjs.end())
    throw ContextWithIdAlreadyExists("ID path '"+ aId + "' is already present in set of disabled IDs");

  mDisabledObjs.insert(aId);
}


const boost::posix_time::ptime& GateKeeper::lastUpdated()
{
  return mUpdateTime;
}

std::ostream& operator<<(std::ostream& aStr,
                         const swatch::action::GateKeeper& aGateKeeper)
{
  std::string lDelimeter(100, '-');

  aStr << lDelimeter << std::endl;
  for (auto lIt = aGateKeeper.mDisabledObjs.begin(); lIt != aGateKeeper.mDisabledObjs.end(); lIt++) {
    aStr << "DISABLE : " << *lIt << std::endl;
  }

  for (GateKeeper::ParametersContextCache_t::const_iterator lContextIt(
         aGateKeeper.mParameters.begin()); lContextIt != aGateKeeper.mParameters.end();
       ++lContextIt) {
    aStr << lDelimeter << std::endl;
    aStr << "CONTEXT (parameters) : " << lContextIt->first << std::endl;
    aStr << lDelimeter << std::endl;

    std::set<std::string> lNames;
    boost::copy(*(lContextIt->second) | boost::adaptors::map_keys,
                std::inserter(lNames, lNames.begin()));

    for( const std::string& name: lNames ) {
      aStr << " " << name << " : " << core::demangleName(typeid(lContextIt->second->at(name)).name())
           << std::endl;
    }

    aStr << lDelimeter << std::endl;
  }

  for (auto lContextIt = aGateKeeper.mMasks.begin(); lContextIt != aGateKeeper.mMasks.end(); lContextIt++) {
    aStr << lDelimeter << std::endl;
    aStr << "CONTEXT (masks) : " << lContextIt->first << std::endl;
    aStr << lDelimeter << std::endl;

    std::set<std::string> lNames;
    boost::copy(*(lContextIt->second), std::inserter(lNames, lNames.begin()));

    for( const std::string& lName: lNames)
      aStr << " " << lName << std::endl;
    aStr << lDelimeter << std::endl;
  }

  return aStr;
}

GateKeeper::ParametersContextCache_t::const_iterator GateKeeper::parametersBegin() const
{
  return mParameters.cbegin();
}
GateKeeper::ParametersContextCache_t::const_iterator GateKeeper::parametersEnd() const
{
  return mParameters.cend();
}

GateKeeper::MasksContextCache_t::const_iterator GateKeeper::masksBegin() const
{
  return mMasks.cbegin();
}
GateKeeper::MasksContextCache_t::const_iterator GateKeeper::masksEnd() const
{
  return mMasks.cend();
}

GateKeeper::DisabledSet_t::const_iterator GateKeeper::disabledFlagsBegin() const
{
  return mDisabledObjs.cbegin();
}
GateKeeper::DisabledSet_t::const_iterator GateKeeper::disabledFlagsEnd() const
{
  return mDisabledObjs.cend();
}

} /* namespace action */
} /* namespace swatch */
