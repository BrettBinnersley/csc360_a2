/*
Brett Binnersley
V00776751
Csc 360
Assignment #2

Operating Systems

PQS.c - Reads input, spawns all client threads and
clerk thread. Waits for them all to terminate before
the program exits

This is the main thread (idle most of the time).

*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#define BUFF_SIZE 1024 //All buffers are 1024 bytes long.
#include "customer.h"
#include "clerk.h"
#include "SimpleList.h"

//Function for trimming whitespace from around a string.
char* withoutSpaces(char* str)
{
  char *endOfStr;

  while(isspace(*str))
  {
    str++;
  }

  if(*str == 0)
  {
    return str;
  }

  endOfStr = str + strlen(str) - 1;
  while(isspace(*endOfStr) && endOfStr > str)
  {
    endOfStr--;
  }

  *(endOfStr + 1) = 0; // Write null terminator

  return str;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    printf("Usage pqs <input>\n");
    exit(0);
  }

  char* inputFile = argv[1];
  FILE* input = fopen(inputFile,"r");
  if(input == 0)
  {
    printf("Unable to locate input file\n");
    exit(0);
  }
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
      printf("No clients to be served.\n");
      exit(0);
    }
    tmpBuffer[pos++] = c;
  }

  //Spawn a clerk thread to handle all of the clients requests
  initializeThreadVars();
  initializeClientTime();
  void* retval;
  pthread_t clerkThread;
  clerkStruct clerk;

  int success = pthread_create( &clerkThread, NULL, runClerkFunction, (void*) &clerk);
  if (success != 0)
  {
    fprintf(stderr, "Can't Create Clerk Thread: %s\n", strerror(success));
    exit(1);
  }

  //Reset buffer position
  //create all client threads + counters
  setTotalClients(totalClients);
  pthread_t clientThreads[totalClients];
  clientStruct clientStructs[totalClients];
  int currentClient = 0;
  pos = 0;

  int readingFile = 1;
  while(readingFile)
  {
    char buffer[BUFF_SIZE];
    memset(&buffer, '\0', BUFF_SIZE);
    while(1)
    {
      char c = fgetc(input);
      if(c == '\n' || c == EOF)
      {
        char* trimmedBuff = withoutSpaces(buffer);
        if(strlen(trimmedBuff) != 0)
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
          pthread_t* clientThread = &clientThreads[currentClient];
          clientStruct* client = &clientStructs[currentClient];
          client->clientNumber = clientNum;
          client->arrivalTime = arrTime;
          client->serviceTime = servTime;
          client->priority = priority;
          client->placeInFile = currentClient;

          int success = pthread_create( clientThread, NULL, runClientFunction, (void*) client);
          if (success != 0)
          {
            fprintf(stderr, "Can't Create Client Thread: %s\n", strerror(success));
            exit(1);
          }
          memset(&buffer, '\0', BUFF_SIZE);
          pos = 0;
          currentClient++;
          //Continue IF EOF is not reached.
          //If EOF is reached, it will hit the block of code below that breaks from reading the input
          if(c != EOF)
          {
            continue;
          }
        }
      } //End reading/parsing one line in the input file
      if(c == EOF)
      {
        readingFile = 0;
        memset(&buffer, '\0', BUFF_SIZE);
        pos = 0;
        break;
      }
      buffer[pos++] = c;
    }
  } //End read file/spawn client threads

  //Wait for the clerk to finish before exiting the program
  pthread_join( clerkThread, &retval);

  return 0;
}//End main


