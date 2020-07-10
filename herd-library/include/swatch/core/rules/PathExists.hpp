#ifndef __SWATCH_CORE_RULES_PATHEXISTS_HPP__
#define __SWATCH_CORE_RULES_PATHEXISTS_HPP__


#include "swatch/core/Rule.hpp"


namespace swatch {
namespace core {
namespace rules {

class PathExists : public Rule<std::string> {

public:
  PathExists( const std::string& aPrefix="", const std::string& aExtension="") : mPrefix(aPrefix), mExtension(aExtension) {};
  ~PathExists() {};

  virtual Match verify( const std::string& aValue ) const;

private:

  virtual void describe(std::ostream& aStream) const;

  const std::string mPrefix;
  const std::string mExtension;
};

} // namespace rules	
} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_RULES_PATHEXISTS_HPP__ */