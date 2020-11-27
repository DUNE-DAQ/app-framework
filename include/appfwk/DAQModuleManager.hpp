#ifndef APPFWK_INCLUDE_APPFWK_DAQMODULEMANAGER_HPP_
#define APPFWK_INCLUDE_APPFWK_DAQMODULEMANAGER_HPP_

#include "appfwk/cmd/Structs.hpp"
#include "cmdlib/CommandedObject.hpp"
#include <nlohmann/json.hpp>
#include <ers/Issue.h>

#include <map>
#include <string>
#include <vector>
#include <memory>

namespace dunedaq {

/**
 * @brief A generic DAQModule ERS Issue
 */
ERS_DECLARE_ISSUE(appfwk,                 ///< Namespace
                  DAQModuleManagerNotInitialized,  ///< Issue class name
                  "Command " << cmdid << " received before initialization", ///< Message
                  ((std::string)cmdid)     ///< Message parameters
)

ERS_DECLARE_ISSUE(appfwk,                 ///< Namespace
                  CommandDispatchingFailed,  ///< Issue class name
                  "Command " << cmdid << " was not executed correctly by: " << modules, ///< Message
                  ((std::string)cmdid)     ///< Message parameters
                  ((std::string)modules)     ///< Message parameters
)


namespace appfwk {

class DAQModule;

class DAQModuleManager : public cmdlib::CommandedObject
{
public:

    using dataobj_t = nlohmann::json;

    DAQModuleManager();

    bool initialized() const { return initialized_; }

    // Execute a properly structured command
    void execute( const dataobj_t& cmd_data );

protected:
    typedef std::map<std::string, std::shared_ptr<DAQModule>> DAQModuleMap; ///< DAQModules indexed by name

    void initialize( const dataobj_t& data );
    void init_queues( const cmd::QueueSpecs& qspecs );
    void init_modules( const cmd::ModSpecs& mspecs );

    void dispatch(cmd::CmdId id, const dataobj_t& data );

private:

    bool initialized_;

    DAQModuleMap modulemap_;
};

} // namespace appfwk
} // namespace dunedaq
#endif // APPFWK_INCLUDE_APPFWK_DAQMODULEMANAGER_HPP_
