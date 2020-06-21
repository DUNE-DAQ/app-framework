/**
 *
 * @file FollyQueue_test.cxx FollyQueue class Unit Tests
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "appfwk/FollyQueue.hpp"

#define BOOST_TEST_MODULE FollyQueue_test // NOLINT
#include <boost/test/included/unit_test.hpp>

#include <chrono>

// For a first look at the code, you may want to skip past the
// contents of the unnamed namespace and move ahead to the actual test
// cases

namespace {

constexpr int max_testable_capacity = 1000000000; ///< The maximum capacity this test will attempt to check
constexpr double fractional_timeout_tolerance = 0.1; ///< The fraction of the timeout which the timing is allowed to be off by

/**
 * @brief Timeout to use for tests
 *
 * Don't set the timeout to zero, otherwise the tests will fail since they'd
 * expect the push/pop functions to execute instananeously
 */
constexpr auto timeout = std::chrono::milliseconds(1);
/**
 * @brief Timeout expressed in microseconds
*/
constexpr auto timeout_in_us =
  std::chrono::duration_cast<std::chrono::microseconds>(timeout).count();

dunedaq::appfwk::FollySPSCQueue<int> Queue("FollyQueue"); ///< Queue instance for the test

} // namespace ""

// This test case should run first. Make sure all other test cases depend on
// this.

BOOST_AUTO_TEST_CASE(sanity_checks)
{
  Queue.SetSize(10);

  BOOST_REQUIRE(!Queue.can_pop());

  auto starttime = std::chrono::steady_clock::now();
  Queue.push(999, timeout);
  auto push_time = std::chrono::steady_clock::now() - starttime;

  if (push_time > timeout) {
    auto push_time_in_us =
      std::chrono::duration_cast<std::chrono::microseconds>(push_time).count();

    BOOST_TEST_REQUIRE(false,
                       "Test failure: pushing element onto empty Queue "
                       "resulted in a timeout (function exited after "
                         << push_time_in_us << " microseconds, timeout is "
                         << timeout_in_us << " microseconds)");
  }

  BOOST_REQUIRE(Queue.can_pop());

  starttime = std::chrono::steady_clock::now();
  int popped_value ; 
  Queue.pop( popped_value, timeout);
  auto pop_time = std::chrono::steady_clock::now() - starttime;

  if (pop_time > timeout) {
    auto pop_time_in_us =
      std::chrono::duration_cast<std::chrono::microseconds>(pop_time).count();
    BOOST_TEST_REQUIRE(false,
                       "Test failure: popping element off Queue "
                       "resulted in a timeout (function exited after "
                         << pop_time_in_us << " microseconds, timeout is "
                         << timeout_in_us << " microseconds)");
  }

  BOOST_REQUIRE_EQUAL(popped_value, 999);
}

BOOST_AUTO_TEST_CASE(empty_checks,
                     *boost::unit_test::depends_on("sanity_checks"))
{

  try {
    while (Queue.can_pop()) {
      int popped_value ; 
      if ( ! Queue.pop(popped_value, timeout) ) {
	BOOST_TEST(false,
		   "False returned in call to FollyQueue::pop(); unable "
		   "to empty the Queue");
	break ;
      }
    }
  } catch (const std::runtime_error& err) {
    BOOST_WARN_MESSAGE(true, err.what());
  }
  
  BOOST_REQUIRE(!Queue.can_pop());
  
  // pop off of an empty Queue

  int popped_value ; 
  
  auto starttime = std::chrono::steady_clock::now();
  BOOST_TEST( ! Queue.pop(popped_value, timeout) ) ; 
  auto pop_duration = std::chrono::steady_clock::now() - starttime;
  
  const double fraction_of_pop_timeout_used = pop_duration / timeout;

  BOOST_CHECK_GT(fraction_of_pop_timeout_used,
                 1 - fractional_timeout_tolerance);
  BOOST_CHECK_LT(fraction_of_pop_timeout_used,
                 1 + fractional_timeout_tolerance);
}
