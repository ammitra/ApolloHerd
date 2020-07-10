#ifndef __SWATCH_DUMMY_APOLLODEVICE_HPP__
#define __SWATCH_DUMMY_APOLLODEVICE_HPP__

#include "swatch/action/Device.hpp"
#include "swatch/dummy/ApolloDeviceController.hpp"

namespace swatch {
namespace dummy {

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

}   // dummy
}   // swatch

#endif  // __SWATCH_DUMMY_APOLLODEVICE_HPP__