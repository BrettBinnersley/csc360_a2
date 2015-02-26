/*
Brett Binnersley
V00776751
Csc 360
Assignment #2

Operating Systems

customer.h - Defines what a customer looks like, as well
as the function that they each run on a thread.

NOTE: My program does NOT* follow my design document
because there were mistakes in it. It follows the design
given out by Dr. Kui Wi
*/

#ifndef customer_included
#define customer_included

  typedef struct clientStruct {
    int clientNumber;
    int arrivalTime;
    int serviceTime;
    int priority;
  } clientStruct;

  void* runClientFunction(void *ptr)
  {
    clientStruct* c = (clientStruct*) ptr;

    usleep(c->arrivalTime * 100000);

    int cNum = c->clientNumber;
    float aTime = (float)(c->arrivalTime);
    float sTime = (float)(c->serviceTime);
    int priority = c->priority;

    printf("Customer %2d arrives: Arrival time (%.2f), Service time (%.1f), Priority (%2d). \n", cNum, aTime, sTime, priority);
    return (void *) 0;
  }

#endif