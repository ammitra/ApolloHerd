
#include "swatch/action/test/DummySleepCommand.hpp"


#include "boost/chrono.hpp"

#include "swatch/action/test/DummyActionableObject.hpp"


using namespace swatch;
namespace swatch {
namespace action {
namespace test {

DummySleepCommand::DummySleepCommand(const std::string& aId, ActionableObject& aActionable) :
  Command(aId, aActionable, int(-33))
{
  registerParameter("n", uint32_t(50));
  registerParameter("millisecPerSleep", uint32_t(100));
}


DummySleepCommand::~DummySleepCommand()
{
}


Command::State DummySleepCommand::code(const core::ParameterSet& aParams)
{
  DummyActionableObject& res = getActionable<DummyActionableObject>();

  setStatusMsg("Dummy sleep command just started");

  unsigned int n(aParams.get<uint32_t>("n"));
  unsigned int millisecPerSleep(aParams.get<uint32_t>("millisecPerSleep"));

  for (unsigned int i = 0; i < n; i++) {
    boost::this_thread::sleep_for(boost::chrono::milliseconds(millisecPerSleep));

    std::ostringstream oss;
    oss << "Dummy sleep command for object '" << res.getPath() << "' progressed. " << i << " of " << n << " sleeps done; " << (n - i) * millisecPerSleep << " milli sec remain";
    setProgress( float(i) / n, oss.str());
  }

  setStatusMsg("Dummy sleep command for object '" + res.getPath() + "' completed");
  return State::kDone;
}

} /* namespace test */
} /* namespace action */
} /* namespace swatch */
