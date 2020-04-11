/*
*  FILE          : chatServer.h
*  PROJECT       : Assignment #4
*  PROGRAMMER    : Gabriel Gurgel & Michael Gordon
*  FIRST VERSION : 2020-04-06
*  DESCRIPTION   : Defines constants, functions and data structures
*                  for the server to function
*/
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
#include <ifaddrs.h>
#include <sys/shm.h>
#include <sys/stat.h>

// Volatile flag for controlling thread
#define TRUE    1
#define FALSE  -1
extern volatile char running;
extern pthread_mutex_t lock;

// Constants
#define PORT_SWITCH       "-port"
#define PORT_SWITCH_SIZE  5

#define MAX_CLIENTS     3
#define SHMKEY_PATH     "."
#define KEY_PATH        "/tmp/"
#define SHM_KEYID       16535
#define EXIT_MSG        ">>bye<<"
#define OFFSET_MSG      27

#define IP_SIZE         16
#define CONT_SIZE       41  // DEBUG: Change later
#define NAME_SIZE       6   // Ni
#define TIME_SIZE       9   // ce
#define MSG_SIZE        78
#define NAME_BEGIN      "["
#define NAME_END        "]"

// Structures
typedef struct{
  long type;
  int socket;
  char content[MSG_SIZE];
} message;

typedef struct
{
  int  socket;
  pthread_t tid;
  char address[IP_SIZE];
  char name[NAME_SIZE];
} Client;

typedef struct
{
  int msgQueueID;
  int numberOfClients;
  pthread_t broadcastTid;
  Client clients[MAX_CLIENTS];
} MasterList;

typedef struct
{
  int index;
  MasterList* shList;
} clientInfo;

//= Prototypes
void * clientListenningThread(void* data);
void * broadcastMessages(void* data);
int addClient(MasterList* shList, message* recMsg, int socket);
int findEmptyNode(MasterList* list);
void removeClient(MasterList* list, int delIndex);
void closeServer(int signal_number);
void serverShutdownSignal(MasterList* clientList);

void getHostIp(char* clientIP);
