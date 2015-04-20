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
 *  in static mode
 *
 *  FUNK yield after malloc
 *  FUNK is NOT destroy after exit, but switched to malloced ep.
 *  FUNK ep:
 *  1. 0 is used for destroy (caller set this value)
 *  2. alloced is set after malloc and instantly yield right after.
 *  3. resumed is the same with dynamic
 *  4. other values are valid entry points;
 *
 *  So, in dynamic mode
 *  1. FUNK_YIELDED if func call return NON-NULL
 *  2. FUNK_EXITED if func call return NULL
 *
 *  in static mode
 *  1. FUNK_YIELDED if ep is NOT malloc
 *  2. FUNK_EXITED if ep is malloc
 */

/* used only in static mode */


#ifdef FUNK_STATIC
#define FUNK_ALLOC(co, name,...)            \
  (co = name(co, __VA_ARGS__))
#else
#define FUNK_ALLOC(co, name,...)            \
  co = 0;
#endif

#ifdef FUNK_STATIC
#define FUNK_BEGIN                          if (co == 0) YIELD();
#else
#define FUNK_BEGIN
#endif


#define CONCAT01(a, b)                      a##b
#define CONCAT02(a, b)                      CONCAT01(a, b)

/* externally set */
#define SIG_KILL                            0
#define SIG_STOP                            1

#define FUNK_ALLOCED                        2
#define FUNK_STOPPED                        3
#define FUNK_RESUMED                        4

#define EXIT_STATIC()                       \
  do                                        \
  {                                         \
    memset(this, 0, sizeof(*this));         \
    this->co.ep = (void*)FUNK_ALLOCED;      \
    return 0;                               \
  } while (0)


#define EXIT_DYNAMIC()                      \
  do                                        \
  {                                         \
    if (this) free(this);                   \
    return 0;                               \
  } while (0)

#ifdef FUNK_STATIC
#define EXIT()    EXIT_STATIC()
#else
#define EXIT()    EXIT_DYNAMIC()
#endif

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

#define CALL_FUNK_DYNAMIC(co, name,...)     \
    while((co = name(co, __VA_ARGS__)))     \
      YIELD();                              \

#define CALL_FUNK_STATIC(co, name, ...)     \
    while(name(co, __VA_ARGS__))            \
      YIELD();

#ifdef FUNK_STATIC
#define CALL_FUNK(co, name,...)             CALL_FUNK_STATIC(co, name, __VA_ARGS__)
#else
#define CALL_FUNK(co, name,...)             CALL_FUNK_DYNAMIC(co, name, __VA_ARGS__)
#endif

#define STOP_FUNK(co, name,...)             \
    do { if (co) { co->ep = 0;              \
      co = name(co, __VA_ARGS__);           \
    }} while(0)

#define VAR_BEGIN                           \
  struct _func_data {                       \
    Continuation co;

/*
 * After VAR_END code, there are following possibilities:
 *
 * 1. this is set, ep is 0, being killed.
 * 2. this is set, ep is FUNK_ALLOCED, malloc succeed
 * 3. this is not set
 */
#define VAR_END_DYNAMIC                     \
  }* this = (struct _func_data *) co;       \
  if (!this)                                \
  {                                         \
    this = malloc(sizeof(*this));           \
    if (!this) return 0;                    \
    this->co.ep = (void*)FUNK_ALLOCED;      \
  }                                         \
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

#define VAR_END_STATIC                      \
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


#ifdef FUNK_STATIC
#define VAR_END                             VAR_END_STATIC
#else
#define VAR_END                             VAR_END_DYNAMIC
#endif

typedef struct Continuation Continuation;

typedef Continuation* (*FUNK)(Continuation*);

typedef struct Continuation {
  void* ep;                   /** entry point **/
  FUNK funk;
} Continuation;

#endif


