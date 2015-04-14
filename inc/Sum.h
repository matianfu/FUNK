#include "co.h"
#include "Adder.h"

#define DDCall(co, name, ...)   CONCAT02(ENTRY, __LINE__) :                 \
                                do                                          \
                                {                                           \
                                  if (co)                                   \
                                  {                                         \
                                    if (((name*)co)->Do(__VA_ARGS__))       \
                                    {                                       \
                                      EP =                                  \
                                      && CONCAT02(ENTRY, __LINE__);         \
                                      return this;                          \
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



/*
 * Sum three numbers, call adder twice
 */
class Sum: public Co
{
public:

  Adder* adder;
  int sum;

  Co* Do(int a, int b, int c, int * ret)
  {
    if (EP) goto *EP;

    sum = 0;


    ENTRY1:
    if (adder->Do(a, b, &sum))
    {
      EP = && ENTRY1;
      return this;
    }

    ENTRY2:
    if (((Adder*)co)->Do(sum, c, ret))
    {
      EP = && ENTRY2;
      return this;
    }

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


