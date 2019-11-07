//
// Created by anton on 07.11.2019.
//

#include "queue.h"


void work_impl::operator()()
{
  http::async_write(
      self_.stream_,
      msg_,
      beast::bind_front_handler(
          &http_session::on_write,
          self_.shared_from_this(),
          msg_.need_eof()));
}


bool queue::on_write() {
  BOOST_ASSERT(! items_.empty());
  auto const was_full = is_full();
  items_.erase(items_.begin());
  if(! items_.empty())
    (*items_.front())();
  return was_full;
}


template<bool isRequest, class Body, class Fields>
void queue::operator()(http::message<isRequest, Body, Fields>&& msg)
{
  // This holds a work item

  // Allocate and store the work
  items_.push_back(boost::make_unique<work_impl>(self_, std::move(msg)));

  // If there was no previous work, start this one
  if(items_.size() == 1)
    (*items_.front())();
}

bool queue::is_full() const
{
  return items_.size() >= limit;
}

queue::queue(http_session& self)
: self_(self)
    {
        static_assert(limit > 0, "queue limit must be positive");
        items_.reserve(limit);
    }