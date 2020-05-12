/**
 * @file The FanOutUserModule class interface
 *
 * FanOutUserModule is a simple UserModule implementation that simply logs the
 * fact that it received a command from DAQProcess.
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef APP_FRAMEWORK_USERMODULES_FANOUTUSERMODULE_HH
#define APP_FRAMEWORK_USERMODULES_FANOUTUSERMODULE_HH

#include "app-framework-base/Buffers/Buffer.hh"
#include "app-framework-base/UserModules/UserModule.hh"
#include "app-framework-base/UserModules/UserModuleThreadHelper.hh"

#include <future>
#include <list>
#include <memory>
#include <string>
#include <type_traits>
#include <unistd.h>

namespace appframework {
/**
 * @brief FanOutUserModule sends data to multiple Buffers
 */
template<typename DATA_TYPE>
class FanOutUserModule : public UserModule
{
public:
  FanOutUserModule(
    std::shared_ptr<BufferOutput<DATA_TYPE>> inputBuffer,
    std::initializer_list<std::shared_ptr<BufferInput<DATA_TYPE>>>
      outputBuffers);

  /**
   * @brief Logs the reception of the command
   * @param cmd Command from DAQProcess
   */
  std::future<std::string> execute_command(std::string cmd) override;

  enum class FanOutMode
  {
    NotConfigured,
    Broadcast,
    RoundRobin,
    FirstAvailable
  };

private:
  // Commands
  std::string do_configure();
  std::string do_start();
  std::string do_stop();

  // Threading
  UserModuleThreadHelper thread_;
  void do_work();

  // Configuration
  FanOutMode mode_;
  const std::chrono::milliseconds bufferTimeout_;  
  std::shared_ptr<BufferOutput<DATA_TYPE>> inputBuffer_;
  std::list<std::shared_ptr<BufferInput<DATA_TYPE>>> outputBuffers_;
  size_t wait_interval_us_;

  // Type traits handling
  template<typename... Dummy, typename U = DATA_TYPE>
  typename std::enable_if<!std::is_copy_constructible<U>::value>::type
  do_broadcast(DATA_TYPE&)
  {
    throw std::runtime_error(
      "Broadcast mode cannot be used for non-copy-constructible types!");
  }
  template<typename... Dummy, typename U = DATA_TYPE>
  typename std::enable_if<std::is_copy_constructible<U>::value>::type
  do_broadcast(DATA_TYPE& data)
  {
    for (auto& o : outputBuffers_) {
      o->push(DATA_TYPE(data), bufferTimeout_);
    }
  }
};
} // namespace appframework

#include "impl/FanOutUserModule.icc"

#endif // APP_FRAMEWORK_USERMODULES_FANOUTUSERMODULE_HH
