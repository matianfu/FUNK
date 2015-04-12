#include "co.h"
#include "Adder.h"

/*
 * Sum three numbers, call adder twice
 */
class Sum: public Co
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

    DoCall(co, Adder, a, b, &sum);
    DoCall(co, Adder, sum, c, ret);

    return Exit();
  }
};

/*
 * Old Sum function
 */
class SumOld: public Continuation
{
private:
  Continuation * mSub;
  int m_a;
  int m_b;
  int m_c;
  int *m_ret;

public:
  SumOld(int a, int b, int c, int * ret) :
    mSub(0), m_a(a), m_b(b), m_c(c), m_ret(ret) {}

  virtual int Run()
  {
    if (mEntry) { goto *mEntry; }

    FUNK(mSub, new AdderOld(m_a, m_b, m_ret));
    FUNK(mSub, new AdderOld(*m_ret, m_c, m_ret));

    return 1;
  }
};


