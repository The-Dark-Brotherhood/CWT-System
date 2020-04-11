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


void * clientListenningThread(void* data)
{
  // Newly accepted client info
  clientInfo* cInfo = (clientInfo*) data;
  MasterList* shList = cInfo->shList;

  // Setup message
  message recMsg;
  memset(recMsg.content, 0, MSG_SIZE);

  // Read first message -> Add client info to List
  read(cInfo->socket, recMsg.content, MSG_SIZE);
  recMsg.type = 1;
  int insertedAt = addClient(shList, &recMsg, cInfo->socket);

  // If message contains exit message -> QUIT
  while((strstr(recMsg.content, EXIT_MSG)) == NULL)
  {
    printf("Received: %s\n", recMsg.content);
    // Push message into the queue, reset buffer and read next msg
    msgsnd(shList->msgQueueID, (void*)&recMsg, MSG_SIZE, 0);
    memset(recMsg.content, 0, MSG_SIZE);
    read(cInfo->socket, recMsg.content, MSG_SIZE);
  }

  // Remove from the master list and clean up
  removeClient(shList, insertedAt);
  close(cInfo->socket);
}


void * broadcastMessages(void* data)
{
  // Get client Master list
  MasterList* clientList = (MasterList*)data;
  struct msqid_ds queueInfo;
  message msg;

  while(running != FALSE)
  {
    msgctl(clientList->msgQueueID, IPC_STAT, &queueInfo); // Check how many messages
    if((int)queueInfo.__msg_cbytes/MSG_SIZE > 0)           // are in the queue
    {
      msgrcv(clientList->msgQueueID, &msg, MSG_SIZE, 1, IPC_NOWAIT);               // Take message from queue
      for(int counter = 0; counter < clientList->numberOfClients; counter++)      // Send messages to all clients
      {                                                                           // DEBUG: But the one who sent the message
        write(clientList->clients[counter].socket, msg.content, strlen(msg.content));
        fflush(stdout);
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
