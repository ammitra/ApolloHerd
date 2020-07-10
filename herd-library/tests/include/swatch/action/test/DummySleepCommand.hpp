/**
 * @file    DummySleepCommand.hpp
 * @author  Tom Williams
 * @date    February 2015
 */

#ifndef __SWATCH_ACTION_TEST_DUMMYSLEEPCOMMAND_HPP__
#define __SWATCH_ACTION_TEST_DUMMYSLEEPCOMMAND_HPP__


#include "swatch/action/Command.hpp"


namespace swatch {
namespace action {
namespace test {


class DummySleepCommand: public Command {
public:
  DummySleepCommand( const std::string& aId, ActionableObject& aActionable );

  virtual ~DummySleepCommand();

  virtual Command::State code(const core::ParameterSet& aParams);
};


} /* namespace test */
} /* namespace action */
} /* namespace swatch */

#endif /* __SWATCH_ACTION_TEST_DUMMYCOMMAND_HPP__ */
