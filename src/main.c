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

  if (this->co.ep == (void*)SIG_STOP) EXIT();

  FUNK_BEGIN

  clock_gettime(CLOCK_REALTIME, &this->start);
  while(1)
  {
    clock_gettime(CLOCK_REALTIME, &now);
    if ((now.tv_sec - this->start.tv_sec) > 1)
      break;
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
    Continuation* sub;
  VAR_END

  if ((uintptr_t)this->co.ep == SIG_STOP) {
    STOP_FUNK(this->sub, add, 0, 0, 0);
    EXIT();
  }

  this->co.funk = (FUNK)sum;
  FUNK_ALLOC(this->sub, add, 0, 0, 0);

  FUNK_BEGIN

  CALL_FUNK(this->sub, add, a, b, &this->sum);
  CALL_FUNK(this->sub, add, this->sum, c, ret);
  EXIT();
}

int main(void)
{
  int ret = 0;
  Continuation* st_co = 0;

  setbuf(stdout, NULL);


//  Continuation* co = 0;
//  while ((co = sum(co, 1, 2, 4, &ret)))
//  {
//    sleep(1);
//    printf("wake up\n");
//  }
//  printf("sum 1, 2, 4 is %d\n", ret);
//
//  ret = 0;
//  co = 0;
//  while ((co = sum(co, 1, 2, 4, &ret)))
//  {
//    sleep(1);
//    printf("wake up\n");
//    counter++;
//    if (counter > 1)
//    {
//      STOP_FUNK(co, sum, 0, 0, 0, 0);
//      break;
//    }
//  }

  FUNK_ALLOC(st_co, sum, 0, 0, 0, 0);

  while(sum(st_co, 1, 5, 9, &ret));
  printf("ret is %d\n", ret);

  while(sum(st_co, 2, 8, 8, &ret));
  printf("ret is %d\n", ret);

  return 0;
}
