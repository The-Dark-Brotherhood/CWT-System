#include "../inc/chatServer.h"

// DEBUG: Remove the debug flag in makefile
int main(void)
{
  int serverSocket = -1;
  struct sockaddr_in serverAddr;

  //= Create socket
  if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    // Error getting the socket
    return 1;
  }

  //= Set up server configuration
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverAddr.sin_port = htons(5000);

  //= Bind server config to Socket
  if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
  {
    // Cannot Bind to Socket
   close(serverSocket);
   return 2;
  }

  //= Listen for connections
  if (listen(serverSocket, 5) < 0)
  {
   // Cannot listen lost year
   close(serverSocket);
   return 3;
  }

  //DEBUG: = Accept client
  int clientSocket = -1;
  struct sockaddr_in client;
  int clientLen = sizeof(client);
  clientSocket = accept(serverSocket, (struct sockaddr*)&client, &clientLen);
  printf("Worked!\n");

  return 0;
}
