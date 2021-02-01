/**
 * @file DAQModuleHelper.hpp Some utility functions for writing modules.
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef APPFWK_INCLUDE_APPFWK_DAQMODULEHELPER_HPP_
#define APPFWK_INCLUDE_APPFWK_DAQMODULEHELPER_HPP_

#include "appfwk/cmd/Structs.hpp"

#include <map>
#include <string>
#include <vector>

namespace dunedaq {

namespace appfwk {

using IndexedQueueInfos_t = std::map<std::string, cmd::QueueInfo>;

/// @brief Return map of QueueInfo indexed by their "name".
///
/// The iniobj is as given to a module's init() method.
IndexedQueueInfos_t
queue_index(const nlohmann::json& iniobj, std::vector<std::string> required = {});

/// @brief Return vector of QueueInfo from the init() object
cmd::QueueInfos
queue_infos(const nlohmann::json& iniobj);

/// @brief Base case for the variadic template function below
void
connect_sinks_and_sources(appfwk::cmd::ModInit&) {}

/// @brief Connect a list of DAQSinks and DAQSources to the corresponding queue names
///
/// The arguments to this function are a ModInit object followed by
/// pairs of unique_ptr<DAQSink or DAQSource> and the name of the
/// queue to connect the sink or source to. For example:
///
///     std::unique_ptr<DAQSink<MyType1>> sink1;
///     std::unique_ptr<DAQSink<MyType2>> sink2;
///     std::unique_ptr<DAQSource<MyType3>> source1;
///     connect_sinks_and_sources(conf,
///                               sink1,   "queue1",
///                               sink2,   "queue2",
///                               source1, "queue2");
template<typename T, typename... Args>
void
connect_sinks_and_sources(appfwk::cmd::ModInit& conf, std::unique_ptr<T>& p, const char* c, Args&&... args)
{
  // Connect the first queue in the list
  for (const auto& qi : conf.qinfos) {
    if (qi.name == c) {
      p.reset(new T(qi.inst));
    }
  }
  // Recursively do the rest
  connect_sinks_and_sources(conf, args...);
}

} // namespace appfwk

} // namespace dunedaq

#endif // APPFWK_INCLUDE_APPFWK_DAQMODULEHELPER_HPP_

// Local Variables:
// c-basic-offset: 2
// End:
