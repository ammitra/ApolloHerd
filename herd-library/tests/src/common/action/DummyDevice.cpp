
#include "swatch/action/test/DummyDevice.hpp"


#include "swatch/core/Factory.hpp"


SWATCH_REGISTER_CLASS(swatch::action::test::DummyDevice)


namespace swatch {
namespace action {
namespace test {

DummyDevice::DummyDevice(const core::AbstractStub& aStub) :
  Device(aStub)
{
}


DummyDevice::~DummyDevice()
{
}


StateMachine& DummyDevice::registerStateMachine(const std::string& aId, const std::string& aInitialState, const std::string& aErrorState)
{
  return ActionableObject::registerStateMachine(aId, aInitialState, aErrorState);
}

} /* namespace test */
} /* namespace server */
} /* namespace swatch */
