#include "swatch/dummy/ApolloDeviceController.hpp"

namespace swatch {
namespace dummy {

ApolloDeviceController::ApolloDeviceController(const std::string& aURI, const std::string& aAddrTable)
{
    ptrSMDevice = new BUTool::ApolloSMDevice(arg);
}

ApolloDeviceController::~ApolloDeviceController()
{
    if (NULL != ptrSMDevice) {
        delete ptrSMDevice;
    }
}

}       // dummy
}       // swatch