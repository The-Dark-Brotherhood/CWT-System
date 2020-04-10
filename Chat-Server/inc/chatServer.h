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
#include <sys/types.h>
#include <sys/select.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>

// Volatile flag
#define TRUE    1
#define FALSE  -1
extern volatile char running;

// Constants
#define MAX_CLIENTS     10
#define NUM_THREADS     2 + MAX_CLIENTS
#define SHMKEY_PATH     "."
#define KEY_PATH        "/tmp/"
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
  int socket;
  char address[IP_SIZE];
  char name[NAME_SIZE];
} Client;

typedef struct
{
  int msgQueueID;
  int numberOfClients;
  Client clients[MAX_CLIENTS];
  pthread_t tid[NUM_THREADS];
} MasterList;

//--> Client info
typedef struct
{
  int msgQueueID;
  int socket;
  MasterList* shList;
} clientInfo;

//= Prototypes
void * clientListenningThread(void* data);
void * broadcastMessages(void* data);                    
int addClient(MasterList* shList, message* recMsg, int socket);
void removeClient(MasterList* list, int delIndex);
void closeServer(int signal_number);
