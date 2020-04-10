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

// DEBUG: This entire function is debug: implement as thread
void * clientListenningThread(void* data)
{
  // Newly accepted client info
  clientInfo* cInfo = (clientInfo*) data;
  MasterList* shList = cInfo->shList;
  printf("--> %p Inside thread\n", shList );

  // Setup message
  message recMsg;
  memset(recMsg.content, 0, MSG_SIZE);

  //DEBUG:----------------------------------------//
  strcpy(recMsg.address, "192.169.0.1");
  strcpy(recMsg.name, "Gab");
  recMsg.type = 1;
  int contentSize = (int) strlen(recMsg.content);
  //----------------------------------------------//

  // Read first message -> Add client info to List
  read(cInfo->socket, recMsg.content, MSG_SIZE);
  int insertedAt = addClient(shList, &recMsg, cInfo->socket);

  while(strcmp(recMsg.content,"quit") != 0)     // DEBUG: quit -> bye
  {
    // Push message in the queue and write ACK back to client
    msgsnd(cInfo->msgQueueID, (void*)&recMsg, msgSize, 0);

    // Reset buffer and read next message
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
    if((int)queueInfo.__msg_cbytes/msgSize > 0)           // are in the queue
    {
      msgrcv(clientList->msgQueueID, &msg, msgSize, 1, IPC_NOWAIT);               // Take message from queue
      for(int counter = 0; counter < clientList->numberOfClients; counter++)      // Send messages to all clients
      {                                                                           // But the one who sent the message
        write(clientList->clients[counter].socket, msg.content, strlen(msg.content));
        fflush(stdout);
      }
    }
  }
}
