/*
Brett Binnersley
V00776751
Csc 360
Assignment #2

Operating Systems

customer.c - Defines the customer.
The customer will wait <X> seconds before inserting itself into the PQS (the line)
It will then request service (wait for the clerk to tell the client its his turn).
Once service is granted, it will wait (sleep) how long its service time is, then it
will signal to the clerk that it is done (clientDone convar)

*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#include "clerk.h"
#include "customer.h"
#include "SimpleList.h"

struct timeval initialTime;

//Give initial values for time and convars/mutexes
void initializeClientTime()
{
  gettimeofday(&initialTime,NULL);
}

//Time between (now) and when the clerk timer was initialized
float calculateDeltaTime()
{
  struct timeval end;
  gettimeofday(&end, NULL);
  long int timeInUSecs = ((end.tv_sec * 1000000 + end.tv_usec) - (initialTime.tv_sec * 1000000 + initialTime.tv_usec));
  long int timeInTenthSeconds = timeInUSecs / 100000;
  float timeAsFloat = ((float)timeInTenthSeconds) / 10.0f;
  return timeAsFloat;
}

//Requesting service (wait for signal, check if I am the requested client, if not wait for another signal)
void reqestService(clientStruct* c)
{
  pthread_mutex_lock(&requestMutex);
  while(1)
  {
    pthread_cond_wait(&clerkIdle, &requestMutex);
    if(getCurrentClient() == c)
    {
      break;
    }
  }
  pthread_mutex_unlock(&requestMutex);
}

void* runClientFunction(void* ptr)
{
  clientStruct* c = (clientStruct* ) ptr;
  int cNum = c->clientNumber;
  float aTime = ((float)(c->arrivalTime))/(10.0f);
  float sTime = ((float)(c->serviceTime))/(10.0f);
  int priority = c->priority;
  usleep(c->arrivalTime * 100000);
  printf("Customer %2d arrives: Arrival time (%.2f), Service time (%.1f), Priority (%2d). \n", cNum, aTime, sTime, priority);

  if(getClerkBusy())
  {
    printf("Customer %2d waits for the finish of customer %2d. \n", cNum, getCurrentClient()->clientNumber);
  }
  int addSuccess = AddCustomerToClerkLineUp(c); //Add customer to the lineup (PQS). This is protected by a mutex.
  if(addSuccess !=0) //Catch system errors
  {
    clientDealtWith(); //We can say that this client was dealt with
    pthread_cond_broadcast(&clientArrive); //Broadcast (so the clerk gets signalled that something changed)
    return (void* ) 0;
  }
  pthread_cond_broadcast(&clientArrive);

  //Request service from the clerk. It will be stuck here until clerk says it is ready to serve me
  reqestService(c);

  //Time for client to be served (post notice that the clerk is serving the customer)
  printf("The clerk starts serving customer %2d at time %.2f. \n", c->clientNumber, calculateDeltaTime());
  usleep(c->serviceTime * 100000);
  printf("The clerk finishes the service to customer %2d at time %.2f. \n", c->clientNumber, calculateDeltaTime());

  //Release service (tell clerk that I am done)
  pthread_cond_broadcast(&clientDone);
  return (void *) 0;
}

