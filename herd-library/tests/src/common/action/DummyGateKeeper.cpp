
#include "swatch/action/test/DummyGateKeeper.hpp"


namespace swatch {
namespace action {
namespace test {


DummyGateKeeper::DummyGateKeeper():
  GateKeeper( "__dummyGK__" )
{
}


DummyGateKeeper::~DummyGateKeeper()
{
}


void DummyGateKeeper::addContext(const std::string& aId, const ParametersContext_t& aContext)
{
  add(aId, aContext);
}


} /* namespace test */
} /* namespace action */
} /* namespace swatch */
