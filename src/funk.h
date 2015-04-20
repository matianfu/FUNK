#ifndef FUNK_C_H
#define FUNK_C_H

#include <stdint.h>
#include <string.h>

/** in dynamic mode
 *
 *  FUNK don't yield right after malloc.
 *  FUNK is destroyed after exit
 *
 *  FUNK ep:
 *  1. 0 is used for destroy (caller set this value)
 *  2. alloced is set after malloc but won't yield at this state
 *  3. resumed is set after yield resumed but won't yield at this state
 *  4. other values are valid entry point values;
 *
 */

#define CONCAT01(a, b)                      a##b
#define CONCAT02(a, b)                      CONCAT01(a, b)

/* externally set */
#define FUNK_SIG_KILL                       0
#define FUNK_SIG_STOP                       1 // not used yet

#define FUNK_ALLOCED                        2
#define FUNK_STOPPED                        3 // not used yet
#define FUNK_RESUMED                        4

#define KILL_SIGNALLED                      (this->co.ep == (void*)FUNK_SIG_KILL)

#define EXIT()                              \
  do                                        \
  {                                         \
    if (this) free(this);                   \
    return 0;                               \
  } while (0)

#define YIELD()                             \
  CONCAT02(ENTRY, __LINE__):                \
  do {                                      \
    if (this->co.ep ==                      \
        && CONCAT02(ENTRY, __LINE__))       \
    {                                       \
      this->co.ep =                         \
        (void*)(FUNK_RESUMED);              \
    }                                       \
    else                                    \
    {                                       \
      this->co.ep =                         \
        && CONCAT02(ENTRY, __LINE__);       \
      return (Continuation*)this;           \
    }                                       \
  } while (0)

#define CALL_FUNK(co, name,...)             \
    (co = name(co, __VA_ARGS__))

#define KILL_FUNC(co, name,...)             \
    do { if (co) {                          \
      co->ep = FUNK_SIG_KILL;               \
      co = name(co, __VA_ARGS__);           \
    }} while(0)

#define VAR_BEGIN                           \
  struct _func_data {                       \
    Continuation co;

/*
 * After VAR_END code, there are 3 possibilities:
 *
 * 1. this is set, ep is SIGNAL.
 * 2. this is set, ep is FUNK_ALLOCED, first time malloc succeed
 * 3. this is not set, first time mem alloc fail
 */
#define VAR_END                             \
  }* this = (struct _func_data *) co;       \
  if (this)                                 \
  {                                         \
    if ((uintptr_t) (this->co.ep) > FUNK_RESUMED) \
      goto *this->co.ep;                    \
  }                                         \
  else if ((this = malloc(sizeof(*this))))  \
  {                                         \
    memset(this, 0, sizeof(*this));         \
    this->co.ep = (void*) FUNK_ALLOCED;     \
  }

typedef struct Continuation {
  void* ep;                   /** entry point **/
} Continuation;

#endif


