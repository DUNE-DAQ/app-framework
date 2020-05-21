/**
 * @file DebugLoggingUserModule class implementation
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "app-framework/UserModules/DebugLoggingUserModule.hh"

#include "app-framework-base/Services/Logger.hh" // get "logging" streamer macros
#include "app-framework-base/UserModules/UserModule.hh"

// #include "TRACE/trace.h"
//#include <ers/ers.h>

#include <iostream>

namespace appframework {
std::future<std::string>
DebugLoggingUserModule::execute_command(std::string cmd) {
	ERS_LOG() << "Executing command: " << cmd;
  return std::async([]() { return std::string("Success"); });
}
} // namespace appframework
