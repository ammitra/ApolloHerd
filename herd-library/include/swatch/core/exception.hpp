/**
 * @file    exception.hpp
 * @author  Alessandro Thea
 * @date    July 2014
 */

#ifndef __SWATCH_CORE_EXCEPTION_HPP__
#define __SWATCH_CORE_EXCEPTION_HPP__


// Standard headers
#include <exception>
#include <string>

// boost headers
#include <boost/thread/thread.hpp>


//! MACRO to create definitions for the derived exceptions
#define SWATCH_DEFINE_EXCEPTION(T) 				\
	class T : public swatch::core::exception 	\
	{											\
		public:									\
		T() : swatch::core::exception()			\
		{}										\
		T(const std::string& aWhat) :			\
		swatch::core::exception(aWhat)			\
		{}										\
	};


namespace swatch {
namespace core {

class exception : public std::exception {
public:

  /**
   * Constructor
   */
  exception() throw ();

  /**
   * Overloaded constructor
   */
  exception(const std::string& aWhat);

  /**
   * Copy constructor
   * @param aOther The to copy to build this exception object
   */
  exception(const exception& aOther) throw ();

  /**
   * Assignment operator
   * @param aOther The exception object to assign to this one
   * @return Reference to a new exception object
   */
  exception& operator=(const exception& aOther) throw ();

  /**
   * Destructor. Destruction delegated to subclasses
   */
  virtual ~exception() throw ();

  /**
   * Retrieve the reason for throwing the exception
   * @return const char* containing the reason the exception was thrown
   */
  virtual const char* what() const throw ();


protected:

  //! Thread ID in which the exception was thrown
  boost::thread::id mThreadId;

  //! Description of the exception
  std::string mWhat;
};


SWATCH_DEFINE_EXCEPTION(RuntimeError)
SWATCH_DEFINE_EXCEPTION(InvalidArgument)

} // namespace core
} // namespace swatch

#endif /* __SWATCH_CORE_EXCEPTION_HPP__ */
