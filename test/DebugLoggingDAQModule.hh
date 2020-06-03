/**
 * @file DebugLoggingDAQModule.hh
 *
 * DebugLoggingDAQModule is a simple DAQModule implementation that simply logs
 * the fact that it received a command from DAQProcess.
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef APP_FRAMEWORK_TEST_DEBUGLOGGINGDAQMODULE_HH_
#define APP_FRAMEWORK_TEST_DEBUGLOGGINGDAQMODULE_HH_

#include "app-framework/DAQModule.hh"

#include <string>
#include <vector>

namespace appframework {
/**
 * @brief DebugLoggingDAQModule logs that it has received a command from
 * DAQProcess
 */
class DebugLoggingDAQModule : public DAQModule
{
public:
  /**
   * @brief DebugLoggingDAQModule Constructor
   * @param name Instance name for this DebugLoggingDAQModule
  */
  explicit DebugLoggingDAQModule(std::string name)
    : DAQModule(name)
  {}

  /**
   * @brief Execute a command from DAQProcess
   * @param cmd Command from DAQProcess
   * @param args Arguments for the command from DAQProcess
   */
  void execute_command(const std::string& cmd,
                       const std::vector<std::string>& args = {}) override;
};
} // namespace appframework

#endif // APP_FRAMEWORK_TEST_DEBUGLOGGINGDAQMODULE_HH_
