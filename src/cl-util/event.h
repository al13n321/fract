#pragma once

#include <vector>
#include "context.h"

namespace fract { namespace CL {

class Event {
 public:
  Event();
  Event(cl_event event);
  Event(const Event &e);
  ~Event();

  Event& operator=(Event e);

  void reset(cl_event e);

  // Can be null.
  cl_event event() const;

  void WaitFor() const;

 private:
  cl_event event_;
};

class EventList {
 public:
  EventList();
  EventList(const std::vector<cl_event> &events);
  EventList(const EventList &l);
  ~EventList();

  EventList& operator=(EventList e);

  void Clear();
  void Add(const Event &e);

  int size() const;

  void WaitFor() const;

  const cl_event* events() const;
 private:
  std::vector<cl_event> events_;
};
    
}}
