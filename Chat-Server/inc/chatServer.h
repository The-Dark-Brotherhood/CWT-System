#pragma once
#include "../inc/logger.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>

// Constants
#define MAX_CLIENTS     10
#define NUM_THREADS     1 + MAX_CLIENTS
#define SHMKEY_PATH     "."
#define KEY_PATH        "/tmp/"
#define ID_ERROR        -1
#define SHM_KEYID       16535

#define IP_SIZE         16
#define MSG_SIZE        41  // DEBUG: Change later
#define NAME_SIZE       6   // Ni
#define TIME_SIZE       9   // ce

// Structures
//--> Message QUEUE
typedef struct{
  long type;
  char address[IP_SIZE];
  char name[NAME_SIZE];
  char content[MSG_SIZE];
  char time[TIME_SIZE];
} message;

//--> Shared Memory
typedef struct
{
  char address[IP_SIZE];
  char name[NAME_SIZE];
} Client;

typedef struct
{
  int msgQueueID;
  int numberOfClients;
  Client clients[MAX_CLIENTS];
} MasterList;

//--> Client info
typedef struct
{
  int msgQueueID;
  int socket;
  MasterList* shList;
} clientInfo;

// Prototypes
//Thread Functions
void * clientListenningThread(void* data);
void * broadcastMessages(void* data);
int addClient(MasterList* shList, message* recMsg);
void removeClient(MasterList* list, int delIndex);
