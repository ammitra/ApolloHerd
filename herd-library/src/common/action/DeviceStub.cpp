
#include "swatch/action/DeviceStub.hpp"


namespace swatch {
namespace action {


DeviceStub::DeviceStub(const std::string& aId) :
  AbstractStub(aId)
{
}

DeviceStub::DeviceStub(const std::string& aId, const std::string& aCreator, const std::string& aRole, const std::string& aUri, const std::string& aAddressTable) :
  AbstractStub(aId),
  creator(aCreator),
  role(aRole),
  uri(aUri),
  addressTable(aAddressTable)
{
}


DeviceStub::~DeviceStub()
{
}


} // namespace processor
} // namespace swatch
