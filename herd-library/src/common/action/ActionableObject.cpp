
#include "swatch/action/ActionableObject.hpp"


#include <sstream>                      // for operator<<, basic_ostream, etc
#include <stdexcept>                    // for out_of_range
#include <typeinfo>                     // for type_info
#include <utility>                      // for make_pair

#include "boost/chrono/duration.hpp"    // for operator<, milliseconds, etc
#include "boost/ratio/ratio.hpp"        // for ratio
#include "boost/thread/thread.hpp"   // for sleep_for

#include "log4cplus/logger.h"           // for Logger
#include "log4cplus/loggingmacros.h"

#include "swatch/action/Command.hpp"
#include "swatch/action/CommandSequence.hpp"
#include "swatch/action/StateMachine.hpp"
#include "swatch/core/LeafObject.hpp"
#include "swatch/core/utilities.hpp"
#include "swatch/logger/Logger.hpp"



using namespace std;

namespace swatch {
namespace action {


//------------------------------------------------------------------------------------
ActionableObject::ActionableObject( const std::string& aId, const std::string& aAlias, const std::string& aLoggerName ) :
  Object( aId , aAlias ),
  mStatus(),
  mLogger(swatch::logger::Logger::getInstance(aLoggerName))
{
}


//------------------------------------------------------------------------------------
ActionableObject::~ActionableObject()
{
}


//------------------------------------------------------------------------------------
StateMachine& ActionableObject::getStateMachine( const std::string& aId )
{
  try {
    return *(mFSMs.at( aId ));
  }
  catch ( const std::out_of_range& e ) {
    throw StateMachineNotFoundInActionableObject("Unable to find state machine with ID '" + aId + "' in object '" + getPath() + "'");
  }
}


//------------------------------------------------------------------------------------
const StateMachine& ActionableObject::getStateMachine( const std::string& aId ) const
{
  try {
    return *(mFSMs.at( aId ));
  }
  catch ( const std::out_of_range& e ) {
    throw StateMachineNotFoundInActionableObject("Unable to find state machine with ID '" + aId + "' in object '" + getPath() + "'");
  }
}


//------------------------------------------------------------------------------------
std::set<std::string> ActionableObject::getCommands() const
{
  std::set<std::string> lNames;
  for ( CommandMap_t::value_type p : mCommands)
    lNames.insert( p.first );

  return lNames;
}


//------------------------------------------------------------------------------------
std::set<std::string> ActionableObject::getSequences() const
{
  std::set<std::string> lNames;
  for (CommandSequenceMap_t::value_type p : mCommandSequences) {
    lNames.insert( p.first );
  }
  return lNames;
}


//------------------------------------------------------------------------------------
std::set<std::string> ActionableObject::getStateMachines() const
{
  std::set<std::string> lNames;
  for (StateMachineMap_t::value_type p: mFSMs)
    lNames.insert( p.first );

  return lNames;
}


//------------------------------------------------------------------------------------
Command& ActionableObject::getCommand( const std::string& aId )
{
  try {
    return *(mCommands.at( aId ));
  }
  catch ( const std::out_of_range& e ) {
    throw CommandNotFoundInActionableObject("Unable to find Command with ID '" + aId + "' in object '" + getPath() + "'" );
  }
}


//------------------------------------------------------------------------------------
const Command& ActionableObject::getCommand( const std::string& aId ) const
{
  try {
    return *(mCommands.at( aId ));
  }
  catch ( const std::out_of_range& e ) {
    throw CommandNotFoundInActionableObject("Unable to find Command with ID '" + aId + "' in object '" + getPath() + "'" );
  }
}


//------------------------------------------------------------------------------------
CommandSequence& ActionableObject::getSequence( const std::string& aId )
{
  try {
    return *(mCommandSequences.at( aId ));
  }
  catch ( const std::out_of_range& e ) {
    throw CommandSequenceNotFoundInActionableObject("Unable to find CommandSequence with ID '" + aId + "' in object '" + getPath() + "'" );
  }
}


//------------------------------------------------------------------------------------
const CommandSequence& ActionableObject::getSequence( const std::string& aId ) const
{
  try {
    return *(mCommandSequences.at( aId ));
  }
  catch ( const std::out_of_range& e ) {
    throw CommandSequenceNotFoundInActionableObject("Unable to find CommandSequence with ID '" + aId + "' in object '" + getPath() + "'" );
  }
}


//------------------------------------------------------------------------------------
Command& ActionableObject::registerCommand( const std::string& aId , Command* aCommand )
{
  if (mCommands.count(aId)) {
    delete aCommand;
    throw CommandAlreadyExistsInActionableObject("Command With ID '"+aId+"' already exists" );
  }
  this->addObj(aCommand);
  mCommands.insert( std::make_pair( aId , aCommand ) );
  return *aCommand;
}


//------------------------------------------------------------------------------------
CommandSequence& ActionableObject::registerSequence( const std::string& aId, const std::string& aFirstCommandId, const std::string& aFirstCommandNamespace)
{
  return registerSequence(aId, "", aFirstCommandId, aFirstCommandNamespace);
}


//------------------------------------------------------------------------------------
CommandSequence& ActionableObject::registerSequence( const std::string& aId, const std::string& aAlias, const std::string& aFirstCommandId, const std::string& aFirstCommandNamespace)
{
  if (mCommandSequences.count(aId)) {
    throw CommandSequenceAlreadyExistsInActionableObject("CommandSequence With ID '"+aId+"' already exists" );
  }
  CommandSequence* lSequence = new CommandSequence(aId, aAlias, *this, mStatus, aFirstCommandId, aFirstCommandNamespace);
  addObj(lSequence);
  mCommandSequences.insert( std::make_pair( aId , lSequence ) );
  return *lSequence;
}


//------------------------------------------------------------------------------------
CommandSequence& ActionableObject::registerSequence( const std::string& aId, Command& aFirstCommand, const std::string& aFirstCommandNamespace)
{
  return registerSequence(aId, "", aFirstCommand, aFirstCommandNamespace);
}


//------------------------------------------------------------------------------------
CommandSequence& ActionableObject::registerSequence( const std::string& aId, const std::string& aAlias, Command& aFirstCommand, const std::string& aFirstCommandNamespace)
{
  if (mCommandSequences.count(aId)) {
    throw CommandSequenceAlreadyExistsInActionableObject("CommandSequence With ID '"+aId+"' already exists" );
  }
  CommandSequence* lSequence = new CommandSequence(aId, aAlias, *this, mStatus, aFirstCommand, aFirstCommandNamespace);
  addObj(lSequence);
  mCommandSequences.insert( std::make_pair( aId , lSequence ) );
  return *lSequence;
}


//------------------------------------------------------------------------------------
StateMachine& ActionableObject::registerStateMachine( const std::string& aId, const std::string& aInitialState, const std::string& aErrorState )
{
  return registerStateMachine(aId, "", aInitialState, aErrorState);
}

//------------------------------------------------------------------------------------
StateMachine& ActionableObject::registerStateMachine( const std::string& aId, const std::string& aAlias, const std::string& aInitialState, const std::string& aErrorState )
{
  if (mFSMs.count(aId))
    throw StateMachineAlreadyExistsInActionableObject("State machine With ID '"+aId+"' already exists" );

  StateMachine* lFSM = new StateMachine(aId, aAlias, *this, mStatus, aInitialState, aErrorState);
  addObj(lFSM);
  mFSMs.insert( std::make_pair( aId , lFSM ) );
  return *lFSM;
}


//------------------------------------------------------------------------------------
ActionableObject::Status_t ActionableObject::getStatus() const
{
  ActionableStatusGuard lGuard(mStatus);
  return mStatus.getSnapshot(lGuard);
}


//------------------------------------------------------------------------------------
void ActionableObject::Deleter::operator ()(LeafObject* aObject)
{
  if (ActionableObject* lActionableObj = dynamic_cast<ActionableObject*>(aObject)) {
    LOG4CPLUS_INFO(lActionableObj->getLogger(), aObject->getPath() << " : ActionableObject deleter called");

    {
      ActionableStatusGuard lGuard(lActionableObj->mStatus);
      lActionableObj->mStatus.kill(lGuard);
    }

    //TODO (low-ish priority): Eventually replace this "spinning" do-loop with a more efficient implementation based on ActionableObject/Functionoid methods that use conditional variables behind-the-scenes
    do {
      boost::this_thread::sleep_for( boost::chrono::milliseconds(1) );
    }
    while ( lActionableObj->getStatus().isRunning() );

    LOG4CPLUS_INFO(lActionableObj->getLogger(), aObject->getPath() << " : ActionableObject now being deleted");

    delete lActionableObj;
  }
  else {
    log4cplus::Logger lLogger = swatch::logger::Logger::getInstance("swatch.action.ActionableObject");
    LOG4CPLUS_WARN(lLogger, "ActionableObject::Deleter being used on object '" << aObject->getPath() << "' of type '" << core::demangleName(typeid(*aObject).name()) << "' that doesn't inherit from ActionableObject");
    delete aObject;
  }
}


log4cplus::Logger& ActionableObject::getLogger()
{
  return mLogger;
}

}
}
