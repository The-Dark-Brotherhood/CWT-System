/*
*  FILE          : communications.c
*  PROJECT       : Assignment #4
*  PROGRAMMER    : Gabriel Gurgel & Michael Gordon
*  FIRST VERSION : 2020-04-06
*  DESCRIPTION   : Defines thread functions for communication purposes: Client
*                  listenning function, and Broadcast function
*/
#include "../inc/chatServer.h"
const int msgSize = sizeof(message) - sizeof(long);

// FUNCTION      : clientListenningThread
// DESCRIPTION   : (THREAD) Listen to a client socket and stores the message in
//                queue
//
// PARAMETERS    :
//	void* data  : (MasterList*) List of all the clients + index of the new client
//
// RETURNS       :
//	void*
void * clientListenningThread(void* data)
{
  // Newly accepted client info
  clientInfo* cInfo = (clientInfo*) data;
  MasterList* shList = cInfo->shList;
  int clientIndex = cInfo->index;

  // Setup message
  message recMsg;
  memset(recMsg.content, 0, MSG_SIZE);
  int clientSocket = shList->clients[cInfo->index].socket;

  // Read first client's message
  read(clientSocket, recMsg.content, MSG_SIZE);

  // If message contains exit message -> QUIT
  while(running != FALSE)
  {
    if(strncmp((recMsg.content + OFFSET_MSG), EXIT_MSG, strlen(EXIT_MSG)) == 0)
    {
      break;
    }

    // Push message into the queue, reset buffer and read next msg
    recMsg.type = 1;
    recMsg.socket = clientSocket;
    msgsnd(shList->msgQueueID, (void*)&recMsg, msgSize, 0);
    memset(recMsg.content, 0, MSG_SIZE);
    read(clientSocket, recMsg.content, MSG_SIZE);
  }

  // Remove from the master list and clean up
  removeClient(shList, clientIndex);
}

// FUNCTION      : broadcastMessages
// DESCRIPTION   : (THREAD) Broadcast messages in the queue to the connected clients
//
// PARAMETERS    :
//	void* data  : (MasterList*) List of all the clients
//
// RETURNS       :
//	void*
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
      msgrcv(list->msgQueueID, &msg, msgSize, 1, IPC_NOWAIT);                 // Take message from queue
      for(int counter = 0; foundClients != list->numberOfClients; counter++)  // Send messages to all clients
      {
        if(list->clients[counter].socket != -1)
        {
          if(list->clients[counter].socket != msg.socket)
          {
            msg.content[DIRECTION] = '<';
            msg.content[DIRECTION + 1] = '<';
            write(list->clients[counter].socket, msg.content, strlen(msg.content));
            fflush(stdout);
          }
          foundClients++;
        }
      }
    }
  }
}

// FUNCTION      : serverShutdownSignal
// DESCRIPTION   : Indicate to all clients via the EXIT_MSG that the server
//                 is shutting down
//
// PARAMETERS    :
//	MasterList* list  : Pointer to the shared memory master list
//
// RETURNS       :
//	void*
void serverShutdownSignal(MasterList* clientList)
{
  for(int counter = 0; counter < clientList->numberOfClients; counter++)
  {
    write(clientList->clients[counter].socket, EXIT_MSG, strlen(EXIT_MSG) + 1);
    fflush(stdout);
  }
}
