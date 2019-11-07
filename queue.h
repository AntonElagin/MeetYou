//
// Created by anton on 07.11.2019.
//

#ifndef TECHPROJECT_QUEUE_H
#define TECHPROJECT_QUEUE_H

#include "http_session.h"

// The type-erased, saved work item
struct work
{
  virtual ~work() = default;
  virtual void operator()() = 0;
};



struct work_impl : work
{
  http_session& self_;
  http::message<isRequest, Body, Fields> msg_;

  work_impl(http_session& self,http::message<isRequest, Body, Fields>&& msg)
      : self_(self), msg_(std::move(msg))
  {
  }

  void operator()();
};



// This queue is used for HTTP pipelining.
class queue
{
  enum
  {
    // Maximum number of responses we will queue
        limit = 8
  };

  http_session& self_;
  std::vector<std::unique_ptr<work>> items_;

public:
  explicit
  queue(http_session& self);
  // Returns `true` if we have reached the queue limit
  bool is_full() const;

  // Called when a message finishes sending
  // Returns `true` if the caller should initiate a read
  bool on_write();

  // Called by the HTTP handler to send a response.
  template<bool isRequest, class Body, class Fields> void
  operator()(http::message<isRequest, Body, Fields>&& msg):
};



#endif //TECHPROJECT_QUEUE_H
