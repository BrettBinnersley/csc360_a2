/*
Brett Binnersley
V00776751
Csc 360
Assignment #2

Operating Systems

SimpelList.h - Header file for the PQS

*/

#ifndef SimpleListIncluded
#define SimpleListIncluded

  //Thread variables
  pthread_mutex_t pqsMutex;
  pthread_mutex_t clerkMutex;
  pthread_mutex_t requestMutex; //Covers the clients requests to ensure multi clients are not trying to make requests at the same time
  pthread_cond_t clerkIdle; //Sent when the clerk is idle
  pthread_cond_t clientArrive; //Sent when a client arrives
  pthread_cond_t clientDone; //Sent when the client is done with the clerk (from client->clerk)

  typedef struct listItem { //Struct defining the clerk
    struct listItem* next; //Next node in the list
    struct listItem* prev; //Previous node in the list (used for removing from the linked list)
    struct clientStruct* client;
  } listItem;

  //Add a customer to the lineup
  int AddCustomerToClerkLineUp(struct clientStruct* client);
  int getSizeOfLine();
  //Returns highest priority client AND*** removes it from
  //the collection (lineup) if remove is set to true.
  //If no client is in the lineup, it will return 0 (null)
  struct clientStruct* getHighestPriorityClient(int andRemove);
  void initializeThreadVars();

#endif