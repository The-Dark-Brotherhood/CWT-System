/*
*  FILE          : server.c
*  PROJECT       : Assignment #4
*  PROGRAMMER    : Gabriel Gurgel & Michael Gordon
*  FIRST VERSION : 2020-04-06
*  DESCRIPTION   : Main process loop for the server. Parses command arguments,
*                  iniciates threads, creates message queue and share memory,
*                  and handles the closing of the server
*/
#define _REENTRANT
#include "../inc/chatServer.h"


volatile char running = TRUE;
pthread_mutex_t lock;

// DEBUG: Remove the debug flag in makefile
int main(int argc, char const *argv[])
{
  signal(SIGINT, closeServer);
  char ipAddress[IP_SIZE] = { 0 };
  getHostIp(ipAddress);
  int port = 0;

  if (pthread_mutex_init(&lock, NULL) != 0)
  {
    serverLog("ERROR", "Failed to Initialize mutex");
    return -1;
  }

  printf("-> %s\n", ipAddress );

  // Handle arguments
  if(argc != 2)
  {
    printf("usage: -port<port>\n");
    return -1;
  }
  else if(strncmp(argv[1], PORT_SWITCH, PORT_SWITCH_SIZE) == 0)
  {
    port = (int)strtol(argv[1] + PORT_SWITCH_SIZE, NULL, 10);
    if(port == 0)
    {
      printf("Not valid port\n");
    }
  }


  int serverSocket = -1;
  struct sockaddr_in serverAddr;

  //= Create socket
  if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    serverLog("ERROR", "Could not create server socket");
    return -1;
  }

  //= Set up server configuration
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = inet_addr(ipAddress);      // DEBUG: Server config
  serverAddr.sin_port = htons(port);

  //= Bind server config to Socket
  if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
  {
    serverLog("ERROR", "Could not config server");
    close(serverSocket);
    return -2;
  }

  //= Set up Shared Mem and QUEUE // MESSAGE QUEUE;
  // Generate key
  key_t msgKey = ftok(KEY_PATH, 'G');
  int msgID = 0;

  if(msgKey == -1)
  {
    serverLog("ERROR", "Could generate MSG_KEY");
    return -3;
  }

  // Creates or Grab message queue
  msgID = msgget(msgKey, 0);
  if(msgID == -1)
  {
    msgID = msgget(msgKey, (IPC_CREAT | 0660));
    if(msgID == -1)
    {
      serverLog("ERROR", "Could not create or get MSG Queue");
      return -4;
    }
  }

  // MASTER LIST;
  // Generate key
  key_t shmKey = ftok(SHMKEY_PATH, SHM_KEYID);
  int shmID = 0;
  if(shmKey == -1)
  {
    serverLog("ERROR", "Could not get SHList Key");
    return -5;
  }

  // Creates or Grabs shared memory
  MasterList* shList = NULL;
  shmID = shmget(shmKey, sizeof(MasterList), 0);
  if(shmID == -1)
  {
    shmID = shmget(shmKey, sizeof(MasterList), (IPC_CREAT | 0660));
    if(shmID == -1)
    {
      serverLog("ERROR", "Could not create SHList");
      return -6;
    }
  }

  // Grab master list
  shList = (MasterList*)shmat (shmID, NULL, 0);       // Grabs the shared memory and
  shList->msgQueueID = msgID;                         // Assign the message queue ID
  shList->numberOfClients = 0;
  Client blankClient = {-1 , -1, { -1 }, { -1 }};         // Initialize clients to the same value
  for(int counter = 0; counter < MAX_CLIENTS; counter++)
  {
    shList->clients[counter] = blankClient;
  }

  //= Listen for connections
  if(listen(serverSocket, 5) < 0)
  {
   serverLog("ERROR", "Could not open server for connetions");
   close(serverSocket);
   return 3;
  }

  //DEBUG: = Accept client
  int clientSocket = -1;
  clientInfo cInfo = { -1, shList };

  struct sockaddr_in client;
  int clientLen = sizeof(client);
  int numClient = 0;

  //= Iniciate broacasting thread
  pthread_create(&(shList->broadcastTid), NULL, broadcastMessages, (void *)shList);
  serverLog("INFO", "Server Started Listening & Sending");

  // Accepts client connectionServer -> Spawns a thread for listenning for that client's message
  while(running)
  {
    if((clientSocket = accept(serverSocket, (struct sockaddr*)&client, &clientLen)) < 0)
    {
      serverLog("ERROR", "Unexpected: Failed to accept client - CLOSING");
      // DEBUG: Put the signal to clean up the memory
      return -7;
    }

    if(shList->numberOfClients == MAX_CLIENTS)
    {
      write(clientSocket, EXIT_MSG, strlen(EXIT_MSG) + 1);
      continue;
    }

    // Read new client info and add into the client list
    message firstMsg;
    memset(firstMsg.content, 0, MSG_SIZE);
    read(clientSocket, firstMsg.content, MSG_SIZE);
    fflush(stdout);

    // Spawn thread for listenning for clients messages

    int index = addClient(shList, &firstMsg, clientSocket);
    printf("It is still --> %d\n", index );

    cInfo.index = index;
    printf("Did Cinfo lost it --> %d\n", cInfo.index );
    pthread_create(&(shList->clients[index]).tid, NULL, clientListenningThread, (void *)&cInfo);
    fflush(stdout);
  }

  // DEBUG: If number of clients reach 10 does this get executed?
  return 0;
}

void closeServer(int signal_number)
{
  running = FALSE;
  key_t shmKey = ftok(SHMKEY_PATH, SHM_KEYID);
  if(shmKey == -1)
  {
    serverLog("ERROR", "Could not get SHList Key to clean memory");
    exit(-1);
  }
  int shmID = shmget(shmKey, sizeof(MasterList), 0);
  if(shmID != -1)
  {
    MasterList* list = (MasterList*)shmat(shmID, NULL, SHM_RDONLY);
    serverShutdownSignal(list);
    pthread_join(list->broadcastTid, NULL);

    // Clean resources
    msgctl(list->msgQueueID, IPC_RMID, (struct msqid_ds*)NULL);
    shmdt(list);
    shmctl(shmID, IPC_RMID, 0);
    pthread_mutex_destroy(&lock);
    serverLog("DEBUG", "Server Closed");
  }
  else
  {
    serverLog("ERROR", "Could not retrieve master list for clean up");
  }

  // Terminate
	exit(0);
}
