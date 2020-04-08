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

  //DEBUG:
  strcpy(recMsg.address, "192.169.0.1");
  strcpy(recMsg.name, "Gab");
  recMsg.type = 1;
  int contentSize = (int) strlen(recMsg.content);

  // Read first message -> Add client info to List
  read(cInfo->socket, recMsg.content, MSG_SIZE);
  int insertedAt = addClient(shList, &recMsg);

  while(strcmp(recMsg.content,"quit") != 0)
  {
    // Push message in the queue and write ACK back to client
    msgsnd(cInfo->msgQueueID, (void*)&recMsg, msgSize, 0);
    write(cInfo->socket, "ACK", strlen(recMsg.content));

    // Reset buffer and read next message
    memset(recMsg.content, 0, MSG_SIZE);
    read(cInfo->socket, recMsg.content, MSG_SIZE);
  }

  // Remove from the master list and clean up
  removeClient(shList, insertedAt);
  printf("Number of Clients: %d\n", shList->numberOfClients );      // DEBUG: Number of clients left
  close(cInfo->socket);
}


void * broadcastMessages(void* data)
{
  // Get client Master list
  MasterList* clientList = (MasterList*)data;
  struct msqid_ds queueInfo;

  message msg;
  const int msgSize = sizeof(message) - sizeof(long);

  while(1)
  {
    msgctl(clientList->msgQueueID, IPC_STAT, &queueInfo); // Update queue info
    if((int)queueInfo.__msg_cbytes/msgSize > 0)           // Broadcast msgs if there are
    {
      msgrcv(clientList->msgQueueID, &msg, msgSize, 0, IPC_NOWAIT);
      printf("Resulted: %s\n", msg.content);
      fflush(stdout);
    }
  }
}

int addClient(MasterList* shList, message* recMsg)
{
  int index = shList->numberOfClients - 1;
  strcpy(shList->clients[0].name, recMsg->name);
  strcpy(shList->clients[0].address, recMsg->address);

  return index;
}


// FUNCTION      : removeClient
// DESCRIPTION   : Deletes an innactivity client from the server list
//
// PARAMETERS    :
//	MasterList* list : Pointer to the shared memory master list
//
// RETURNS       :
//	void
void removeClient(MasterList* list, int delIndex)
{
	// Replace the deleted object with last element
  int lastIndex = list->numberOfClients - 1;
  if(lastIndex != delIndex)
  {
    list->clients[delIndex] = list->clients[lastIndex];
  }
  // Remove the last client
  list->numberOfClients--;
}
