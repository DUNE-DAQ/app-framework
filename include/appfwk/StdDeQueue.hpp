#ifndef APPFWK_INCLUDE_APPFWK_STDDEQUEUE_HPP_
#define APPFWK_INCLUDE_APPFWK_STDDEQUEUE_HPP_

/**
 *
 * @file StdDeQueue.hpp
 *
 * A std::deque-based implementation of Queue
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "appfwk/Queue.hpp"

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>

namespace dunedaq {
namespace appfwk {
/**
 * @brief A Queue Implementation that uses a std::deque as its backend
 * @tparam T Data Type to be stored in the std::deque
 */
template<class T>
class StdDeQueue : public Queue<T>
{
public:
  using value_type = T;                                   ///< Type of data stored in the StdDeQueue
  using duration_type = typename Queue<T>::duration_type; ///< Type used for expressing timeouts

  /**
   * @brief StdDeQueue Constructor
   * @param name Name of this StdDeQueue instance
   */
  explicit StdDeQueue(const std::string& name, size_t capacity);

  bool can_pop() const noexcept override { return fSize.load() > 0; }
  void pop(value_type& val, const duration_type&) override; // Throws QueueTimeoutExpired if a timeout occurs

  bool can_push() const noexcept override { return fSize.load() < this->GetCapacity(); }
  void push(value_type&&, const duration_type&) override; // Throws QueueTimeoutExpired if a timeout occurs

  // Delete the copy and move operations since various member data instances
  // (e.g., of std::mutex or of std::atomic) aren't copyable or movable

  StdDeQueue(const StdDeQueue&) = delete;            ///< StdDeQueue is not copy-constructible
  StdDeQueue& operator=(const StdDeQueue&) = delete; ///< StdDeQueue is not copy-assignable
  StdDeQueue(StdDeQueue&&) = delete;                 ///< StdDeQueue is not move-constructible
  StdDeQueue& operator=(StdDeQueue&&) = delete;      ///< StdDeQueue is not move-assignable

private:
  void try_lock_for(std::unique_lock<std::mutex>&, const duration_type&);

  std::deque<value_type> fDeque;
  std::atomic<size_t> fSize = 0;

  std::mutex fMutex;
  std::condition_variable fNoLongerFull;
  std::condition_variable fNoLongerEmpty;
};

} // namespace appfwk

} // namespace dunedaq

#include "detail/StdDeQueue.hxx"

#endif // APPFWK_INCLUDE_APPFWK_STDDEQUEUE_HPP_
