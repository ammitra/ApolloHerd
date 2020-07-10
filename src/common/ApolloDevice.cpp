#include "swatch/dummy/ApolloDevice.hpp"
#include "swatch/dummy/commands/ApolloAccess.hpp"
#include "swatch/dummy/commands/Read.hpp"
#include "swatch/dummy/commands/SVFPlayer.hpp"
#include "swatch/dummy/commands/CMPwrUp.hpp"
#include "swatch/dummy/commands/CMPwrDown.hpp"
#include "swatch/core/Factory.hpp"
#include "swatch/dummy/ApolloDeviceController.hpp"

// register device to factory
SWATCH_REGISTER_CLASS(swatch::dummy::ApolloDevice)

namespace swatch {
namespace dummy {

ApolloDevice::ApolloDevice(const core::AbstractStub& aStub) :
    Device(aStub),
    mController(getStub().uri, getStub().addressTable)
    {
        // register commands 
        auto& dev_cmd = registerCommand<commands::ApolloAccess>("dev_cmd");
        auto& read = registerCommand<commands::Read>("read");
        auto& svfplayer = registerCommand<commands::SVFPlayer>("svfplayer");
        auto& cmpwrup = registerCommand<commands::CMPwrUp>("cmpwrup");
        auto& cmpwrdown = registerCommand<commands::CMPwrDown>("cmpwrdown");
    }

ApolloDevice::~ApolloDevice()
{
}

}       // dummy
}       // swatch