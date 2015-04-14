/*
 *  Created on: Apr 10, 2015
 *      Author: ma (matianfu@gmail.com)
 *      Apache License
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "funk.h"

Continuation* add(Continuation* co,
    int a, int b, int * ret)
{
  struct timespec now;
  VAR_BEGIN
  struct timespec start;
  VAR_END

  if (!this) { *ret = 1; EXIT(); }

  clock_gettime(CLOCK_REALTIME_COARSE, &this->start);

  while(1)
  {
    clock_gettime(CLOCK_REALTIME, &now);
    if ((now.tv_sec - this->start.tv_sec) > 1)
    {
      break;
    }
    YIELD();
  }

  *ret = a + b;
  EXIT();
}

Continuation * sum(Continuation * co,
    int a, int b, int c, int * ret)
{
  VAR_BEGIN
  int sum;
  VAR_END

  if (!this) { *ret = -1; EXIT(); }

  while(CALL_FUNK(add, a, b, &this->sum)) YIELD();
  if (this->sum == -1) { // mem fail
    *ret = -1;
    EXIT();
  }

  while(CALL_FUNK(add, this->sum, c, ret)) YIELD();
  EXIT();
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
