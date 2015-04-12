//============================================================================
// Name        : main.cpp
// Author      : matianfu@gmail.com
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <time.h>
using namespace std;

/*
 * macro definitions, hiding spaghetti control flow code in one line
 */
#define CONCAT01(s1, s2)    s1##s2
#define CONCAT02(s1, s2)    CONCAT01(s1, s2)

#define FUNK(sub, exp)      CONCAT02(ENTRY, __LINE__) :                 \
                            do {                                        \
                              if (sub) {                                \
                                if (0 == sub->Run())                    \
                                {                                       \
                                    mEntry =                            \
                                    && CONCAT02(ENTRY, __LINE__);       \
                                  return 0;                             \
                                }                                       \
                                else                                    \
                                {                                       \
                                  delete sub;                           \
                                  sub = 0;                              \
                                }                                       \
                              }                                         \
                              else {                                    \
                                sub = (exp);                            \
                                if (sub)                                \
                                {                                       \
                                  goto CONCAT02(ENTRY, __LINE__);       \
                                }                                       \
                              }                                         \
                            } while (0)

/*
 * base class for continuation
 */
class Continuation
{
public:
  Continuation() :
      mEntry(0) {}
  virtual int Run() = 0;
  virtual ~Continuation() {}
protected:
  void* mEntry;
};

class Co
{
public:

  enum State
  {
    RESET = 0, RUNNING, DEAD,
  };

  void* EP;
  State state;

  Co() :
      EP(0), state(RESET)
  {
  }

  Co* Yield()
  {
    state = RUNNING;
    return NULL;
  }

  Co* Exit()
  {
    state = DEAD;
    return this;
  }
};


/*
 * sum two integers, block waiting for 1s before return
 */
class FuncA: public Continuation
{
private:
  int m_a;
  int m_b;
  int *m_ret;
  struct timespec m_start;

public:
  FuncA(int a, int b, int * ret) :
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

class FuncA2: public Co
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

#define CoDo(co, name, ...)     CONCAT02(ENTRY, __LINE__) :                 \
                                do                                          \
                                {                                           \
                                  if (co)                                   \
                                  {                                         \
                                    if (0 == ((name*)co)->Do(__VA_ARGS__))  \
                                    {                                       \
                                      EP =                                  \
                                      && CONCAT02(ENTRY, __LINE__);         \
                                      return Yield();                       \
                                    }                                       \
                                    else                                    \
                                    {                                       \
                                      delete co;                            \
                                      co = 0;                               \
                                    }                                       \
                                  }                                         \
                                  else                                      \
                                  {                                         \
                                    co = (new name());                      \
                                    if (co)                                 \
                                    {                                       \
                                      goto CONCAT02(ENTRY, __LINE__);       \
                                    }                                       \
                                  }                                         \
                                } while (0)


class FuncB2: public Co
{
  Co* co;
  int sum;

public:

  Co* Do(int a, int b, int c, int * ret)
  {
    if (state == RESET)
    {
      co = 0;
      sum = 0;
      return Yield();
    }

    if (EP) goto *EP;

    CoDo(co, FuncA2, a, b, &sum);
    CoDo(co, FuncA2, sum, c, ret);

    return Exit();
  }
};

/*
 * sum three integers, call FuncA twice.
 */
class FuncB: public Continuation
{
private:
  Continuation * mSub;
  int m_a;
  int m_b;
  int m_c;
  int *m_ret;

public:
  FuncB(int a, int b, int c, int * ret) :
    mSub(0), m_a(a), m_b(b), m_c(c), m_ret(ret) {}

  virtual int Run()
  {
    if (mEntry) { goto *mEntry; }
    FUNK(mSub, new FuncA(m_a, m_b, m_ret));
    FUNK(mSub, new FuncA(*m_ret, m_c, m_ret));
    return 1;
  }
};

/* call FUNK in block waiting mode */
void task1()
{
  int ret;

  cout << "Task1 : " << flush;

  FuncB b(1, 2, 3, &ret);
  while (0 == b.Run());

  cout << "1 + 2 + 3 = " << ret << endl;
}

/* call FUNK in busy-return mode */
void task2()
{
  static int state = 0;
  static FuncB * b;

  int ret;

  switch (state)
  {
  case 0:

    cout << "Task2 begins" << endl;
    b = new FuncB(4, 5, 6, &ret);
    if (b)
    {
      state = 1;
    }
    else
    {
      cout << "  Error Code: " << ret << endl;
    }
    break;

  case 1:

    if (b->Run())
    {
      cout << "Task2 ends :  4 + 5 + 6 = " << ret << endl;
      state = 0;
    }

    break;

  case 2:
  default:
    break;
  }
}

void task3()
{
  cout << "Task3" << endl;
  sleep(1);
}

/*
 * main function
 *
 * create FuncB object on stack, block waiting for return.
 */
int main(void)
{
  int ret;

  FuncB2 b2;
  while (0 == b2.Do(1, 2, 3, &ret));

  cout << "b2 returns" << ret << endl;


  task1();
  while (1)
  {
    task2();
    task3();
  }
  return 0;
}


