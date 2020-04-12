/*
*  FILE          : server.c
*  PROJECT       : Assignment #4
*  PROGRAMMER    : Gabriel Gurgel & Michael Gordon
*  FIRST VERSION : 2020-04-06
*  DESCRIPTION   : Main process loop for the server. Parses command arguments,
*                  iniciates threads, creates message queue and share memory,
*                  and handles the closing of the server
* REFERNCES:
*   Hoochamacalit -> Assignment #3 - by Gabriel Gurgel and Michael Gordon
*/
#define _REENTRANT
#include "../inc/chatServer.h"


volatile char running = TRUE;
pthread_mutex_t lock;

int main(int argc, char const *argv[])
{
  // Set up interrupt handler and config info
  signal(SIGINT, closeServer);
  char ipAddress[IP_SIZE] = { 0 };
  getHostIp(ipAddress);
  int port = 0;
  if (pthread_mutex_init(&lock, NULL) != 0)
  {
    serverLog("ERROR", "Failed to Initialize mutex");
    return -1;
  }

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

  //= Create server socket
  int serverSocket = -1;
  struct sockaddr_in serverAddr;
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

  // Grab master list and Initializes all elements
  shList = (MasterList*)shmat (shmID, NULL, 0);
  shList->msgQueueID = msgID;
  shList->numberOfClients = 0;
  Client blankClient = { -1 , 0, { 0 }, { 0 } };
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

  // Prepare new client info
  int clientSocket = -1;
  clientInfo cInfo = { -1, shList };
  struct sockaddr_in client;
  int clientLen = sizeof(client);

  //= Iniciate broacasting thread
  pthread_create(&(shList->broadcastTid), NULL, broadcastMessages, (void *)shList);
  serverLog("INFO", "Server Started Listening & Sending");

  // Main processing: Accept new clients and spawns thread for listening
  while(running)
  {
    if((clientSocket = accept(serverSocket, (struct sockaddr*)&client, &clientLen)) < 0)
    {
      serverLog("ERROR", "Unexpected: Failed to accept client - CLOSING");
      kill(getpid(), SIGINT);
    }

    // Rejects if the client is full
    if(shList->numberOfClients == MAX_CLIENTS)
    {
      write(clientSocket, EXIT_MSG, strlen(EXIT_MSG) + 1);
      continue;
    }

    // Read new client info and add into the client list
    message firstMsg;
    memset(firstMsg.content, 0, MSG_SIZE);
    read(clientSocket, firstMsg.content, MSG_SIZE);
    int index = addClient(shList, &firstMsg, clientSocket);
    fflush(stdout);

    // Spawn thread for listenning for clients messages
    cInfo.index = index;
    pthread_create(&(shList->clients[index]).tid, NULL, clientListenningThread, (void *)&cInfo);
    fflush(stdout);
  }

  return 0;
}

// FUNCTION      : closeServer
// DESCRIPTION   : Interrupt handler for SIGNIT
//                 Is activated when the number of clients reach 0
//                 Peforms clean up of the resources
//
// PARAMETERS    :
//	int signal_number  : Signal received (SIGINIT)
//
// RETURNS       :
//	void
void closeServer(int signal_number)
{
  running = FALSE;

  // Grab the shared memory section for clean up
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

    // Warn all the clients the server is closed and join threads
    serverShutdownSignal(list);
    for(int counter = 0; counter < list->numberOfClients; counter++)
    {
      pthread_join(list->broadcastTid, NULL);
    }
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
