/*
Brett Binnersley
V00776751
Csc 360
Assignment #2

Operating Systems

customer.h - Header file for the customer
*/

#ifndef customerIncluded
#define customerIncluded

  typedef struct clientStruct {
    int clientNumber;
    int arrivalTime;
    int serviceTime;
    int priority;
    int placeInFile; //Last tiebreaker for output
  } clientStruct;

  void* runClientFunction(void* ptr);
  void initializeClientTime();

#endif