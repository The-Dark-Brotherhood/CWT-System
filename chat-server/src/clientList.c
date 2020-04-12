/*
*  FILE          : clientList.c
*  PROJECT       : Assignment #4
*  PROGRAMMER    : Gabriel Gurgel & Michael Gordon
*  FIRST VERSION : 2020-04-08
*  DESCRIPTION   : Defines functions for the management of the MasterList
*                  a shared memory section for storing all the sever's
*                  clients information
*/
#include "../inc/chatServer.h"

// FUNCTION      : addClient
// DESCRIPTION   : Add a client to the client master list
//
// PARAMETERS    :
//	MasterList* list  : Pointer to the shared memory master list
//  message* firstMsg : Pointer to the message received by the server.
//                      Contains client info that is going to be stored
//                      in the server
//  int socket        : Socket of the new client
//
// RETURNS       :
//	void
int addClient(MasterList* list, message* firstMsg, int socket)
{
  pthread_mutex_lock(&lock);
  list->numberOfClients++;
  int index = findEmptyNode(list);

  // Parse client info from first message
  char clientAddr[IP_SIZE] = { 0 };
  char* foundIt = strchr(firstMsg->content, ' ');
  int ipSeparator = foundIt - firstMsg->content;
  strncpy(clientAddr, firstMsg->content, ipSeparator);

  char* username = strtok(firstMsg->content, NAME_BEGIN);
  username = strtok(NULL, NAME_END);

  // Assign client
  list->clients[index].socket = socket;
  strcpy(list->clients[index].address, clientAddr);
  strcpy(list->clients[index].name, username);
  serverLog("INFO", "Client Accepted - Number of Clients: %d", list->numberOfClients);
  serverLog("DEBUG", "CLIENT INFO: %s -- %s", list->clients[index].name, list->clients[index].address);

  pthread_mutex_unlock(&lock);
  return index;
}

// FUNCTION      : findEmptyNode
// DESCRIPTION   : Find the next empty node in the client list
//
// PARAMETERS    :
//	MasterList* list  : Pointer to the shared memory master list
//
// RETURNS       :
//	void
int findEmptyNode(MasterList* list)
{
  int foundIndex = -1;
  for(int counter = 0; counter < MAX_CLIENTS; counter++ )
  {
    if(list->clients[counter].socket == -1)
    {
      foundIndex = counter;
      break;
    }
  }

  return foundIndex;
}


// FUNCTION      : removeClient
// DESCRIPTION   : Deletes an innactivity client from the server list
//
// PARAMETERS    :
//	MasterList* list : Pointer to the shared memory master list
//  int     delIndex : Client index that is going to be deleted
//
// RETURNS       :
//	void
void removeClient(MasterList* list, int delIndex)
{
  pthread_mutex_lock(&lock);

  // Close socket and clean client info
  close(list->clients[delIndex].tid);
  list->clients[delIndex].socket = -1;
  list->clients[delIndex].tid = 0;
  list->numberOfClients--;

  pthread_mutex_unlock(&lock);
  serverLog("INFO", "Client Removed - Number of Clients: %d", list->numberOfClients);

  // Check if server is empty -> Terminate the server
  if(list->numberOfClients == 0)
  {
    serverLog("INFO", "No more clients -- CLOSING SERVER");
    running = FALSE;
    kill(getpid(), SIGINT);
  }
}
