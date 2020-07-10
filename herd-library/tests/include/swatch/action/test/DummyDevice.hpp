
#ifndef __SWATCH_ACTION_TEST_DUMMYDEVICE_HPP__
#define __SWATCH_ACTION_TEST_DUMMYDEVICE_HPP__


#include "swatch/action/Device.hpp"


namespace swatch {
namespace action {
namespace test {

class DummyDevice : public action::Device {
public:
  DummyDevice( const core::AbstractStub& aStub );
  virtual ~DummyDevice();

  // Expose registerFunctionoid template method as public for tests
  template< typename T>
  T& registerCommand( const std::string& aId )
  {
    return ActionableObject::registerCommand<T>(aId);
  }

  StateMachine& registerStateMachine(const std::string& aId, const std::string& aInitialState, const std::string& aErrorState);
};


} /* namespace test */
} /* namespace server */
} /* namespace swatch */

#endif /* SWATCH_ACTION_TEST_DUMMYDEVICE_HPP */
