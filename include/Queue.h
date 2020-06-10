//
// Created by anton on 06.12.2019.
//

#ifndef TECHPROJECT_QUEUE_H
#define TECHPROJECT_QUEUE_H

#include <boost/beast.hpp>
#include <vector>
#include "Work.h"

namespace http = boost::beast::http;

template <class HttpSession>
class Queue {
  enum {
    // Максимальное количество ответов, которые мы будем ставить в очередь
    limit = 8
  };

  HttpSession& self;
  std::vector<std::unique_ptr<Work>> items;

 public:
  explicit Queue(HttpSession& _self);
  bool is_full() const;
  bool onWrite();

  // Вызывается обработчиком HTTP для отправки ответа.
  template <bool isRequest, class Body, class Fields>
  void operator()(http::message<isRequest, Body, Fields>&& msg_);
};

/////////////////////////////////////////////////////////////////////

namespace beast = boost::beast;

template <class HttpSession>
Queue<HttpSession>::Queue(HttpSession& _self) : self(_self) {
  static_assert(limit > 0, "queue limit must be positive");
  items.reserve(limit);
}

template <class HttpSession>
bool Queue<HttpSession>::onWrite() {
  BOOST_ASSERT(!items.empty());
  auto const was_full = is_full();
  items.erase(items.begin());
  if (!items.empty()) (*items.front())();
  return was_full;
}

template <class HttpSession>
template <bool isRequest, class Body, class Fields>
void Queue<HttpSession>::operator()(
    http::message<isRequest, Body, Fields>&& msg_) {
  // This holds a work item
  struct work_impl : Work {
    HttpSession& self;
    http::message<isRequest, Body, Fields> msg;

    work_impl(HttpSession& self, http::message<isRequest, Body, Fields>&& _msg)
        : self(self), msg(std::move(_msg)) {}

    void operator()() {
      http::async_write(
          self.getStream(), msg,
          beast::bind_front_handler(&HttpSession::onWrite,
                                    self.shared_from_this(), msg.need_eof()));
    }
  };

  // Allocate and store the work
  items.push_back(boost::make_unique<work_impl>(self, std::move(msg_)));

  // If there was no previous work, start this one
  if (items.size() == 1) (*items.front())();
}

template <class HttpSession>
bool Queue<HttpSession>::is_full() const {
  return items.size() >= limit;
}

#endif  // TECHPROJECT_QUEUE_H
