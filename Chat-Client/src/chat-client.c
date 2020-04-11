#include "../inc/chat-client.h"

//Recommended to take in arguments for user and server

int clientRunning = 1;

//DEBUG: change to pass into function

int main(int argc, char *argv[])
{
  struct sockaddr_in server_address;
  struct hostent* host;
  int my_server_socket;
  int wrongUsage = 0;

  if (argc != 3)
  {
    wrongUsage = 1;
  }
  else if(strncmp("-user", argv[1], USER_FLAG_LENGTH))
  {
    wrongUsage = 1;
  }
  else if(strncmp("-server", argv[2], SERVER_FLAG_LENGTH))
  {
    wrongUsage = 1;
  }



  if(wrongUsage)
  {
    printf ("USAGE : chat-client -user<userID> -server<server name>\n");
    return 1;
  }
  if(strlen(argv[1]+USER_FLAG_LENGTH) > NAME_SIZE || strlen(argv[1]+USER_FLAG_LENGTH) < 1)
  {
    printf("User name length must be 1-5 characters max\n");
    return 2;
  }

  //Extract the username
  char userID[NAME_SIZE+1] = "";
  strcpy(userID, (argv[1]+USER_FLAG_LENGTH));

  /*
   * determine host info for server name supplied
   */
  if ((host = gethostbyname(argv[2]+SERVER_FLAG_LENGTH)) == NULL)
  {
    printf ("Failed to get host by name\n");
    return 3;
  }

  int sockfd = 0;

  if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
  {
   printf ("Failed to get socket\n");
   return 4;
  }

  //Connect to the server

  //Socket settings
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  char *ServerIP = inet_ntoa(*((struct in_addr*)host->h_addr_list[0]));

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(ServerIP);
  server_address.sin_port = htons(5568);


  //Connect to Server
  int err = connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));
  if (err == -1)
  {
    printf("Error: unable to connect\n");
    return -1;
  }
  // Send the message with client info
  sendFirstMsg(sockfd, userID);
  //Create the ncurses windows.
  initscr();
  cbreak();
  refresh();

  //get the x,y of terminal window in order
  //to set proper width/height dimension of windows
  int x,y;
  getmaxyx(stdscr,y,x);

  //initialize and set the dimensions of windows
  //UI consists of 4 windows -2 inner windows which
  //hold the outgoing and incoming messages
  // -2 windows are the background for the incoming
  // and outgoing messages
  /*
   _____________________________________________
  |  _________________________________________  |
  | |                                         | |
  | |                                         | |
  | |         incoming messages               | |
  | |         (height limited to 10           | |
  | |          messages - for scrolling       | |
  | |                                         | |
  | |                                         | |
  | |_________________________________________| |
  |_____________________________________________|
  |  _________________________________________  |
  | |                                         | |
  | |            outgoing messages            | |
  | |                                         | |
  | |_________________________________________| |
  |_____________________________________________|

  */

  //Window dimensions are set using % in order to add some responsivity
  //to the application
  WINDOW *msgWinBackground = newwin(0,0,0,0);
  WINDOW *msgWin = newwin(0,0,0,0);
  scrollok(msgWin, TRUE);
  WINDOW *txtBoxBackground = newwin(0,0,0,0);
  WINDOW *txtBoxWin = newwin(0,0,0,0);

  //Set the windows height/width and x,y placement
  resizeWindows(txtBoxWin, txtBoxBackground, msgWin, msgWinBackground);
  //Set the colors, draw the boxes, write the box titles
  setUpWindows(txtBoxWin, txtBoxBackground, msgWin, msgWinBackground);

  //the following struct will be passed as an argument to the rmessage-sendingreceiving threads.
  SendThreadArgs args = {
    .outgoingWindow = txtBoxWin,
    .outgoingBckgrnd = txtBoxBackground,
    .incomingWindow = msgWin,
    .incomingBckgrnd = msgWinBackground,
    .userName = userID,
    .socket = sockfd
  };

  //Create the thread responsible for receiving messages from the server
  pthread_t recv_msg_thread;
  if(pthread_create(&recv_msg_thread, NULL, (void *) receiveMsg, (void*)&args) != 0)
  {
    printf("Error creating the receive messages thread\n");
    return EXIT_FAILURE;
  }

  //Create the thread responsible for sending messages to the server, passing struct as arg
  pthread_t send_msg_thread;
  if(pthread_create(&send_msg_thread, NULL, (void *) sendMessage, (void*)&args) != 0)
  {
    printf("Error creating the send messages thread\n");
    return EXIT_FAILURE;
  }
  //wait for msg thread to end before exiting
  pthread_join(send_msg_thread, NULL);

  //close socket
  close(sockfd);

  //delete the windows
  delwin(txtBoxBackground);
  delwin(msgWin);
  delwin(msgWinBackground);
  delwin(txtBoxWin);
  endwin();
  return 0;

}
