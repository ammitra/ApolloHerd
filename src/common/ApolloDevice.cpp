#include "swatch/apolloherd/ApolloDevice.hpp"
#include "swatch/apolloherd/commands/ApolloAccess.hpp"
#include "swatch/core/Factory.hpp"
#include "swatch/apolloherd/ApolloDeviceController.hpp"

// register device to factory
SWATCH_REGISTER_CLASS(swatch::apolloherd::ApolloDevice)

namespace swatch {
namespace apolloherd {

ApolloDevice::ApolloDevice(const core::AbstractStub& aStub) :
  Device(aStub),
  mController(getStub().uri, getStub().addressTable)
{
  // register command
  auto& dev_cmd = registerCommand<commands::ApolloAccess>("dev_cmd");
}

ApolloDevice::~ApolloDevice()
{
}

}   // apolloherd
}   // swatch

