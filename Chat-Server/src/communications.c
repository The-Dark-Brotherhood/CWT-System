/*
*  FILE          : communications.c
*  PROJECT       : Assignment #4
*  PROGRAMMER    : Gabriel Gurgel & Michael Gordon
*  FIRST VERSION : 2020-04-06
*  DESCRIPTION   : Defines thread functions for communication purposes: Client
*                  listenning function, and Broadcast function
*/
// REFERENCE: The Hoochamacallit
#include "../inc/chatServer.h"
const int msgSize = sizeof(message) - sizeof(long);

void * clientListenningThread(void* data)
{
  // Newly accepted client info
  clientInfo* cInfo = (clientInfo*) data;
  MasterList* shList = cInfo->shList;

  // Setup message
  message recMsg;
  memset(recMsg.content, 0, MSG_SIZE);
  int clientSocket = shList->clients[cInfo->index].socket;

  // Read first client's message
  read(clientSocket, recMsg.content, MSG_SIZE);
  printf("Rec: --> %s\n", recMsg.content);

  // If message contains exit message -> QUIT
  while((strstr(recMsg.content, EXIT_MSG)) == NULL)
  {
    // Push message into the queue, reset buffer and read next msg
    recMsg.type = 1;
    recMsg.socket = clientSocket;
    msgsnd(shList->msgQueueID, (void*)&recMsg, msgSize, 0);
    memset(recMsg.content, 0, MSG_SIZE);
    read(clientSocket, recMsg.content, MSG_SIZE);
  }

  // Remove from the master list and clean up
  removeClient(shList, cInfo->index);
  close(clientSocket);
}


void * broadcastMessages(void* data)
{
  // Get client Master list
  MasterList* list = (MasterList*)data;
  struct msqid_ds queueInfo;
  message msg;

  while(running != FALSE)
  {
    msgctl(list->msgQueueID, IPC_STAT, &queueInfo); // Check how many messages
    if((int)queueInfo.__msg_cbytes/msgSize > 0)     // are in the queue
    {
      int foundClients = 0;
      msgrcv(list->msgQueueID, &msg, msgSize, 1, IPC_NOWAIT);              // Take message from queue
      for(int counter = 0; foundClients != list->numberOfClients; counter++)      // Send messages to all clients
      {
        if(list->clients[counter].socket != -1)
        {
          if(list->clients[counter].socket != msg.socket)
          {
            write(list->clients[counter].socket, msg.content, strlen(msg.content));
            fflush(stdout);
          }
          foundClients++;
        }
      }
    }
  }
}

void serverShutdownSignal(MasterList* clientList)
{
  for(int counter = 0; counter < clientList->numberOfClients; counter++)
  {
    write(clientList->clients[counter].socket, EXIT_MSG, strlen(EXIT_MSG) + 1);
    fflush(stdout);
  }
}

// DEBUG:
void getHostIp(char* clientIP)
{

  //https://stackoverflow.com/questions/4139405/how-can-i-get-to-know-the-ip-address-for-interfaces-in-c
  struct ifaddrs *ifap, *ifa;
  struct sockaddr_in *sa;
  char *addr;
  getifaddrs (&ifap);
  for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
      if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET) {
          sa = (struct sockaddr_in *) ifa->ifa_addr;
          addr = inet_ntoa(sa->sin_addr);
          if(!strcmp(ifa->ifa_name, "ens33"))
          {
            strcpy(clientIP, addr);
          }
      }
  }
  freeifaddrs(ifap);
}
