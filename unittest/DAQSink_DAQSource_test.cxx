/**
 * @file DAQSink_DAQSource_test.cxx Test application which demonstrates the
 * functionality of the QueueRegistry, DAQSink, and DAQSource
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "appfwk/DAQSink.hpp"
#include "appfwk/DAQSource.hpp"
#include "appfwk/StdDeQueue.hpp"

#include "ers/ers.hpp"

#define BOOST_TEST_MODULE DAQSink_DAQSource_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <map>
#include <string>
#include <utility>

using namespace dunedaq::appfwk;

BOOST_AUTO_TEST_SUITE(DAQSink_DAQSource_test)

/**
 * @brief Sample base class used in tests
 */
class SampleBaseClass
{
public:
  int base_class_value;
};

/**
 * @brief Sample derived class used in tests
 */
class SampleDerivedClass : public SampleBaseClass
{
public:
  int derived_class_value;
};

/**
 * @brief Initializes the QueueRegistry
 */
struct DAQSinkDAQSourceTestFixture
{
  DAQSinkDAQSourceTestFixture() {}

  void setup()
  {
    std::map<std::string, QueueConfig> queue_map = { { "dummy", { QueueConfig::queue_kind::kStdDeQueue, 100 } },
                                                     { "base_class_q", { QueueConfig::queue_kind::kStdDeQueue, 100 } },
                                                     { "derived_class_q", { QueueConfig::queue_kind::kStdDeQueue, 100 } },
                                                     { "mixed_class_q", { QueueConfig::queue_kind::kStdDeQueue, 100 } } };

    QueueRegistry::get().configure(queue_map);
  }
};

BOOST_TEST_GLOBAL_FIXTURE(DAQSinkDAQSourceTestFixture);

BOOST_AUTO_TEST_CASE(Construct)
{
  DAQSink<std::string> sink("dummy");
  DAQSource<std::string> source("dummy");

  [[maybe_unused]] DAQSink<int>* bad_sink;

  BOOST_REQUIRE_EXCEPTION(bad_sink = new DAQSink<int>("dummy"),
                          dunedaq::appfwk::DAQSinkConstructionFailed,
                          [&](dunedaq::appfwk::DAQSinkConstructionFailed) { return true; });

  [[maybe_unused]] DAQSource<int>* bad_source;

  BOOST_REQUIRE_EXCEPTION(bad_source = new DAQSource<int>("dummy"),
                          dunedaq::appfwk::DAQSourceConstructionFailed,
                          [&](dunedaq::appfwk::DAQSourceConstructionFailed) { return true; });
}

BOOST_AUTO_TEST_CASE(InitialConditions)
{

  DAQSink<std::string> sink("dummy");
  DAQSource<std::string> source("dummy");

  BOOST_REQUIRE(sink.can_push());
  BOOST_REQUIRE(!source.can_pop());
}

BOOST_AUTO_TEST_CASE(DataFlow)
{

  DAQSink<std::string> sink("dummy");
  DAQSource<std::string> source("dummy");

  sink.push(std::move("hello"));
  std::string res("undefined");
  try {
    source.pop(res);
  } catch (const dunedaq::appfwk::QueueTimeoutExpired& ex) {
  }
  BOOST_REQUIRE_EQUAL(res, "hello");
}

BOOST_AUTO_TEST_CASE(Exceptions)
{

  DAQSink<std::string> sink("dummy");
  DAQSource<std::string> source("dummy");
  std::string res;

  BOOST_REQUIRE(!source.can_pop());
  BOOST_CHECK_THROW(source.pop(res), dunedaq::appfwk::QueueTimeoutExpired);

  for (int ii = 0; ii < 100; ++ii) {
    BOOST_REQUIRE(sink.can_push());
    sink.push("aaAaaa");
  }

  BOOST_REQUIRE(!sink.can_push());
  BOOST_REQUIRE_EXCEPTION(sink.push("bbBbbb"),
                          dunedaq::appfwk::QueueTimeoutExpired,
                          [&](dunedaq::appfwk::QueueTimeoutExpired) { return true; });
}

BOOST_AUTO_TEST_CASE(SameQueueDifferentType)
{
  try {
    DAQSink<int> sink("dummy");
    BOOST_TEST_REQUIRE(false, "Test failure: attempt to use an existing Queue with a different type succeeded when we expected it to fail");
  } catch (...) {}
  try {
    DAQSource<int> source("dummy");
    BOOST_TEST_REQUIRE(false, "Test failure: attempt to use an existing Queue with a different type succeeded when we expected it to fail");
  } catch (...) {}
}

BOOST_AUTO_TEST_CASE(BaseClassInstance)
{
  int expected_base_class_value = 111;

  DAQSink<SampleBaseClass> sink("base_class_q");
  DAQSource<SampleBaseClass> source("base_class_q");

  SampleBaseClass sbc;
  sbc.base_class_value = expected_base_class_value;
  sink.push(std::move(sbc));

  SampleBaseClass res;
  res.base_class_value = 2 * expected_base_class_value;
  try {
    source.pop(res);
  } catch (const dunedaq::appfwk::QueueTimeoutExpired& ex) {
    BOOST_TEST_REQUIRE(false, "Test failure: unexpected timeout exception throw from pop");
  }
  BOOST_REQUIRE_EQUAL(res.base_class_value, expected_base_class_value);
}

BOOST_AUTO_TEST_CASE(DerivedClassInstance)
{
  int expected_base_class_value = 123;
  int expected_derived_class_value = 456;

  DAQSink<SampleDerivedClass> sink("derived_class_q");
  DAQSource<SampleDerivedClass> source("derived_class_q");

  SampleDerivedClass sdc;
  sdc.base_class_value = expected_base_class_value;
  sdc.derived_class_value = expected_derived_class_value;
  sink.push(std::move(sdc));

  SampleDerivedClass res;
  res.base_class_value = 2 * expected_base_class_value;
  res.derived_class_value = 2 * expected_derived_class_value;
  try {
    source.pop(res);
  } catch (const dunedaq::appfwk::QueueTimeoutExpired& ex) {
    BOOST_TEST_REQUIRE(false, "Test failure: unexpected timeout exception throw from pop");
  }
  BOOST_REQUIRE_EQUAL(res.base_class_value, expected_base_class_value);
  BOOST_REQUIRE_EQUAL(res.derived_class_value, expected_derived_class_value);
}

BOOST_AUTO_TEST_CASE(MixedClassInstances)
{
  DAQSink<SampleDerivedClass*> sink("mixed_class_q");
  DAQSource<SampleBaseClass*> source("mixed_class_q");
}

BOOST_AUTO_TEST_SUITE_END()
