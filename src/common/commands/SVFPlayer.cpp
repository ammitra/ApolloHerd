#include "swatch/dummy/commands/SVFPlayer.hpp"
#include "swatch/dummy/ApolloDevice.hpp"
#include <BUTool/CommandReturn.hh>

namespace swatch {
namespace dummy {
namespace commands{

SVFPlayer::SVFPlayer(const std::string& aId, action::ActionableObject& aActionable)
    : Command(aId, aActionable, std::string("svfplayer"))
    {
        registerParameter<std::string>("SVF file  ", " ");
        registerParameter<std::string>("XVC device", " ");
    }

SVFPlayer::~SVFPlayer()
{
}

action::Command::State SVFPlayer::code(const core::ParameterSet& aParams)
{
    ApolloDeviceController& lController = getActionable<ApolloDevice>().getController();

    std::string SVF = aParams.get<std::string>("SVF file  ");
    std::string XVC = aParams.get<std::string>("XVC device");

    // concatenating the two parameters into space-separated string
    std::string SVF_XVC;
    SVF_XVC = SVF + ' ' + XVC;

    State lState = kDone;

    if (lController.svfplayer(SVF_XVC) == CommandReturn::status::BAD_ARGS)
        throw core::RuntimeError("bad arguments");
    else
        lState = kDone;
    
    return lState;
}

}   // commands
}   // dummy
}   // swatch