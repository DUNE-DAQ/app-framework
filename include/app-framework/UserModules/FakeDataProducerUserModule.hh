/**
 * @file The FakeDataProducerUserModule class interface
 *
 * FakeDataProducerUserModule is a simple UserModule implementation that simply
 * logs the fact that it received a command from DAQProcess.
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef APP_FRAMEWORK_INCLUDE_APP_FRAMEWORK_USERMODULES_FAKEDATAPRODUCERUSERMODULE_HH_
#define APP_FRAMEWORK_INCLUDE_APP_FRAMEWORK_USERMODULES_FAKEDATAPRODUCERUSERMODULE_HH_

#include "app-framework-base/Buffers/Buffer.hh"
#include "app-framework-base/UserModules/UserModule.hh"
#include "app-framework-base/UserModules/UserModuleThreadHelper.hh"

#include <future>
#include <memory>
#include <string>
#include <vector>

namespace appframework {
/**
 * @brief FakeDataProducerUserModule creates vectors of ints and sends them
 * downstream
 */
class FakeDataProducerUserModule : public UserModule {
public:
  explicit FakeDataProducerUserModule(
      std::shared_ptr<BufferInput<std::vector<int>>> outputBuffer);

  std::future<std::string> execute_command(std::string cmd) override;

  FakeDataProducerUserModule(const FakeDataProducerUserModule &) = delete;
  FakeDataProducerUserModule &
  operator=(const FakeDataProducerUserModule &) = delete;
  FakeDataProducerUserModule(FakeDataProducerUserModule &&) = delete;
  FakeDataProducerUserModule &operator=(FakeDataProducerUserModule &&) = delete;

private:
  // Commands
  std::string do_configure();
  std::string do_start();
  std::string do_stop();

  // Threading
  UserModuleThreadHelper thread_;
  void do_work();

  // Configuration
  std::shared_ptr<BufferInput<std::vector<int>>> outputBuffer_;
  std::chrono::milliseconds bufferTimeout_;
  size_t nIntsPerVector_;
  int starting_int_;
  int ending_int_;

  size_t wait_between_sends_ms_;
};
} // namespace appframework

#endif // APP_FRAMEWORK_INCLUDE_APP_FRAMEWORK_USERMODULES_FAKEDATAPRODUCERUSERMODULE_HH_
