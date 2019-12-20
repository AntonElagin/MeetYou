//
// Created by anton on 06.12.2019.
//

#ifndef TECHPROJECT_WORK_H
#define TECHPROJECT_WORK_H

struct Work {
  virtual ~Work() = default;

  virtual void operator()() = 0;
};

#endif  // TECHPROJECT_WORK_H
