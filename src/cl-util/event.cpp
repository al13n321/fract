#include "event.h"
#include <cassert>
#include <vector>
#include "util/exceptions.h"
using namespace std;

namespace fract { namespace CL {

Event::Event(): event_(nullptr) {}

Event::Event(cl_event e) {
  event_ = e;
  if (e)
    if (clRetainEvent(e) != CL_SUCCESS)
      throw CLException("failed to retain event");
}

Event::Event(const Event &e) {
  Event tmp(e.event_);
	std::swap(event_, tmp.event_);
}

Event::~Event() {
  if (event_)
    if (clReleaseEvent(event_) != CL_SUCCESS)
      throw CLException("failed to release event");
}

Event& Event::operator=(Event e) {
	std::swap(event_, e.event_);
  return *this;
}

void Event::reset(cl_event e) {
	Event tmp(e);
	std::swap(event_, tmp.event_);
}

cl_event Event::event() const {
  return event_;
}

void Event::WaitFor() const {
  if (clWaitForEvents(1, &event_) != CL_SUCCESS)
    throw CLException("failed to wait for event");
}


EventList::EventList() {}

EventList::EventList(const EventList &l) {
  for (cl_event e: l.events_)
    Add(e);
}

EventList::~EventList() {
	bool fail = false;
  for (int i = 0; i < events_.size(); ++i)
    if (clReleaseEvent(events_[i]) != CL_SUCCESS)
    	fail = true;
  if (fail)
    throw CLException("failed to release event");
}

EventList& EventList::operator=(EventList l) {
	std::swap(events_, l.events_);
  return *this;
}

void EventList::Clear() {
	*this = EventList();
}

void EventList::Add(const Event &e) {
  if (clRetainEvent(e.event()) != CL_SUCCESS)
    throw CLException("failed to retain event");
  events_.push_back(e.event());
}

int EventList::size() const {
  return events_.size();
}

void EventList::WaitFor() const {
  if (clWaitForEvents(events_.size(), &events_[0]) != CL_SUCCESS)
    throw CLException("failed to wait for events");
}

const cl_event* EventList::events() const {
  return &events_[0];
}
    
}}
