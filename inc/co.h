#ifndef _FUNK_CO_H_
#define _FUNK_CO_H_

#include "common.h"

/**
 * This spaghetti code is intended for do FUNK call in just one line.
 * Leave the optimization to GCC compiler.
 */
#define DoCall(co, name, ...)   CONCAT02(ENTRY, __LINE__) :                 \
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

/* This is the base class for blocking function.
 * This is the new version.
 * Co for Coroutine.
 */
class Co
{
public:

  /**
   * RESET:   not initialized
   * RUNNING: can be called
   * DEAD:    finished, either initialization fail, or function returned,
   *          success or failed.
   */
  enum State
  {
    RESET = 0, RUNNING,
  };

  void* EP;
  State state;

  Co() :
      EP(0), state(RESET)
  {
  }

  /**
   * return 0 for "BUSY"
   */
  Co* Yield()
  {
    state = RUNNING;
    return 0;
  }

  /**
   * return this for exit or init failure.
   */
  Co* Exit()
  {
    state = RESET;
    return this;
  }
};

/*
 * This is the old version of spaghetti call macro
 */
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
 * This is the old version implementation of base class.
 *
 * ABANDONED. Do NOT use it!
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

#endif
