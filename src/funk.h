#ifndef __FUNK_C_H_
#define __FUNK_C_H_

#include <string.h>

#define CONCAT01(a, b)        a##b
#define CONCAT02(a, b)        CONCAT01(a, b)

#define EXIT                            \
  do {                                  \
    if (this)                           \
    {                                   \
      free(this);                       \
    }                                   \
    return 0;                           \
  } while(0)

#define YIELD                           \
  do {                                  \
    return (Continuation*)this;         \
  } while (0)


#define CALL_FUNK(name,...)             \
  CONCAT02(ENTRY, __LINE__):            \
  if ((this->co.sub =                   \
    name(this->co.sub, __VA_ARGS__)))   \
  {                                     \
    this->co.ep =                       \
    && CONCAT02(ENTRY, __LINE__);       \
    return (Continuation*)this;         \
  }

#define VAR_BEGIN                       \
  struct _func_data                     \
  {                                     \
    Continuation co;

#define VAR_END                         \
  }* this = (struct _func_data *) co;   \
                                        \
  if (this)                             \
  {                                     \
    goto *this->co.ep;                  \
  }                                     \
  if ((this = malloc(sizeof(*this))))   \
  {                                     \
    memset(this, 0, sizeof(*this));     \
  }


#define INIT_END                        \
  __INIT_END__:                         \
  if (this->co.ep == 0)                 \
  {                                     \
    this->co.ep = && __INIT_END__;      \
  }

typedef struct Continuation
{
  void* ep;                   /** entry point **/
  struct Continuation* sub;
} Continuation;

/*
 * The following commented code
 * are examples without FUNK macro
 */

#if 0
Continuation* _add(Continuation* co, int * ret, int a, int b)
{
  struct _func_data
  {
    Continuation co;


    struct timespec start;
    struct timespec now;


  }* this = (struct _func_data *) co;

  if (this)
  {
    goto *this->co.ep;
  }
  if ((this = malloc(sizeof(*this))))
  {
    memset(this, 0, sizeof(*this));
  }

  if (this == 0)
  {
    *ret = -1;
    EXIT;
  }

  // put first time init code here
  clock_gettime(CLOCK_REALTIME_COARSE, &this->start);

  __INIT_END__: if (this->co.ep == 0)
  {
    this->co.ep = && __INIT_END__;
  }

  clock_gettime(CLOCK_REALTIME, &this->now);

  if ((this->now.tv_sec - this->start.tv_sec) > 1)
  {
    *ret = a + b;
    // put clean up code here if needed

    free(this);
    return 0;
  }

  return (Continuation*)this;
}

Continuation * _sum(Continuation * co, int a, int b, int c, int * ret)
{
  struct _func_data
  {
    Continuation co;

    int sum;

  }* this = (struct _func_data *) co;

  if (this)
  {
    goto *this->co.ep;
  }
  if ((this = malloc(sizeof(*this))))
  {
    memset(this, 0, sizeof(*this));
  }

  if (this == 0)
  {
    *ret = -1;
    EXIT;
  }

  __INIT_END__: if (this->co.ep == 0)
  {
    this->co.ep = && __INIT_END__;
  }

  ENTRY1: if ((this->co.sub = add(this->co.sub, a, b, &this->sum)))
  {
    this->co.ep = &&ENTRY1;
    return (Continuation*) this;
  }

  ENTRY2: if ((this->co.sub = add(this->co.sub, this->sum, c, ret)))
  {
    this->co.ep = &&ENTRY2;
    return (Continuation*) this;
  }

  free(this);
  return 0;
}

#endif

#endif


