#include "../inc/chat-client.h"

//Recommended to take in arguments for user and server
//To do, two threads. one for sending one for receiving
//splitting a message into two if over 40 characters

int main(int argc, char *argv[])
{
  int                my_server_socket, len, done;
  int                whichClient;
  struct sockaddr_in server_addr;
  struct hostent*    host;

  char clientIP[IP_SIZE] = "";
  getClientIP(clientIP);




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

  int numMessages = 0;

  char str[INPUT_MAX] = "";
  char input[INPUT_MAX] = "";

  wrefresh(subwindow);
  for(;;)
  {
    char time[19] = "";
    format_time(time);
    blankWin(subwindow);
    int ret = 0;
    mvwprintw(subwindow, 0,0, input, 80);
    placeCursor(&x, &y, subwindow, (int)strlen(input));

    ret = mvwgetnstr(subwindow, y, x, str, 80-strlen(input));
    if(ret == OK)
    {
      blankWin(subwindow);
      if(!strcmp(str, ">>bye<<"))
      {
        break;
      }
      strcat(input, str);
      char outgoingMsg[MESSAGE_SIZE] = "";
      sprintf(outgoingMsg, "%-15s [%-5s] >> %-40s (%s)", clientIP, argv[1], input, time);
      waddstr(messagesWindow, outgoingMsg);
      wrefresh(messagesWindow);
      input[0] = 0;


    }
    else if(ret == KEY_RESIZE)
    {
      strcat(input, str);

      resizeWindows(subwindow, subBackground, messagesWindow, messagesWindowBackground);
    }

  }

  getch();
  delwin(subBackground);
  delwin(messagesWindow);
  delwin(messagesWindowBackground);
  delwin(subwindow);

  endwin();
  return(0);


}
