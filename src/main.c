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

/*
 * This function simulate adding two positive numbers
 * and wait for 1 second before returning result.
 *
 * The code deals with memory failure and invalid arguments.
 * The code responds to KILL signal.
 * The code calls YIELD but does not call sub-FUNK
 */
Continuation* add(Continuation* co,
    int a, int b, int * ret)
{
  struct timespec now;
  VAR_BEGIN
    struct timespec start;
  VAR_END

  if (!this || a < 0 || b < 0) { *ret = -1; EXIT(); }
  if (KILL_SIGNALLED()) EXIT();

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

/*
 * This function simulate summing up three positive numbers
 * using add function.
 *
 * The code deals with memory failure and invalid arguments.
 * The code responds to KILL signal and kill sub-FUNK accordingly.
 * The code calls sub-FUNK and YIELD.
 */
Continuation * sum(Continuation * co,
    int a, int b, int c, int * ret)
{
  VAR_BEGIN
    int sum;
    Continuation* sub;
  VAR_END

  if (!this || a < 0 || b < 0 || c < 0) { *ret = -1; EXIT(); }
  if (KILL_SIGNALLED()) {
    KILL_FUNC(this->sub, add, 0, 0, 0);
    EXIT();
  }

  while (CALL_FUNK(this->sub, add, a, b, &this->sum))
    YIELD();

  if (this->sum == -1) // mem fail
  {
    *ret = -1;
    EXIT();
  }

  while (CALL_FUNK(this->sub, add, this->sum, c, ret))
    YIELD();

  EXIT();
}

int main(void)
{
  int ret;
  int counter;
  Continuation* co;

  setbuf(stdout, NULL);


  /* case 1: call sum FUNK
   * notice that you can put other jobs in while-loop
   * this is important pattern for polling event
   */
  ret = 0;
  co = 0; // be sure to null the pointer before using it.
  while (CALL_FUNK(co, sum, 1, 2, 4, &ret))
  {
    sleep(1);
    printf("wake up\n");
  }
  printf("sum 1, 2, 4 is %d\n", ret);

  /* case 2: call sum FUNK
   * this time we cancel the job after sleep twice
   * this is also EXTREMELY important when high level
   * state machine transit to new state, and you need
   * to destroy the current one.
   */
  ret = 0;
  co = 0;
  while (CALL_FUNK(co, sum, 1, 2, 4, &ret))
  {
    sleep(1);
    printf("wake up\n");
    counter++;
    if (counter > 1)
    {
      KILL_FUNC(co, sum, 0, 0, 0, 0);
      printf("funk cancelled");
      break;
    }
  }
  printf("sum 3, 5, 7 is %d\n", ret);

  return 0;
}
