#include "../inc/chat-client.h"

//Recommended to take in arguments for user and server
//splitting a message into two if over 40 characters

int clientRunning = 1;

//DEBUG: change to pass into function
int sockfd = 0;

int main(int argc, char *argv[])
{
  int                my_server_socket, len, done;
  int                whichClient;
  struct hostent*    host;

  //   if (argc != 3)
  // {
  //   printf ("USAGE : chat-client <user> <server_name>\n");
  //   return 1;
  // }
  //
  // if(strlen(argv[1]) > 5)
  // {
  //   printf("User name length must be 5 characters max\n");
  //   return 2;
  // }
  //
  // /*
  //  * determine host info for server name supplied
  //  */
  // if ((host = gethostbyname(argv[2])) == NULL)
  // {
  //   printf ("[CLIENT-%d] : Host Info Search - FAILED\n",whichClient);
  //   return 3;
  // }
  //
  // memset (&server_addr, 0, sizeof (server_addr));
  // server_addr.sin_family = AF_INET;
  // memcpy (&server_addr.sin_addr, host->h_addr, host->h_length);
  // server_addr.sin_port = htons (PORT);
  //
  // printf ("[CLIENT-%d] : Getting STREAM Socket to talk to SERVER\n", whichClient);
  // fflush(stdout);
  //      if ((my_server_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0)
  //      {
  //        printf ("[CLIENT-%d] : Getting Client Socket - FAILED\n", whichClient);
  //        return 4;
  //      }


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
  WINDOW *msgWinBackground = newwin(y-(y/5+BCKGRND_PADDING_OFFSET), x, 0, BCKGRND_STARTING_X);
  WINDOW *msgWin = newwin(INCOMING_MSG_WINDOW_HEIGHT, x-INNER_WINDOW_PADDING, 2, INNER_WINDOW_STARTING_X);
  scrollok(msgWin, TRUE);
  WINDOW *txtBoxBackground = newwin(y/TXT_BOX_HEIGHT_RATIO+BCKGRND_PADDING_OFFSET,x,(y-(y/TXT_BOX_HEIGHT_RATIO+BCKGRND_PADDING_OFFSET)),BCKGRND_STARTING_X);
  WINDOW *txtBoxWin = newwin(y/TXT_BOX_HEIGHT_RATIO,x-INNER_WINDOW_PADDING,(y-(y/TXT_BOX_HEIGHT_RATIO+1)),INNER_WINDOW_STARTING_X);

  //Set the colors, draw the boxes, write the box titles
  setUpWindows(txtBoxWin, txtBoxBackground, msgWin, msgWinBackground);

  //Connect to the server
  struct sockaddr_in server_addr;

  //Socket settings
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_addr.sin_port = htons(5567);


  //Connect to Server
  int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (err == -1)
  {
    printf("Error: unable to connect\n");
    return -1;
  }

  //DEBUGGGGGG
  send(sockfd, "You're in\n", 32, 0);

  //Create the thread responsible for receiving messages from the server
  pthread_t recv_msg_thread;
  if(pthread_create(&recv_msg_thread, NULL, (void *) recv_msg_handler, (void*)msgWin) != 0)
  {
    printf("Error creating the receive messages thread\n");
    return EXIT_FAILURE;
  }

  //the following struct will be passed as an argument to the message-sending thread.
  Windows windows = {
    .outgoingWindow = txtBoxWin,
    .outgoingBckgrnd = txtBoxBackground,
    .incomingWindow = msgWin,
    .incomingBckgrnd = msgWinBackground,
    .userName = argv[1]
  };

  //Create the thread responsible for sending messages to the server, passing struct as arg
  pthread_t send_msg_thread;
  if(pthread_create(&send_msg_thread, NULL, (void *) sendMessage, (void*)&windows) != 0)
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
  return(0);


}
