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
//	MasterList* list : Pointer to the shared memory master list
//  message* recMsg  : Pointer to the message received by the server.
//                     Contains client info that is going to be stored
//                     in the server
//
// RETURNS       :
//	void
int addClient(MasterList* list, message* recMsg, int socket)
{
  list->numberOfClients++;
  int index = list->numberOfClients - 1;

  // Parse
  char clientAddr[IP_SIZE] = { 0 };
  char* foundIt = strchr(recMsg->content, ' ');
  int ipSeparator = foundIt - recMsg->content;
  strncpy(clientAddr, recMsg->content, ipSeparator);

  char* username = strtok(recMsg->content, NAME_BEGIN);
  username = strtok(NULL, NAME_END);

  // Assign client
  list->clients[index].socket = socket;
  strcpy(list->clients[index].address, clientAddr);
  strcpy(list->clients[index].name, username);

  serverLog("INFO", "Client Accepted - Number of Clients: %d", list->numberOfClients);
  serverLog("DEBUG", "CLIENT INFO: %s -- %s", list->clients[index].name, list->clients[index].address);
  return index;
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
	// Replace the deleted object with last element
  int lastIndex = list->numberOfClients - 1;
  if(lastIndex != delIndex)
  {
    list->clients[delIndex] = list->clients[lastIndex];
  }

  // Remove the last client
  list->numberOfClients--;
  serverLog("INFO", "Client Removed - Number of Clients: %d", list->numberOfClients);

  // Check if server is empty
  if(list->numberOfClients == 0)
  {
    serverLog("INFO", "No more clients -- CLOSING SERVER");
    running = FALSE;
    kill(getpid(), SIGINT);   // Terminate server
  }
}
