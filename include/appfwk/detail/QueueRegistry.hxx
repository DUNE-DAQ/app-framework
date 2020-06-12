#include "appfwk/StdDeQueue.hpp"

#include <cxxabi.h>

// Declarations
namespace dunedaq::appfwk {

QueueConfig::queue_kind QueueConfig::stoqk(const std::string &name) {
  if (name == "StdDeQueue" || name == "std_deque")
    return queue_kind::kStdDeQueue;
  else
    throw QueueKindUnknown(ERS_HERE, name);
}

template <typename T>
std::shared_ptr<Queue<T>> QueueRegistry::get_queue(const std::string &name) {

  auto itQ = queue_registry_.find(name);
  if (itQ != queue_registry_.end()) {
    auto queuePtr = std::dynamic_pointer_cast<Queue<T>>(itQ->second.instance);

    if (!queuePtr) {
      int status;
      std::string realname_target =
          abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
      std::string realname_source =
          abi::__cxa_demangle(itQ->second.type->name(), 0, 0, &status);

      throw QueueTypeMismatch(ERS_HERE, name, realname_source, realname_target);
    }

    return queuePtr;
  }

  auto itP = this->queue_configmap_.find(name);
  if (itP != queue_configmap_.end()) {
    QueueEntry entry = {&typeid(T), create_queue<T>(name, itP->second)};
    queue_registry_[name] = entry;
    return std::dynamic_pointer_cast<Queue<T>>(entry.instance);

  } else {
    throw std::runtime_error("Queue not found");
  }
}

template <typename T>
std::shared_ptr<NamedObject>
QueueRegistry::create_queue(std::string name, const QueueConfig &config) {

  std::shared_ptr<NamedObject> queue;
  switch (config.kind) {
  case QueueConfig::kStdDeQueue:
    queue = std::make_shared<StdDeQueue<T>>(name);
    std::dynamic_pointer_cast<StdDeQueue<T>>(queue)->SetSize(config.size);
    break;
  default:
    throw std::runtime_error("Unknown queue kind");
  }

  return queue;
}

} // namespace dunedaq::appfwk
