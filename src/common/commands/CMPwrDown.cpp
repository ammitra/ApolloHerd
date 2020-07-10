#include "swatch/dummy/commands/CMPwrDown.hpp"
#include "swatch/dummy/ApolloDevice.hpp"
#include <BUTool/CommandReturn.hh>

namespace swatch {
namespace dummy {
namespace commands {

CMPwrDown::CMPwrDown(const std::string& aId, action::ActionableObject& aActionable)
    : Command(aId, aActionable, std::string("cmpwrdown"))
    {
        registerParameter<std::string>("CM ID   ", " ");
        registerParameter<std::string>("wait (s)", " ");
    }

CMPwrDown::~CMPwrDown()
{
}

action::Command::State CMPwrDown::code(const core::ParameterSet& aParams)
{
    ApolloDeviceController& lController = getActionable<ApolloDevice>().getController();

    std::string CMID = aParams.get<std::string>("CM ID   ");
    std::string WAIT = aParams.get<std::string>("wait (s)");

    std::string CMID_WAIT;
    CMID_WAIT = CMID + ' ' + WAIT;

    State lState = kDone;

    if (lController.cmpwrdown(CMID_WAIT) == CommandReturn::status::BAD_ARGS)
        throw core::RuntimeError("bad arguments");
    else
        lState = kDone;

    return lState;
}


}   // commands
}   // dummy
}   // swatch