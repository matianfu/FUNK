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

/* return 0 for BUSY */
typedef int (*RUN)(struct continuation * c);

struct continuation
{
  RUN run;
  void* entry;
  struct continuation * sub;
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

struct func_b
{
  struct continuation c;
  int m_a;
  int m_b;
  int m_c;
  int* m_ret;
};

#define CONCAT01(a, b)        a##b
#define CONCAT02(a, b)        CONCAT01(a, b)
#define ENTRY(c)              if (c->entry) goto *c->entry;
#define FUNK(c, exp)          CONCAT02(ENTRY, __LINE__):                      \
                              do {                                            \
                                if (c->sub) {                                 \
                                  if (0 == c->sub->run(c->sub))               \
                                  {                                           \
                                    c->entry = && CONCAT02(ENTRY, __LINE__);  \
                                    return 0;                                 \
                                  }                                           \
                                  else {                                      \
                                    c->sub = 0;                               \
                                  }                                           \
                                }                                             \
                                else                                          \
                                {                                             \
                                  c->sub = (exp);                             \
                                  if (c->sub)                                 \
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
  if (c->sub) {
    if (0 == c->sub->run(c->sub))
    {
      c->entry = && entry1;
      return 0;
    }
    else {
      c->sub = 0;
    }
  }
  else
  {
    c->sub = func_a(f->m_a, f->m_b, f->m_ret);
    if (c->sub)
    {
      goto entry1;
    }
  }

  /* don't panic, this is the second call, suprisingly simple, right? */
  FUNK(c, func_a(*(f->m_ret), f->m_c, f->m_ret));

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
  setbuf(stdout, NULL);

  int ret;
  struct continuation * c = func_b(1, 2, 3, &ret);
  if (c)
  {
    while (0 == c->run(c));
  }

  printf("1 + 2 + 3 = %d\n", ret);
  return 0;
}
