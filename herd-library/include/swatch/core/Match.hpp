#ifndef __SWATCH_CORE_MATCH_HPP__
#define __SWATCH_CORE_MATCH_HPP__

#include <string>
#include <ostream>

namespace swatch {
namespace core {

class Match {
public:

  /**
   * @brief      The constructor
   *
   * @param[in]  ok       Rule result (true=pass, false=fail)
   * @param[in]  details  Details, optionals
   */
  Match(bool ok, std::string details="");

  bool ok;

  std::string details;
  
  bool operator!=(const Match& right) const;

  bool operator==(const Match& right) const;

  friend std::ostream& operator<<(std::ostream& os, const Match& obj);

};

} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_MATCH_HPP__ */