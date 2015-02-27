/*
Brett Binnersley
V00776751
Csc 360
Assignment #2

Operating Systems

clerk.c - Defines the clerk.
If the clerk is busy it will wait for the "clientDone" convar to be triggered
If the clerk is idle, it will wait for a client to arrive (or pick the highest from the PQS), then
set its state to busy


*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "SimpleList.h"
#include "clerk.h"
#include "customer.h"


int totalClients = 0;
clerkStruct* clerk;
char clientServeOrder[8192]; //String at the end show the serve order
char clientServePos = 0; //Serve order

void setTotalClients(int number)
{
  totalClients = number;
}

int getClerkBusy()
{
  return (clerk->currentClient != 0);
}

struct clientStruct* getCurrentClient()
{
  return clerk->currentClient;
}

//Append ->[ClientNum] to the server order string
void appendClientNumToServeOrder(struct clientStruct* client)
{
  if(strlen(clientServeOrder) == 0)
  {
    sprintf(clientServeOrder, "%d", client->clientNumber);
  }
  else
  {
    sprintf(clientServeOrder, "%s - %d", clientServeOrder, client->clientNumber);
  }
}

void* runClerkFunction(void* ptr)
{
  int customersDealtWith = 0;
  clerk = (clerkStruct* ) ptr;
  clerk->currentClient = 0;
  memset(&clientServeOrder,'\0', sizeof(clientServeOrder));
  while(1)//Constantly wait for something to be in the PQS. Note: there is a convar in getHighestPriority (so it doesnt loop forever)
  {
    //Waiting for a signal from the client (to finish up). We are serving a customer right now!
    if(getClerkBusy())
    {
      pthread_mutex_lock(&clerkMutex);
      pthread_cond_wait(&clientDone, &clerkMutex);
      pthread_mutex_unlock(&clerkMutex);
      clerk->currentClient = 0;

      customersDealtWith++;
      if(customersDealtWith >= totalClients) //We have dealt with every customer, it is safe to exit now :D
      {
        printf("\n\nClients Served in order:\n%s\n", clientServeOrder);
        return (void* ) 0;
      }
    }
    else
    {
      //Waiting for a signal from a client to arrive (if no clients in the line)
      //If there are clients in the line, skip the waiting and grab the one with the highest priority.
      if(getSizeOfLine() == 0)
      {
        pthread_mutex_lock(&clerkMutex);
        pthread_cond_wait(&clientArrive, &clerkMutex);
        pthread_mutex_unlock(&clerkMutex);
      }
      struct clientStruct* client = getHighestPriorityClient(1); //Guaranteed there is somebody in the line
      if(client == 0)
      {
        printf("Error: Priority Queue returned with a null client\n");
        continue; //Note this should never happen. Here incase something <bad> happens though.
      }
      clerk->currentClient = client;
      appendClientNumToServeOrder(client);
      pthread_cond_broadcast(&clerkIdle);
    }
  }
}


