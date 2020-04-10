#define _REENTRANT
#include "../inc/chatServer.h"


volatile char running = TRUE;

// DEBUG: Remove the debug flag in makefile
int main(void)
{
  signal(SIGINT, closeServer);
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
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");      // DEBUG: Server config
  serverAddr.sin_port = htons(5000);

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
  printf("SHMID INSIDE %d\n", shmID); // DEBUG:
  // Grab master list
  shList = (MasterList*)shmat (shmID, NULL, 0);       // Grabs the shared memory and
  shList->msgQueueID = msgID;                         // Assign the message queue ID
  shList->numberOfClients = 0;

  //= Listen for connections
  if(listen(serverSocket, 5) < 0)
  {
   serverLog("ERROR", "Could not open server for connetions");
   close(serverSocket);
   return 3;
  }

  //DEBUG: = Accept client
  int clientSocket = -1;
  clientInfo cInfo = {msgID, clientSocket, shList};

  struct sockaddr_in client;
  int clientLen = sizeof(client);
  int numClient = 0;

  //= Iniciate broacasting thread
  pthread_create(&shList->tid[0], NULL, broadcastMessages, (void *)shList);
  serverLog("INFO", "Server Started Listening & Sending");

  // Accepts client connectionServer -> Spawns a thread for listenning for that client's message
  while(shList->numberOfClients <= MAX_CLIENTS)
  {
    if((clientSocket = accept(serverSocket, (struct sockaddr*)&client, &clientLen)) < 0)
    {
      serverLog("ERROR", "Unexpected: Failed to accept client - CLOSING");
      // DEBUG: Put the signal to clean up the memory
      return -7;
    }

    // Spawn thread for listenning for clients messages
    cInfo.socket = clientSocket;
    pthread_create(&shList->tid[1], NULL, clientListenningThread, (void *)&cInfo);
    fflush(stdout);
  }

  return 0;
}

void closeServer(int signal_number)
{
  printf("Running in the close: %d\n", running  );
  key_t shmKey = ftok(SHMKEY_PATH, SHM_KEYID);
  if(shmKey == -1)
  {
    serverLog("ERROR", "Could not get SHList Key to clean memory");
  }
  MasterList* shList = NULL;
  int shmID = shmget(shmKey, sizeof(MasterList), 0);
  printf("SHMID INSIDE %d\n", shmID);
  if(shmID != -1)
  {
    MasterList* list = (MasterList*)shmat(shmID, NULL, SHM_RDONLY);
    printf("--> %p (Closing)\n", list);

    // Join all threads
    for (int counter = 0; counter < NUM_THREADS; counter++)
    {
      pthread_join(list->tid[counter], NULL);
    }

    // Clean resources
    msgctl(list->msgQueueID, IPC_RMID, (struct msqid_ds*)NULL);
    shmdt(list);
    shmctl(shmID, IPC_RMID, 0);
    serverLog("DEBUG", "Server Closed");
  }
  else
  {
    serverLog("ERROR", "Could not retrieve master list for clean up");
  }

  // Terminate
	exit(1);
}
