/*
 * DAQSink.hh
 *
 *  Created on: 19 May 2020
 *      Author: glehmann
 */

#ifndef DAQSOURCE_HH_
#define DAQSOURCE_HH_


#include <string>
#include <memory>
#include <chrono>
#include <app-framework/QueueRegistry.hh>
#include <app-framework-base/NamedQueueI.hh>
#include "ers/Issue.h"

ERS_DECLARE_ISSUE(  appframework,      // namespace
                        DAQSinkConstrutionFailed,       // issue class name
                        "Failed to construct DAQSink \"" << name << "\"",    // no message
                        ((std::string)name ) 
                     )

namespace appframework {

template <typename T>
class DAQSink {
public:
    using value_type = T;
    using duration_type = std::chrono::milliseconds;

	DAQSink(std::string name);
	void push(T element, const duration_type& timeout=duration_type::zero());
	bool can_push();

private:
	std::shared_ptr<NamedQueueI<T>> queue_;
};

template <typename T>
DAQSink<T>::DAQSink(std::string name) {
    try {
	   queue_ = QueueRegistry::get()->get_queue<T>(name);
    } catch ( QueueTypeMismatch& ex ) {
        throw DAQSinkConstrutionFailed(ERS_HERE, name, ex);
    }
}

template <typename T>
void DAQSink<T>::push(T element, const duration_type& timeout) {
	queue_->push(element, timeout);
}

template <typename T>
bool DAQSink<T>::can_push() {
	return queue_->can_push();
}

} // namespace appframework

#endif /* DAQSOURCE_HH_ */
