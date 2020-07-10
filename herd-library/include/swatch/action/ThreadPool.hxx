
#ifndef __SWATCH_ACTION_THREADPOOL_HXX__
#define __SWATCH_ACTION_THREADPOOL_HXX__

// IWYU pragma: private, include "swatch/action/ThreadPool.hpp"

#include <boost/function.hpp>
#include <boost/bind.hpp>


namespace swatch {
namespace action {

template<class OBJECT, class ResourceGuardType>
void ThreadPool::addTask(OBJECT* aCmd,
                         boost::function<void(OBJECT*, std::shared_ptr<ResourceGuardType>, const core::ParameterSet&)> aFunction,
                         const std::shared_ptr<ResourceGuardType>& aResourceGuard,
                         const core::ParameterSet& aParamSet)
{
  // create packed_task
  boost::packaged_task<void> lTask(boost::bind(aFunction, aCmd, aResourceGuard, boost::ref(aParamSet)));
  {
    // lock mutex
    boost::lock_guard<boost::mutex> lGuard(mQueueMutex);
    if (mStop) {
      throw OperationOnStoppedThreadPool(
        "ThreadPool is stopped, cannot schedule tasks.");
    }
    mTasks.push_back(boost::move(lTask));
  }
  mCondition.notify_one();
}

template<class OBJECT, class ResourceGuardType>
void ThreadPool::addTask( OBJECT* aCmd , boost::function<void(OBJECT*, std::shared_ptr<ResourceGuardType>)> aFunction, const std::shared_ptr<ResourceGuardType>& aResourceGuard )
{
  // create packed_task
  boost::packaged_task<void> lTask(boost::bind(aFunction, aCmd, aResourceGuard));
  {
    // lock mutex
    boost::lock_guard<boost::mutex> lGuard(mQueueMutex);
    if (mStop) {
      throw OperationOnStoppedThreadPool(
        "ThreadPool is stopped, cannot schedule tasks.");
    }
    mTasks.push_back(boost::move(lTask));
  }
  mCondition.notify_one();
}


} // namespace action
} // namespace swatch

#endif /* __SWATCH_ACTION_THREADPOOL_HXX__ */
