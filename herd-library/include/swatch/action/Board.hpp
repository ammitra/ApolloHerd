
#ifndef __SWATCH_ACTION_BOARD_HPP__
#define __SWATCH_ACTION_BOARD_HPP__


#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>


namespace swatch {
namespace action {

class Device;
class DeviceStub;


class Board {
public:

  Board(const std::vector<DeviceStub>&);

  Board(const Board&) = delete;
  Board& operator=(const Board&) = delete;

  ~Board();

  std::set<std::string> getDevices() const;

  Device& getDevice(const std::string& aId);

  const Device& getDevice(const std::string& aId) const;

private:
  std::unordered_map<std::string, std::unique_ptr<Device>> mDevices;
};


}
}

#endif  /* __SWATCH_ACTION_BOARD_HPP__ */

