/**
 * @file    ParameterSet.hpp
 * @author  Alessandro Thea, Tom Williams
 * @date    July 2019
 */

#ifndef __SWATCH_CORE_PARAMETERSET_HPP__
#define __SWATCH_CORE_PARAMETERSET_HPP__


// Standard headers
#include <stddef.h>                     // for size_t
#include <iosfwd>                       // for ostream
#include <set>                          // for set
#include <string>                       // for string
#include <unordered_map>


// SWATCH headers
#include "swatch/core/exception.hpp"


// Forward declarations
namespace boost {
class any;
}

namespace swatch {
namespace core {


class ParameterSet;

std::ostream& operator<< ( std::ostream& aStr , const swatch::core::ParameterSet& aParameterSet );

bool operator !=(const ParameterSet&, const ParameterSet&);

//! Generic container (read-only) interface for containers of parameters
class ParameterSet {
public:

  friend std::ostream& (operator<<) ( std::ostream& aStr , const swatch::core::ParameterSet& aParameterSet );

  ParameterSet();

  ParameterSet(const ParameterSet& aOther);

  virtual ~ParameterSet();

  bool operator==(const ParameterSet& aOther) const;

  /**
   * @brief      Size of the collection
   *
   * @return     Number of stored parameters
   */
  size_t size() const;

  /**
   * @brief      List of names of stored parameters.
   *
   * @return     names of stored parameters
   */
  std::set<std::string> keys() const;

  /**
   * @brief      Returns whether parameter with given name exists
   *
   * @param      aName  Name of the entry to test
   *
   * @return     True if parameter exists
   */
  bool has( const std::string& aName ) const;

  /**
   * @brief      Retrieve reference to parameter with specified name; throws
   *             ParameterNotFound if doesn't contain a parameter with
   *             specified name
   *
   * @param      aName  Name of the entry to retrieve
   *
   * @return     The requested entry
   */
  const boost::any& get( const std::string& aName ) const;

  /**
   * @brief      Retrieve reference to parameter with specified name; throws
   *             ParameterNotFound if doesn't contain a parameter with
   *             specified name
   *
   * @param      aName  Name of the entry to retrieve
   *
   * @return     The requested entry
   */
  const boost::any& operator[]( const std::string& aName ) const;

  /**
   * @brief      Retrieve reference to parameter with specified name; throws
   *             ParameterFailedCast if dynamic cast fails; throws
   *             ParameterNotFound if doesn't contain a parameter with
   *             specified name
   *
   * @param[in]  aName  Name of the parameter.
   *
   * @tparam     T      Type of the parameter.
   *
   * @return     The requested parameter
   */
  template<typename T>
  const T& get( const std::string& aName ) const;

  /**
   * Adopt a parameter in the set; the data that is pointed to is not copied, and stored internally via a shared_ptr
   *
   * @param aName Parameter name
   * @param aData Pointer to import in the set
   */
  void adopt( const std::string& aName , const std::shared_ptr<const boost::any>& aData );

  /**
   * Adopt a parameter that's already stored in another set; the data that is pointed to is not copied - instead its shared between the parameter sets.
   *
   * @param aName Parameter name
   */
  void adopt( const std::string& aName , const ParameterSet& aOtherSet);

  /**
   * @brief      Add a parameter to the set, by copying data into the set
   *
   * @param      aName  Parameter name
   * @param      aData  Value to copy in the set.
   *
   * @tparam     T      Parameter type.
   */
  template <typename T>
  void add( const std::string& aName , const T& aData );

  /**
   * @brief      Deep copy the content of another set in this one.
   *
   * @param[in]  aOtherSet  The set to copy the content from.
   */
  void deepCopyFrom(const ParameterSet& aOtherSet);

  /**
   * @brief      Removes parameter of specified name from the list of variables.
   *
   * @param      aName  ParameterSet entry to delete
   */
  void erase( const std::string& aName );

private:
  typedef std::unordered_map<std::string, std::shared_ptr<const boost::any> > EntryMap_t;

  typedef std::unordered_map<std::type_index, std::function<bool (const boost::any&, const boost::any&)> > ComparatorMap_t;

  EntryMap_t mEntries;

  static const ComparatorMap_t kComparators;

  template <typename T>
  static bool equals(const boost::any&, const boost::any&);

  friend bool operator !=(const ParameterSet&, const ParameterSet&);
};


// Exceptions
SWATCH_DEFINE_EXCEPTION(ParameterNotFound)
SWATCH_DEFINE_EXCEPTION(ParameterExists)
SWATCH_DEFINE_EXCEPTION(ParameterFailedCast)
SWATCH_DEFINE_EXCEPTION(ParameterUnknownType)

} // namespace core
} // namespace swatch


#include "swatch/core/ParameterSet.hxx"


#endif /* __SWATCH_CORE_PARAMETERSET_HPP__ */
