/*
 * main.c
 *
 *  Created on: Apr 10, 2015
 *      Author: ma (matianfu@gmail.com)
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct continuation;

typedef struct
{
  void* entry;
  void* data;
} entry_t;

/* return 0 for BUSY */
typedef int (*RUN)(struct continuation * c);

struct continuation
{
  RUN run;
  void* entry;
};

struct continuation * init_funk(struct continuation * c, RUN run)
{
  c->run = run;
  c->entry = 0;
  return c;
}

struct func_a
{
  struct continuation c;
  int m_a;
  int m_b;
  struct timespec m_start;
  int * m_ret;
};

static int func_a_run(struct continuation * c)
{
  struct func_a * f = (struct func_a *)c;
  struct timespec now;

  printf("%s\n", __func__);

  clock_gettime(CLOCK_REALTIME, &now);

  if ((now.tv_sec - f->m_start.tv_sec) > 1)
  {
    *(f->m_ret) = f->m_a + f->m_b;
    return 1;
  }
  return 0;
}

struct continuation* func_a(int a, int b, int * ret)
{
  struct func_a * f = malloc(sizeof(struct func_a));

  f->m_a = a;
  f->m_b = b;
  f->m_ret = ret;
  clock_gettime(CLOCK_REALTIME_COARSE, &f->m_start);

  return init_funk(&f->c, func_a_run);
}

entry_t* func_c(entry_t* ep, int a, int b, int * ret)
{
  typedef struct {
    struct timespec start;
  } data_t;

  data_t * f;
  struct timespec now;

  if (ep->entry == 0)
  {
    f = malloc(sizeof(data_t));
    if (f == 0)
    {
      ep->entry = (void*)-1;
      return ep;
    }
    ep->entry = && RUNNING;
    ep->data = f;
    clock_gettime(CLOCK_REALTIME_COARSE, &f->start);
    return NULL;
  }
RUNNING:
  f = (data_t *)ep->data;
  clock_gettime(CLOCK_REALTIME, &now);

  if ((now.tv_sec - f->start.tv_sec) > 1)
  {
    *ret = a + b;
    free(f);
    ep->data = 0;
    ep->entry = 0;
    return ep;
  }
  return NULL;
}

entry_t* func_d(entry_t * ep, int a, int b, int c, int * ret)
{
  typedef struct
  {
    int sum;
    entry_t sub;
  } data_t;

  data_t * f;

  if (ep->entry == 0)
  {
    f = malloc(sizeof(data_t));
    if (f == 0)
    {
      ep->entry = (void*)-1;
      return ep;
    }
    ep->entry = && RUNNING;
    ep->data = f;

    f->sum = 0;
    f->sub.data = 0;
    f->sub.entry = 0;
    return NULL;
  }

  f = (data_t *)ep->data;
  goto *(ep->entry);

  RUNNING:

  ENTRY1:

  if (0 == func_c(&f->sub, a, b, &f->sum))
  {
    ep->entry = && ENTRY1;
    return 0;
  }

  f->sub.data = 0;
  f->sub.entry = 0;

  ENTRY2:
  if (0 == func_c(&f->sub, f->sum, c, ret))
  {
    ep->entry = && ENTRY2;
    return 0;
  }

  return ep;
}

struct func_b
{
  struct continuation c;
  struct continuation * sub;
  int m_a;
  int m_b;
  int m_c;
  int* m_ret;
};

#define CONCAT01(a, b)        a##b
#define CONCAT02(a, b)        CONCAT01(a, b)
#define ENTRY(c)              if (c->entry) goto *c->entry;
#define FUNK(sub, exp)        CONCAT02(ENTRY, __LINE__):                      \
                              do {                                            \
                                if (sub) {                                    \
                                  if (0 == sub->run(sub))                     \
                                  {                                           \
                                    c->entry = && CONCAT02(ENTRY, __LINE__);  \
                                    return 0;                                 \
                                  }                                           \
                                  else {                                      \
                                    sub = 0;                                  \
                                  }                                           \
                                }                                             \
                                else                                          \
                                {                                             \
                                  sub = (exp);                                \
                                  if (sub)                                    \
                                  {                                           \
                                    goto CONCAT02(ENTRY, __LINE__);           \
                                  }                                           \
                                }                                             \
                              } while(0)

static int func_b_run(struct continuation * c)
{
  struct func_b * f = (struct func_b *)c;

  ENTRY(c);

  /* the first call */
entry1:
  if (f->sub) {
    if (0 == f->sub->run(f->sub))
    {
      c->entry = && entry1;
      return 0;
    }
    else {
      f->sub = 0;
    }
  }
  else
  {
    f->sub = func_a(f->m_a, f->m_b, f->m_ret);
    if (f->sub)
    {
      goto entry1;
    }
  }

  /* don't panic, this is the second call, suprisingly simple, right? */
  FUNK(f->sub, func_a(*(f->m_ret), f->m_c, f->m_ret));

  return 1;
}

struct continuation* func_b(int a, int b, int c, int * ret)
{
  struct func_b * f = malloc(sizeof(struct func_b));
  f->c.run = func_b_run;
  f->c.entry = 0;
  f->m_a = a;
  f->m_b = b;
  f->m_c = c;
  f->m_ret = ret;

  return &f->c;
}

int main(void)
{
  int ret;

  setbuf(stdout, NULL);

  entry_t ent = {0, 0};

  while (func_c(&ent, 1, 2, &ret) == 0);

  printf("func_c returns %d\n", ret);

  ent.data = 0;
  ent.entry = 0;

  while (func_d(&ent, 1, 2, 3, &ret) == 0);

  printf("func_d returns %d\n", ret);



//  struct continuation * c = func_b(1, 2, 3, &ret);
//  if (c)
//  {
//    while (0 == c->run(c));
//  }
//
//  printf("1 + 2 + 3 = %d\n", ret);
//  return 0;
}
