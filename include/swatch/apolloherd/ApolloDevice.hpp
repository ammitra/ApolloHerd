#ifndef __SWATCH_APOLLOHERD_APOLLODEVICE_HPP__
#define __SWATCH_APOLLOHERD_APOLLODEVICE_HPP__

#include "swatch/action/Device.hpp"
#include "swatch/apolloherd/ApolloDeviceController.hpp"

namespace swatch {
namespace apolloherd {

class ApolloDevice : public action::Device {
public:
  ApolloDevice(const swatch::core::AbstractStub& aStub);
  virtual ~ApolloDevice();

  ApolloDeviceController& getController()
  {
    return mController;
  }

private:
  ApolloDeviceController mController;
};

}   // apolloherd
}   // swatch

#endif