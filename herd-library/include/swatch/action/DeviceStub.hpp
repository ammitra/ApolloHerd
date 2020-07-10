
#ifndef __SWATCH_ACTION_DEVICESTUB_HPP__
#define __SWATCH_ACTION_DEVICESTUB_HPP__


// C++ headers
#include <string>                       // for string

#include "swatch/core/AbstractStub.hpp"


namespace swatch {
namespace action {


//! Structure that holds the data required to construct a Device
class DeviceStub : public core::AbstractStub {
public:
  DeviceStub(const std::string& aId);

  DeviceStub(const std::string& aId, const std::string& aCreator, const std::string& aRole, const std::string& aUri, const std::string& aAddressTable);

  virtual ~DeviceStub();

  //! Class to create the Device object
  std::string creator;

  //! Role of the device
  std::string role;

  //! Uri to access the hardware resource
  std::string uri;

  //! Address table
  std::string addressTable;
};


} // namespace action
} // namespace swatch


#endif /* __SWATCH_ACTION_DEVICESTUB_HPP__ */
