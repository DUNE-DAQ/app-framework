/**
 * @file Application.cpp Application implementataion
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "appfwk/Application.hpp"

#include "appfwk/Issues.hpp"
#include "appfwk/appinfo/InfoNljs.hpp"
#include "appfwk/cmd/Nljs.hpp"
#include "rcif/cmd/Nljs.hpp"
#include "rcif/runinfo/InfoNljs.hpp"

#include "logging/Logging.hpp"

#include <string>

namespace dunedaq {
namespace appfwk {

Application::Application(std::string appname, std::string partition, std::string cmdlibimpl, std::string opmonlibimpl)
  : NamedObject(appname)
  , m_partition(partition)
  , m_info_mgr(opmonlibimpl)
  , m_state("NONE")
  , m_busy(false)
  , m_error(false)
  , m_initialized(false)
{
  m_runinfo.running = false;
  m_runinfo.runno = 0;
  m_runinfo.runtime = 0;

  m_fully_qualified_name = partition + "_" + appname;
  m_cmd_fac = cmdlib::make_command_facility(cmdlibimpl);
}

void
Application::init()
{
  m_cmd_fac->set_commanded(*this, get_name());
  m_info_mgr.set_provider(*this);
  m_initialized = true;
}

void
Application::run(std::atomic<bool>& end_marker)
{
  if (!m_initialized) {
    throw ApplicationNotInitialized(ERS_HERE, get_name());
  }

  setenv("DUNEDAQ_OPMON_INTERVAL", "10", 0);
  setenv("DUNEDAQ_OPMON_LEVEL", "1", 0);

  std::stringstream s1(getenv("DUNEDAQ_OPMON_INTERVAL"));
  std::stringstream s2(getenv("DUNEDAQ_OPMON_LEVEL"));
  uint32_t interval = 0; // NOLINT(build/unsigned)
  uint32_t level = 0;    // NOLINT(build/unsigned)
  s1 >> interval;
  s2 >> level;

  m_info_mgr.start(interval, level);
  m_cmd_fac->run(end_marker);
  m_info_mgr.stop();
}

void
Application::execute(const dataobj_t& cmd_data)
{

  auto rc_cmd = cmd_data.get<rcif::cmd::RCCommand>();
  std::string cmdname = rc_cmd.id;
  if (!is_cmd_valid(cmd_data)) {
    throw InvalidCommand(ERS_HERE, cmdname, get_state(), m_error.load(), m_busy.load());
  }

  m_busy.store(true);

  if (cmdname == "start") {
    auto cmd_obj = rc_cmd.data.get<cmd::CmdObj>();

    for (const auto& addressed : cmd_obj.modules) {
      dataobj_t startpars = addressed.data;
      auto rc_startpars = startpars.get<rcif::cmd::StartParams>();
      m_runinfo.runno = rc_startpars.run;
      break;
    }

    m_run_start_time = std::chrono::steady_clock::now();
    ;
    m_runinfo.running = true;
    m_runinfo.runtime = 0;
  } else if (cmdname == "stop") {
    m_runinfo.running = false;
    m_runinfo.runno = 0;
    m_runinfo.runtime = 0;
  }

  try {
    m_mod_mgr.execute(cmd_data);
    m_busy.store(false);
    if (rc_cmd.exit_state != "ANY")
      set_state(rc_cmd.exit_state);
  } catch (ers::Issue& ex) {
    m_busy.store(false);
    m_error.store(true);
    throw;
  }
}

void
Application::gather_stats(opmonlib::InfoCollector& ci, int level)
{
  // TODO:Fill application info and add it to ci
  appinfo::Info ai;
  // ai.partition_name = m_partition;
  // ai.app_name = get_name();
  ai.state = get_state();
  ai.busy = m_busy.load();
  ai.error = m_error.load();

  opmonlib::InfoCollector tmp_ci;

  tmp_ci.add(ai);

  if (ai.state == "RUNNING") {
    auto now = std::chrono::steady_clock::now();
    m_runinfo.runtime = std::chrono::duration_cast<std::chrono::seconds>(now - m_run_start_time).count();
  }
  tmp_ci.add(m_runinfo);

  if (level == 0) {
    // give only generic application info
  } else if (ai.state == "CONFIGURED" || ai.state == "RUNNING") {
    try {
      m_mod_mgr.gather_stats(tmp_ci, level);
    } catch (ers::Issue& ex) {
      ers::error(ex);
    }
  }
  ci.add(m_fully_qualified_name, tmp_ci);
}

bool
Application::is_cmd_valid(const dataobj_t& cmd_data)
{
  if (m_busy.load() || m_error.load())
    return false;

  std::string state = get_state();
  std::string entry_state = cmd_data.get<rcif::cmd::RCCommand>().entry_state;
  if (entry_state == "ANY" || state == entry_state)
    return true;
  
  return false;
}

} // namespace appfwk
} // namespace dunedaq
