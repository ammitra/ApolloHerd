
#include "swatch/action/Device.hpp"


#include "swatch/core/AbstractStub.hpp"


namespace swatch {
namespace action {


Device::Device( const swatch::core::AbstractStub& aStub) :
  ActionableObject(aStub.id, aStub.alias, aStub.loggerName),
  mStub(dynamic_cast<const action::DeviceStub&>(aStub)),
  mGateKeeperTables{getStub().id, getStub().role}
{
}


Device::~Device()
{
}


const DeviceStub& Device::getStub() const
{
  return mStub;
}


const std::vector<std::string>& Device::getGateKeeperContexts() const
{
  return mGateKeeperTables;
}


} // namespace action
} // namespace swatch

