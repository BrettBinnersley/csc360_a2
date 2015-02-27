/*
Brett Binnersley
V00776751
Csc 360
Assignment #2

Operating Systems

clerk.h - Header file for clerk
*/

#ifndef clerkIncluded
#define clerkIncluded

  typedef struct clerkStruct { //Struct defining the clerk
    struct clientStruct* currentClient;
  } clerkStruct;

  void* runClerkFunction(void* ptr);

  void setTotalClients(int number);
  int getClerkBusy(); //Returns nonzero if clerk is busy, 0 if it is idle
  struct clientStruct* getCurrentClient();//Returns current client (or 0 if no client is present)


#endif