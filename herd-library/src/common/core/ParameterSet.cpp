#include "swatch/core/ParameterSet.hpp"


// C++ headers
#include <ostream>                      // for operator<<, basic_ostream
#include <typeinfo>
#include <typeindex>

// SWATCH headers
#include "swatch/core/utilities.hpp"    // for demangleName


namespace swatch {
namespace core {


std::ostream& operator<< ( std::ostream& aStr , const swatch::core::ParameterSet& aParameterSet )
{
  std::set<std::string> lKeys( aParameterSet.keys() );

  aStr << '{';
  //FIXME
  // BOOST_FOREACH( const std::string& name, lKeys ) {
  //   const xdata::Serializable& lData = aParameterSet.get(name);

  //   if (name != *lKeys.begin())
  //     aStr << ',';

  //   aStr << "\"" << name << "\":(<" << demangleName(typeid(lData).name()) << ">, ";

  //   if ( typeid(lData) == typeid(xdata::String) ) {
  //     aStr << "\"" << aParameterSet.get(name).toString() << "\"";
  //   }
  //   else {
  //     aStr << aParameterSet.get(name).toString();
  //   }
  //   aStr << ")";
  // }

  aStr << '}';

  return aStr;
}


template <typename T>
bool ParameterSet::equals(const boost::any& x, const boost::any& y)
{
  return (boost::any_cast<T>(x) == boost::any_cast<T>(y));
}

const ParameterSet::ComparatorMap_t ParameterSet::kComparators({
  {std::type_index(typeid(bool)), equals<bool>},
  {std::type_index(typeid(int32_t)), ParameterSet::equals<int32_t>},
  {std::type_index(typeid(uint32_t)), ParameterSet::equals<uint32_t>},
  {std::type_index(typeid(int64_t)), ParameterSet::equals<int64_t>},
  {std::type_index(typeid(uint64_t)), ParameterSet::equals<uint64_t>},
  {std::type_index(typeid(float)), ParameterSet::equals<float>},
  {std::type_index(typeid(double)), ParameterSet::equals<double>},
  {std::type_index(typeid(std::string)), ParameterSet::equals<std::string>},
  {std::type_index(typeid(std::vector<bool>)), equals<std::vector<bool>>},
  {std::type_index(typeid(std::vector<int32_t>)), ParameterSet::equals<std::vector<int32_t>>},
  {std::type_index(typeid(std::vector<uint32_t>)), ParameterSet::equals<std::vector<uint32_t>>},
  {std::type_index(typeid(std::vector<int64_t>)), ParameterSet::equals<std::vector<int64_t>>},
  {std::type_index(typeid(std::vector<uint64_t>)), ParameterSet::equals<std::vector<uint64_t>>},
  {std::type_index(typeid(std::vector<float>)), ParameterSet::equals<std::vector<float>>},
  {std::type_index(typeid(std::vector<double>)), ParameterSet::equals<std::vector<double>>},
  {std::type_index(typeid(std::vector<std::string>)), ParameterSet::equals<std::vector<std::string>>}
});


ParameterSet::ParameterSet() {}


ParameterSet::ParameterSet(const ParameterSet& aOther) :
  mEntries(aOther.mEntries)
{
}


ParameterSet::~ParameterSet() {}


bool ParameterSet::operator==(const ParameterSet& aOther) const
{
  return mEntries == aOther.mEntries;
}


size_t ParameterSet::size() const
{
  return mEntries.size();
}


std::set<std::string> ParameterSet::keys() const
{
  std::set<std::string> lNames;
  std::transform(mEntries.begin(), mEntries.end(), std::inserter(lNames, lNames.end()), boost::bind(&EntryMap_t::value_type::first, _1));
  return lNames;
}


bool ParameterSet::has(const std::string& aName) const
{
  return mEntries.count(aName);
}


const boost::any& ParameterSet::get( const std::string& aName ) const
{
  EntryMap_t::const_iterator lIt = mEntries.find(aName);
  if ( lIt == mEntries.end() ) {
    throw ParameterNotFound(aName +" not found");
  }

  return *(lIt->second);
}


const boost::any& ParameterSet::operator[](const std::string& aName) const
{
  return get(aName);
}


void ParameterSet::adopt(const std::string& aName , const std::shared_ptr<const boost::any>& aData )
{
  std::pair<EntryMap_t::iterator, bool> lIt = mEntries.emplace(aName, aData);

  // If failed to emplace, then throw
  if ( !lIt.second )
    throw ParameterExists("Parameter '" + aName + "' is already defined");
}


void ParameterSet::adopt(const std::string& aName , const ParameterSet& aOtherSet)
{
  EntryMap_t::const_iterator lIt = aOtherSet.mEntries.find(aName);

  if ( lIt == aOtherSet.mEntries.end() )
    throw ParameterNotFound("Parameter '" + aName + "' does not exist in this set");

  adopt(aName, lIt->second);
}


void ParameterSet::deepCopyFrom(const ParameterSet& aOtherSet)
{
  for ( EntryMap_t::const_iterator lIt = aOtherSet.mEntries.begin(); lIt != aOtherSet.mEntries.end(); lIt++) {
    std::shared_ptr<boost::any> lClonedData( new boost::any(*lIt->second.get()) );

    mEntries.emplace(lIt->first, lClonedData);
  }
}


void ParameterSet::erase(const std::string& aName)
{
  mEntries.erase(aName);
}


bool operator !=(const ParameterSet& aParamSet1, const ParameterSet& aParamSet2)
{
  if (aParamSet1.keys() != aParamSet2.keys())
    return true;

  for (const auto& aKeyVal1 : aParamSet1.mEntries) {
    const boost::any& lValue1 = *aKeyVal1.second;
    const boost::any& lValue2 = *aParamSet2.mEntries.at(aKeyVal1.first);

    if (std::type_index(lValue1.type()) != std::type_index(lValue2.type()))
      return true;

    if ( not ParameterSet::kComparators.at(std::type_index(lValue1.type()))(lValue1, lValue2) )
      return true;
  }

  return false;
}

} // core
} // swatch

