/*
Brett Binnersley
V00776751
Csc 360
Assignment #2

Operating Systems

SimpleList.c - A PQS data structure in which clients can get added to and removed from.
It is protected by a mutex. Also all the thread variables (mutexes, convars) are defined here.

*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include "SimpleList.h"
#include "clerk.h"
#include "customer.h"


struct listItem* listHead;
int sizeOfList = 0;


//Add a customer to the lineup. Returns 0 on success, non zero on failure
int AddCustomerToClerkLineUp(struct clientStruct* client)
{
  int success = pthread_mutex_lock(&pqsMutex);
  if(success != 0)
  {
    printf("Error locking PQS Mutex %s\n", strerror(success));
    return success;
  }

  while(1) //Wrap in a while loop, so we can easily break out and unlock the mutex when done
  {
    struct listItem* item = malloc(sizeof(struct listItem));
    item->next = 0;
    item->prev = 0;
    item->client = client;

    if(listHead == 0)
    {
      listHead = item;
      break;
    }

    struct listItem* iter = listHead;
    while(iter->next != 0)
    {
      iter = iter->next;
    }
    iter->next = item;
    item->prev = iter;
    break;
  }
  ++sizeOfList;
  success = pthread_mutex_unlock(&pqsMutex);
  if(success != 0)
  {
    printf("Error unlocking PQS Mutex %s\n", strerror(success));
    return success;
  }
  return 0;
}

//# of elements (customers) present in the PQS. Returns -1 on an error
int getSizeOfLine()
{
  //Lock to ensure that nobody is editing the PQS as we get the size
  int success = pthread_mutex_lock(&pqsMutex);
  if(success != 0)
  {
    printf("Error locking PQS Mutex %s\n", strerror(success));
    return -1;
  }
  int x = sizeOfList;
  success = pthread_mutex_unlock(&pqsMutex);
  if(success != 0)
  {
    printf("Error unlocking PQS Mutex %s\n", strerror(success));
    return -1;
  }
  return x;
}

//Returns highest priority client AND*** removes it from the collection (lineup)
//If no client is in the lineup, it will return 0 (null)
struct clientStruct* getHighestPriorityClient(int andRemove)
{
  int success = pthread_mutex_lock(&pqsMutex);
  if(success != 0)
  {
    printf("Error locking PQS Mutex %s\n", strerror(success));
  }
  struct listItem* highestPri = listHead; //highest priority client
  while(1) //Wrap in a while loop, so we can easily break out and unlock the mutex when done
  {
    /***********************************
    Handle base cases (list size <= 1)
    ***********************************/
    if(listHead == 0)
    {
      break; //Will return 0.
    }
    if(listHead->next == 0)
    {
      listHead = 0; //Set head to 0, (will return current head)
      break;
    }

    /************************************************************************************************
    Find the node in the list that has the highest priority acording to (priority, serve time, order)
    *************************************************************************************************/

    struct listItem* iter = listHead;     //Current iterator (go over the entire list)
    while(iter->next != 0)
    {
      iter = iter->next;

      //Check conditions AFTER incrementing iter (intentional)
      if(iter->client->priority > highestPri->client->priority) //Higher priority -> Prioritize
      {
        highestPri = iter;
        continue;
      }
      else if (iter->client->priority == highestPri->client->priority) //Same priority => Use sooner arrival time
      {
        if(iter->client->arrivalTime < highestPri->client->arrivalTime)
        {
          highestPri = iter;
          continue;
        }
        else if (iter->client->arrivalTime == highestPri->client->arrivalTime) //Same priority + arrival time => Use shorter service time
        {
          if(iter->client->serviceTime < highestPri->client->serviceTime)
          {
            highestPri = iter;
            continue;
          }
          else if(iter->client->serviceTime == highestPri->client->serviceTime)//Same priority + service time + arrival time=> Use order of input
          {
            if(iter->client->placeInFile < highestPri->client->placeInFile)
            {
              highestPri = iter;
              continue;
            }
          }
        }
      }
      else
      {
        continue; //Just continue, as this node had a lower priority.
      }
    }


    /*********************************************************************************
    Remove it from the list (highestPri will be the node that needs to be removed here)
    **********************************************************************************/
    if(andRemove)
    {
      //Removing the head (special case where the pointer at the start of the linked list changes).
      if(highestPri->prev == 0)
      {
        listHead = highestPri->next;
        if(highestPri->next)
        {
          highestPri->next->prev = 0;
        }
        break;
      }
      //Anything except the head
      highestPri->prev->next = highestPri->next; //Set previous nodes next pointer to my next
      if(highestPri->next)
      {
        highestPri->next->prev = highestPri->prev; //Set next node (iff it exists) previous node to my previous node
      }
    }
    break; //Exit while(true) loop.
  }

  success = pthread_mutex_unlock(&pqsMutex);
  if(success != 0)
  {
    printf("Error unlocking PQS Mutex %s\n", strerror(success));
  }

  //Return 0 if no node was found, else return the nodes client data if one was found
  if(highestPri == 0)
  {
    return 0;
  }
  else
  {
    --sizeOfList; //Item remove from the list;
    return highestPri->client; //Return the highest priority client (xor 0)
  }
}

//Thread variables (mutexes and convars initialized below)
void initializeThreadVars()
{
  pthread_mutex_init(&pqsMutex, 0);
  pthread_mutex_init(&clerkMutex, 0);
  pthread_mutex_init(&requestMutex, 0);
  pthread_cond_init(&clerkIdle, 0);
  pthread_cond_init(&clientArrive, 0);
  pthread_cond_init(&clientDone, 0);
}
