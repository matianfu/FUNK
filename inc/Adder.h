#ifndef __ADDER_H_
#define __ADDER_H_

#include <iostream>
#include <time.h>
#include "co.h"

/**
 * new version example of adder
 */
class Adder: public Co
{

  struct timespec start;
  struct timespec now;

public:

  Co* Do(int a, int b, int *ret)
  {
    if (state == RESET)
    {
      clock_gettime(CLOCK_REALTIME_COARSE, &start);
      return Yield();
    }

    clock_gettime(CLOCK_REALTIME_COARSE, &now);

    if ((now.tv_sec - start.tv_sec) > 1)
    {
      *ret = a+b;
      return Exit();
    }

    return Yield();
  }
};

/*
 * sum two integers, block waiting for 1s before return
 */
class AdderOld: public Continuation
{
private:
  int m_a;
  int m_b;
  int *m_ret;
  struct timespec m_start;

public:
  AdderOld(int a, int b, int * ret) :
      m_a(a), m_b(b), m_ret(ret)
  {
    clock_gettime(CLOCK_REALTIME_COARSE, &m_start);
  }

  virtual int Run()
  {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);

    if ((now.tv_sec - m_start.tv_sec) > 1)
    {
      *m_ret = m_a + m_b;
      return 1;
    }
    return 0;
  }
};

#endif

