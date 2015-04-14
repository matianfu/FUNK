/*
 * main.c
 *
 *  Created on: Apr 10, 2015
 *      Author: ma (matianfu@gmail.com)
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "funk.h"

Continuation* add(Continuation* co, int a, int b, int * ret)
{
  VAR_BEGIN
  struct timespec start;
  struct timespec now;
  VAR_END

  if (!this) { *ret = 1; EXIT; }
  clock_gettime(CLOCK_REALTIME_COARSE, &this->start);

  INIT_END

  clock_gettime(CLOCK_REALTIME, &this->now);
  if ((this->now.tv_sec - this->start.tv_sec) > 1) {
    *ret = a + b;
    EXIT;   }

  YIELD;
}

Continuation * sum(Continuation * co, int a, int b, int c, int * ret)
{
  VAR_BEGIN
  int sum;
  VAR_END

  if (!this) { *ret = -1; EXIT; }

  INIT_END

  CALL_FUNK(add, a, b, &this->sum);
  CALL_FUNK(add, this->sum, c, ret);

  EXIT;
}

int main(void)
{
  int ret;
  setbuf(stdout, NULL);

  Continuation* co = 0;
  while ((co = sum(co, 1, 2, 4, &ret)))
  {
    sleep(1);
    printf("wake up\n");
  }
  printf("sum 1, 2, 4 is %d\n", ret);

  return 0;
}
