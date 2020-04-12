/*
*  FILE          : chatServer.h
*  PROJECT       : Assignment #4
*  PROGRAMMER    : Gabriel Gurgel & Michael Gordon
*  FIRST VERSION : 2020-04-06
*  DESCRIPTION   : Defines constants, functions and data structures
*                  for the server to function
*
*  Message DATA Structure
*-------------------------
*  char address[IP_SIZE];
*  char name[NAME_SIZE];
*  char content[MSG_SIZE];
*  char time[TIME_SIZE];
*
*  the message just come as a array of bytes but it client name and address gets
*  parsed based on offset and [] separator -> Estabilished as our message protocol
*
*  PROTOCOL:
*  Once the client accepts the TCP connection, the first message that it sends only
*  contain ip address and name. The new client info will be stored in the MasterList.
*  It is important to know that all the messages after the first one also contain that
*  info.
*  Therefore the MESSAGE is:
*   XXX.XXX.XXX.XXX_[AAAAA]_>>_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa_(HH:MM:SS)
*   --- IP ADDR --- -USER      ---------CONTENT-----------------------  --TIME--
*
*  SHUTDOWN (>>bye<<)
*  Every time an client leaves the server, sends the >>bye<< message, the client checks
*  if the server is empty - Triggering the SIGINT interrupt and use a custom handler for
*  cleanning up all the resources used by the servers and joining all the threads
*  The use of interrupt was used due to the fact that the accept() call is BLOCKING.
*  Plus, if the server was closed with clients still connected, it sends the >>bye<<
*  before shutting down
*
*  CLIENT
*  Each client structure has its thread id, socket, name and ip address. Therefore, each
*  client object is linked by its pthread tid and way of communication (socket number).
*  So the listening and broadcasting of messages are donte by checking the client list,
*  MasterList, which is a shared memory sector used by the threads to handle client
*  communication. Mutex were used to protect whenever the list is alterated by a thread.
*
*/
#pragma once
#include "../inc/logger.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/ipc.h>
#include <sys/msg.h>
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

#define MAX_CLIENTS     10
#define SHMKEY_PATH     "."
#define KEY_PATH        "/tmp/"
#define SHM_KEYID       16535
#define EXIT_MSG        ">>bye<<"
#define OFFSET_MSG      27
#define DIRECTION       24

#define IP_SIZE         16
#define HOST_LEN        64
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
