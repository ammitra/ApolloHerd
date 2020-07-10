/**
 * @file    GateKeeper.hpp
 * @author  arose
 * @brief   Guardian to the underworld that is the online database
 * @date    May 2015
 *
 */

#ifndef __SWATCH_ACTION_GATEKEEPER_HPP__
#define __SWATCH_ACTION_GATEKEEPER_HPP__

// C++ Headers
#include <string>
#include <iosfwd>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

// SWATCH headers
#include "swatch/core/exception.hpp"

// boost headers
#include "boost/date_time/posix_time/ptime.hpp"  // for ptime


namespace boost {
class any;
}

namespace swatch {
namespace action {

class GateKeeper;
std::ostream& operator<<(std::ostream& aStr, const swatch::action::GateKeeper& aGateKeeper);

class GateKeeper {
  friend std::ostream& operator<<(std::ostream& aStr, const swatch::action::GateKeeper& aGateKeeper);

public:
  typedef std::shared_ptr<const boost::any> Parameter_t;
  // each parameter is mapped against a path, usually
  // 'aSequenceId.aCommandId.aParameterId'
  typedef std::unordered_map<std::string, Parameter_t> Parameters_t;
  typedef std::shared_ptr<const Parameters_t> ParametersContext_t;
  // these pairs are then stored in different contexts, e.g. 'system.processors'
  typedef std::unordered_map<std::string, ParametersContext_t> ParametersContextCache_t;

  typedef std::unordered_set<std::string> Masks_t;
  typedef std::shared_ptr<const Masks_t> MasksContext_t;
  typedef std::unordered_map<std::string, MasksContext_t> MasksContextCache_t;

  typedef std::unordered_set<std::string> DisabledSet_t;

  /**
   * Constructor
   * @param aKey A global run-identifier
   */
  GateKeeper(const std::string& aKey);

  /// Destructor
  virtual ~GateKeeper();

  /**
   * Method to retreive configuration data from a specified path
   * @param aParameterId A key used to identify the configuration data being requested of the gatekeeper
   * @param aContexts A list of context identifiers (which may or may not exist) to look in for the requested parameters
   * @return the requested data, or throw if the key is not found in any context
   */
  Parameter_t get(const std::string& aSequenceId, const std::string& aCommandId, const std::string& aParameterId,
                  const std::vector<std::string>& aContextsToLookIn) const;
  bool getMask(const std::string& aObjId, const std::vector<std::string>& aContextsToLookIn) const;

  bool isEnabled(const std::string& aObjId) const;

  const boost::posix_time::ptime& lastUpdated();

  ParametersContextCache_t::const_iterator parametersBegin() const;
  ParametersContextCache_t::const_iterator parametersEnd() const;

  MasksContextCache_t::const_iterator masksBegin() const;
  MasksContextCache_t::const_iterator masksEnd() const;

  DisabledSet_t::const_iterator disabledFlagsBegin() const;
  DisabledSet_t::const_iterator disabledFlagsEnd() const;

protected:
  /**
   * Add a named context to the map of cached contexts
   * @param aId the name of the context
   * @param aContext a new xdata context, of which the Gatekeeper will take ownership
   */
  void add(const std::string& aId, ParametersContext_t aContext);
  void add(const std::string& aId, MasksContext_t aContext);

  //! Add specified ID string to the set of disabled object IDs
  void addToDisabledSet(const std::string& aId);

private:

  /**
   * Method to retreive configuration data from a specified path
   * @param aParam A key used to identify the configuration data being requested of the gatekeeper
   * @param aContext A context identifier (which may or may not exist) to look in for the requested parameters
   * @return the requested data, or throw if the key is not found in any context
   */
  Parameter_t get(const std::string& aParam, const std::string& aContext) const;

  Parameter_t get(const std::string& aNamespace, const std::string& aCommandPath, const std::string& aParameterId,
                  const std::string& aContext) const;

  bool getMask(const std::string& aObjId, const std::string& aContextsToLookIn) const;

  /// The global run-identifier
  std::string mKey;

  /// The cache of contexts
  ParametersContextCache_t mParameters;
  MasksContextCache_t mMasks;
  DisabledSet_t mDisabledObjs;

  /// The last time a context was modified
  boost::posix_time::ptime mUpdateTime;

};


SWATCH_DEFINE_EXCEPTION ( ContextWithIdAlreadyExists )
SWATCH_DEFINE_EXCEPTION ( ParameterWithGivenIdAlreadyExistsInContext )
SWATCH_DEFINE_EXCEPTION ( NonExistentKey )


} // namespace action
} // namespace swatch

#endif /* __SWATCH_ACTION_GATEKEEPER_HPP__ */
