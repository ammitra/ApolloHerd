
#ifndef __SWATCH_UTILITIES_HPP__
#define __SWATCH_UTILITIES_HPP__


#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>


namespace swatch {

namespace action {
class Board;
class DeviceStub;
}

struct AppConfig {
public:
  std::vector<std::string> libraries;
  std::vector<action::DeviceStub> devices;
};


AppConfig parseConfigFile(const std::string& aPath);

void printCommandSummary(const action::Board& aBoard);

void printCommandAndFSMSummary(const action::Board& aBoard);

void loadLibraries(const std::vector<std::string>& aPaths, const boost::filesystem::path& aBase);

}

#endif /* __SWATCH_UTILITIES_HPP__ */
