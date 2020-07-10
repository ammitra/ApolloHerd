
#ifndef __SWATCH_ACTION_DEVICE_HPP__
#define __SWATCH_ACTION_DEVICE_HPP__


// Standard headers
#include <stdint.h>                     // for uint32_t, uint64_t
#include <string>                       // for string
#include <vector>                       // for vector

// SWATCH headers
#include "swatch/action/ActionableObject.hpp"
#include "swatch/core/exception.hpp"
#include "swatch/action/DeviceStub.hpp"


namespace swatch {

namespace core {
class AbstractStub;
}

namespace action {


class Device : public action::ActionableObject {

protected:
  Device(const swatch::core::AbstractStub& );

public:

  Device(const Device&) = delete;
  Device& operator=(const Device&) = delete;

  virtual ~Device();

  const DeviceStub& getStub() const;

  virtual const std::vector<std::string>& getGateKeeperContexts() const;

private:
  const DeviceStub mStub;

  mutable std::vector<std::string> mGateKeeperTables;
};


}
}

#endif  /* __SWATCH_ACTION_DEVICE_HPP__ */

