#include "../inc/chat-client.h"

void getClientIP(char* clientIP)
{

  //https://stackoverflow.com/questions/4139405/how-can-i-get-to-know-the-ip-address-for-interfaces-in-c
  struct ifaddrs *ifap, *ifa;
  struct sockaddr_in *sa;
  char *addr;
  getifaddrs (&ifap);
  for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
      if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET) {
          sa = (struct sockaddr_in *) ifa->ifa_addr;
          addr = inet_ntoa(sa->sin_addr);
          if(!strcmp(ifa->ifa_name, "ens33"))
          {
            strcpy(clientIP, addr);
          }
      }
  }
  freeifaddrs(ifap);
}

//https://stackoverflow.com/questions/5141960/get-the-current-time-in-c
void format_time(char *output)
{
  struct timeval  now;
  struct tm*      local;

  gettimeofday(&now, NULL);
  local = localtime(&now.tv_sec);

  sprintf(output, "%02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);

}


//still need to add in the address to send to
void* sendMessage(void* args)
{
  int x,y;
  getmaxyx(stdscr,y,x);
  Windows *windows = (Windows*)args;
  char clientIP[IP_SIZE] = "";
  getClientIP(clientIP);
  char str[INPUT_MAX] = "";
  char input[INPUT_MAX] = "";
  char time[19] = "";

  while(1)
  {
    format_time(time);
    blankWin(windows->outgoingWindow);
    int ret = 0;
    mvwprintw(windows->outgoingWindow, 0,0, input, 80);
    placeCursor(&x, &y, windows->outgoingWindow, (int)strlen(input));

    ret = mvwgetnstr(windows->outgoingWindow, y, x, str, 80-strlen(input));

    if(ret == OK)
    {
      blankWin(windows->outgoingWindow);

      strcat(input, str);
      char outgoingMsg[MESSAGE_SIZE] = "";
      sprintf(outgoingMsg, "%-15s [%-5s] >> %-40s (%s)\n", clientIP, windows->userName, input, time);
      waddstr(windows->incomingWindow, outgoingMsg);
      wrefresh(windows->incomingWindow);
      char buffer[2048] = {};
      strcpy(buffer, outgoingMsg);
      send(sockfd, buffer, strlen(buffer), 0);
      if(!strcmp(str, ">>bye<<"))
      {
        break;
      }
      input[0] = 0;
    }
    else if(ret == KEY_RESIZE)
    {
      strcat(input, str);

      resizeWindows(windows->outgoingWindow, windows->outgoingBckgrnd, windows->incomingWindow, windows->incomingBckgrnd);
    }
    else
    {
      waddstr(windows->outgoingWindow, "Hmmmmm\n");
    }
  }
    return NULL;
}

void* recv_msg_handler(void* pargs)
{
  char message[85] = {};
  while (1)
  {
    int receive = recv(sockfd, message, 85, 0);
    if (receive > 0)
    {
      waddstr((WINDOW*)pargs, message);
      wrefresh((WINDOW*)pargs);
    }
    else if (receive == 0)
    {
      //break;
    }
    else
    {
      // -1
    }
    memset(message, 0, sizeof(message));
  }
  return NULL;
}
