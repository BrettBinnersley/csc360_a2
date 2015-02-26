/*
Brett Binnersley
V00776751
Csc 360
Assignment #2

Operating Systems

PQS.c - Reads input, spawns all client threads and
server thread. Waits for them all to terminate before
the program exits

NOTE: My program does NOT* follow my design document
because there were mistakes in it. It follows the design
given out by Dr. Kui Wi
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#define BUFF_SIZE 1024 //All buffers are 1024 bytes long.
#include "customer.h"
#include "clerk.h"

int main()
{
  char* inputFile = "customers.txt";
  FILE* input = fopen(inputFile,"r");
  int totalClients = 0;
  int pos = 0;
  char tmpBuffer[BUFF_SIZE];
  //Read number of entries
  while(1)
  {
    char c = fgetc(input);
    if(c == '\n')
    {
      totalClients = atoi(tmpBuffer);
      break;
    }
    if(c == EOF)
    {
      printf("No clients to be served\n");
      exit(0);
    }
    tmpBuffer[pos++] = c;
  }

  //Reset buffer position
  pos = 0;
  // pthread_t clientThread[totalClients];

  int readingFile = 1;
  while(readingFile)
  {
    char buffer[BUFF_SIZE];
    memset(&buffer, '\0', BUFF_SIZE);
    while(1)
    {
      char c = fgetc(input);
      if(c == '\n')
      {
        //Parse buffer and extract the useful information
        char * pch;
        pch = strtok (buffer,":,");
        int clientNum = atoi(pch);
        pch = strtok (NULL, ",");
        int arrTime = atoi(pch);
        pch = strtok (NULL, ",");
        int servTime = atoi(pch);
        pch = strtok (NULL, ",");
        int priority = atoi(pch);

        //Spawn client thread
        pthread_t* clientThread = (pthread_t*)malloc(sizeof(pthread_t));

        clientStruct* client = (clientStruct*)malloc(sizeof(clientStruct));;
        client->clientNumber = clientNum;
        client->arrivalTime = arrTime;
        client->serviceTime = servTime;
        client->priority = priority;

        int success = pthread_create( clientThread, NULL, runClientFunction, (void*) client);
        if (success != 0)
        {
          fprintf(stderr, "Can't Create Client Thread: %s\n", strerror(success));
          exit(1);
        }
        memset(&buffer, '\0', BUFF_SIZE);
        pos = 0;
        continue;
      }
      if(c == EOF)
      {
        readingFile = 0;
        memset(&buffer, '\0', BUFF_SIZE);
        pos = 0;
        continue;
      }
      buffer[pos++] = c;
    }
  } //End read file/spawn client threads


  sleep(10);
  // pthread_join( thread1, &retval);
  printf("Main Done\n");

  return 0;
}
