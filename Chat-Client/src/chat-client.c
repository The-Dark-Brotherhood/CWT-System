#include "../inc/chat-client.h"

//Recommended to take in arguments for user and server

int main(int argc, char *argv[])
{
  int                my_server_socket, len, done;
  int                whichClient;
  struct sockaddr_in server_addr;
  struct hostent*    host;

  //https://stackoverflow.com/questions/4139405/how-can-i-get-to-know-the-ip-address-for-interfaces-in-c
  struct ifaddrs *ifap, *ifa;
  struct sockaddr_in *sa;
  char *addr;
  char clientIP[IP_SIZE] = "";
  getifaddrs (&ifap);
  for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
      if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET) {
          sa = (struct sockaddr_in *) ifa->ifa_addr;
          addr = inet_ntoa(sa->sin_addr);
          if(!strcmp(ifa->ifa_name, "ens33"))
          {
            strcpy(clientIP, addr);
          }
          //printf("Interface: %s\tAddress: %s\n", ifa->ifa_name, addr);
      }
  }

  freeifaddrs(ifap);
  printf("%s", clientIP);
  return 0;
  /* display result */
  //printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
  //strcat(clientIP, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
  if (argc != 3)
  {
    printf ("USAGE : chat-client <user> <server_name>\n");
    return 1;
  }

  if(strlen(argv[1]) > 5)
  {
    printf("User name length must be 5 characters max\n");
    return 2;
  }

  /*
   * determine host info for server name supplied
   */
  if ((host = gethostbyname(argv[2])) == NULL)
  {
    printf ("[CLIENT-%d] : Host Info Search - FAILED\n",whichClient);
    return 3;
  }

  memset (&server_addr, 0, sizeof (server_addr));
  server_addr.sin_family = AF_INET;
  memcpy (&server_addr.sin_addr, host->h_addr, host->h_length);
  server_addr.sin_port = htons (PORT);

  printf ("[CLIENT-%d] : Getting STREAM Socket to talk to SERVER\n", whichClient);
  fflush(stdout);
       if ((my_server_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0)
       {
         printf ("[CLIENT-%d] : Getting Client Socket - FAILED\n", whichClient);
         return 4;
       }



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
      waddstr(messagesWindow, input);
      waddstr(messagesWindow, "\n");
      wrefresh(messagesWindow);
      input[0] = 0;

    }
    else if(ret == KEY_RESIZE)
    {
      strcat(input, str);

      resizeWindows(subwindow, subBackground, messagesWindow, messagesWindowBackground);
    }
    char outgoingMsg[MESSAGE_SIZE] = "";
    //sprintf(outgoingMsg, "");
  }

  getch();
  delwin(subBackground);
  delwin(messagesWindow);
  delwin(messagesWindowBackground);
  delwin(subwindow);

  endwin();
  return(0);


}
