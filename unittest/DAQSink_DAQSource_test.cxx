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
#include "appfwk/cmd/Nljs.hpp"

#include "ers/ers.h"

#define BOOST_TEST_MODULE DAQSink_DAQSource_test // NOLINT

#include "boost/test/unit_test.hpp"

#include <map>
#include <string>
#include <utility>

using namespace dunedaq::appfwk;

BOOST_AUTO_TEST_SUITE(DAQSink_DAQSource_test)

/**
 * @brief Initializes the QueueRegistry
 */
struct DAQSinkDAQSourceTestFixture
{
  DAQSinkDAQSourceTestFixture() {}

  void setup()
  {
    std::map<std::string, QueueConfig> queue_map = { { "dummy", { QueueConfig::queue_kind::kStdDeQueue, 100 } },
                                                     { "dummy2", { QueueConfig::queue_kind::kStdDeQueue, 100 } } };

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

BOOST_AUTO_TEST_CASE(FromName)
{
  dunedaq::appfwk::cmd::ModInit sink_init;
  sink_init.qinfos.push_back({"dummy2", "myname", "output"});
  nlohmann::json j=sink_init;

  DAQSink<std::string> sink(j, "myname");
  DAQSink<std::string> sink2(sink_init, "myname");
  BOOST_REQUIRE(sink.can_push());
  BOOST_REQUIRE(sink2.can_push());

  dunedaq::appfwk::cmd::ModInit source_init;
  source_init.qinfos.push_back({"dummy2", "myname", "input"});
  nlohmann::json j2=source_init;

  DAQSource<std::string> source(j2, "myname");
  DAQSource<std::string> source2(source_init, "myname");
  BOOST_REQUIRE(!source.can_pop());
  BOOST_REQUIRE(!source2.can_pop());
}

BOOST_AUTO_TEST_SUITE_END()
