#define _REENTRANT
#include "../inc/chatServer.h"


volatile bool running = true;

// DEBUG: Remove the debug flag in makefile
int main(void)
{
  const char* path = "./CWTServer.log";
  writeToLog("Test", path);

  int		status, *ptr_status;   // DEBUG: Do I need this for something?
  int serverSocket = -1;
  struct sockaddr_in serverAddr;
  pthread_t   tid[NUM_THREADS] = { 0 };

  //= Create socket
  if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    // Error getting the socket
    return 1;
  }

  //= Set up server configuration
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");      // DEBUG: Server config
  serverAddr.sin_port = htons(5000);

  //= Bind server config to Socket
  if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
  {
    // Cannot Bind to Socket
   close(serverSocket);
   return 2;
  }

  //= Set up Shared Mem and QUEUE // MESSAGE QUEUE;
  // Generate key
  key_t msgKey = ftok(KEY_PATH, 'G');
  int msgID = 0;

  if(msgKey == ID_ERROR)
  {
    return ID_ERROR;
  }

  // Creates or Grab message queue
  msgID = msgget(msgKey, 0);
  if(msgID == -1)
  {
    msgID = msgget(msgKey, (IPC_CREAT | 0660));
    if(msgID == -1)
    {
      return errno;
    }
  }

  // MASTER LIST;
  // Generattes key
  key_t shmKey = ftok(SHMKEY_PATH, SHM_KEYID);
  int shmID = 0;
  if(shmKey == ID_ERROR)
  {
    return ID_ERROR;
  }

  // Creates or Grabs shared memory
  MasterList* shList = NULL;
  shmID = shmget(shmKey, sizeof(MasterList), 0);
  if(shmID == -1)
  {
    shmID = shmget(shmKey, sizeof(MasterList), (IPC_CREAT | 0660));
    if(shmID == -1)
    {
      return errno;
    }
  }

  // Grab master list
  shList = (MasterList*)shmat (shmID, NULL, 0);       // Grabs the shared memory and
  shList->msgQueueID = msgID;                         // Assign the message queue ID
  shList->numberOfClients = 0;

  //= Listen for connections
  if (listen(serverSocket, 5) < 0)
  {
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
  pthread_create(&tid[0], NULL, broadcastMessages, (void *)shList);

  // Accepts client connection -> Spawns a thread for listenning for that client's message
  while(running && shList->numberOfClients < MAX_CLIENTS)
  {
    if((clientSocket = accept(serverSocket, (struct sockaddr*)&client, &clientLen)) < 0)
    {
      return -1;
    }

    cInfo.socket = clientSocket;
    shList->numberOfClients++;
    pthread_create(&tid[1], NULL, clientListenningThread, (void *)&cInfo);
    fflush(stdout);
    writeToLog("Client Accepted", path);
  }

  //= Finish Execution -> Join threads and clean memory
  // Join all threads
  for (int i = 0; i < MAX_CLIENTS; ++i)
  {
    pthread_join(tid[i], NULL);
  }

  // DEBUG: Message
  printf("-- Thread Done\n");

  // Clean resources
  msgctl (msgID, IPC_RMID, (struct msqid_ds*)NULL);
  shmdt(shList);
  shmctl(shmID, IPC_RMID, 0);

  return 0;
}
