
#include "swatch/action/Board.hpp"


#include "swatch/core/AbstractStub.hpp"
#include "swatch/core/Factory.hpp"
#include "swatch/action/Device.hpp"
#include "swatch/action/DeviceStub.hpp"


namespace swatch {
namespace action {

Board::Board(const std::vector<DeviceStub>& aStubs)
{
  for (const auto& lStub : aStubs) {
    Device* lDevice = core::Factory::get()->make<Device>(lStub.creator, lStub);
    mDevices.emplace(lDevice->getId(), std::unique_ptr<Device>(lDevice));
  }
}


Board::~Board()
{
}


std::set<std::string> Board::getDevices() const
{
  std::set<std::string> lResult;
  for (const auto& x : mDevices)
    lResult.insert(x.second->getId());
  return lResult;
}


Device& Board::getDevice(const std::string& aId)
{
  auto it = mDevices.find(aId);
  if (it == mDevices.end())
    throw std::runtime_error("No device with ID '" + aId + "'");
  else
    return *(it->second);
}


const Device& Board::getDevice(const std::string& aId) const
{
  auto it = mDevices.find(aId);
  if (it == mDevices.end())
    throw std::runtime_error("No device with ID '" + aId + "'");
  else
    return *(it->second);
}


} // namespace action
} // namespace swatch

