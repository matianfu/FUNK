//============================================================================
// Name        : main.cpp
// Author      : matianfu@gmail.com
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <time.h>
#include "co.h"
#include "Adder.h"
#include "Sum.h"
using namespace std;


/* call FUNK in block waiting mode */
void task1()
{
  int ret;

  cout << "Task1 : " << flush;

  Sum sum;
  while (0 == sum.Do(1, 2, 3, &ret));

  cout << "1 + 2 + 3 = " << ret << endl;

  cout << "Task1 (old): " << flush;

  SumOld sumold(1, 2, 3, &ret);
  while (0 == sumold.Run());

  cout << "1 + 2 + 3 = " << ret << endl;
}

/* call FUNK inside state machine code*/
void task2()
{
  static int state = 0;
  static Sum * s;

  int ret;

  switch (state)
  {
  case 0:
    cout << "Task2 begins" << endl;
    s = new Sum();
    if (s)
      state = 1;
    else
      cout << "  Error Code: " << ret << endl;
    break;

  case 1:
    if (s->Do(4, 5, 6, &ret))
    {
      cout << "Task2 ends :  4 + 5 + 6 = " << ret << endl;
      state = 0;
    }
    break;

  default:
    break;
  }
}

/** show concurrency only **/
void task3()
{
  cout << "Task3" << endl;
  sleep(1);
}

/*
 * main function
 *
 * create FuncB object on stack, block waiting for return.
 */
int main(void)
{
  task1();
  while (1)
  {
    task2();
    task3();
  }
  return 0;
}


