#include "../inc/chat-client.h"

//Recommended to take in arguments for user and server
//To do, two threads. one for sending one for receiving
//splitting a message into two if over 40 characters

//for threads to know if should exit right away
//should have thread that spawns threads for listening?
int clientRunning = 1;
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



  initscr();
  cbreak();
  //signal (SIGWINCH, NULL);
  refresh();
  int x,y;
  getmaxyx(stdscr,y,x);
  WINDOW *messagesWindowBackground = newwin(y-(y/5+3), x, 0, 0);
  WINDOW *messagesWindow = newwin(11, x-2, 2, 1);
  scrollok(messagesWindow, TRUE);
  WINDOW *subBackground = newwin(y/5+3,x,(y-(y/5+3)),0);
  WINDOW *subwindow = newwin(y/5,x-2,(y-(y/5+1)),1);

  setUpWindows(subwindow, subBackground, messagesWindow, messagesWindowBackground);

  //Should color change when new message received?


  struct sockaddr_in server_addr;

  /* Socket settings */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_addr.sin_port = htons(5567);


  // Connect to Server
  int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (err == -1)
  {
    printf("ERROR: connect\n");
    return -1;
  }

  // Send name



  // pthread_t send_msg_thread;
  // if(pthread_create(&send_msg_thread, NULL, (void *) send_msg_handler, NULL) != 0)
  // {
  //   printf("ERROR: pthread\n");
  //   return EXIT_FAILURE;
  // }
  send(sockfd, "You're in\n", 32, 0);
  pthread_t recv_msg_thread;
  if(pthread_create(&recv_msg_thread, NULL, (void *) recv_msg_handler, (void*)messagesWindow) != 0)
  {
    printf("ERROR: pthread\n");
    return EXIT_FAILURE;
  }
  Windows windows = {
    .outgoingWindow = subwindow,
    .outgoingBckgrnd = subBackground,
    .incomingWindow = messagesWindow,
    .incomingBckgrnd = messagesWindowBackground,
    .userName = argv[1]
  };
sendMessage((void*)&windows);
  // pthread_t send_msg_thread;
  // if(pthread_create(&send_msg_thread, NULL, (void *) sendMessage, (void*)&windows) != 0)
  // {
  //   printf("ERROR: pthread\n");
  //   return EXIT_FAILURE;
  // }


  // char str[INPUT_MAX] = "";
  // char input[INPUT_MAX] = "";
  //
  // for(;;)
  // {
  //   char time[19] = "";
  //   format_time(time);
  //   blankWin(subwindow);
  //   int ret = 0;
  //   mvwprintw(subwindow, 0,0, input, 80);
  //   placeCursor(&x, &y, subwindow, (int)strlen(input));
  //
  //   ret = mvwgetnstr(subwindow, y, x, str, 80-strlen(input));
  //
  //   if(ret == OK)
  //   {
  //     blankWin(subwindow);
  //     if(!strcmp(str, ">>bye<<"))
  //     {
  //       break;
  //     }
  //     strcat(input, str);
  //     char outgoingMsg[MESSAGE_SIZE] = "";
  //     sprintf(outgoingMsg, "%-15s [%-5s] >> %-40s (%s)\n", clientIP, argv[1], input, time);
  //     waddstr(messagesWindow, outgoingMsg);
  //     wrefresh(messagesWindow);
  //
  //     pthread_t tid;
  //     if( pthread_create(&tid, NULL, sendMessage, (void *)outgoingMsg) < 0)
  //     {
  //         perror("could not create thread");
  //         //return 1;
  //     }
  //     input[0] = 0;
  //
  //
  //   }
  //   else if(ret == KEY_RESIZE)
  //   {
  //     strcat(input, str);
  //
  //     resizeWindows(subwindow, subBackground, messagesWindow, messagesWindowBackground);
  //   }
  //
  // }
  while (clientRunning){
    sleep(1);
    }
  close(sockfd);
  getch();
  delwin(subBackground);
  delwin(messagesWindow);
  delwin(messagesWindowBackground);
  delwin(subwindow);

  endwin();
  return(0);


}
