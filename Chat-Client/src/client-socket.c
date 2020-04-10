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
void* sendMessage(void* arg)
{
  int x,y;
  int windowX, windowY;
  getmaxyx(stdscr,y,x);
  SendThreadArgs *data = (SendThreadArgs*)arg;
  char clientIP[IP_SIZE] = "";
  getClientIP(clientIP);
  char str[INPUT_MAX] = "";
  char input[INPUT_MAX] = "";
  char time[19] = "";
  char message[85] = {};
  int addNewLine = 0;
  while(clientRunning)
  {
    getmaxyx(data->outgoingWindow, windowY, windowX);
    if(windowX > 40)
    {
      addNewLine = 1;
    }

    format_time(time);
    blankWin(data->outgoingWindow);
    int ret = 0;
    mvwprintw(data->outgoingWindow, 0,0, input, 80);
    placeCursor(&x, &y, data->outgoingWindow, (int)strlen(input));

    ret = mvwgetnstr(data->outgoingWindow, y, x, str, 80-strlen(input));

    if(ret == OK)
    {
      blankWin(data->outgoingWindow);

      strcat(input, str);
      char outgoingMsg[MESSAGE_SIZE] = "";
      char* ps = input;
      if(strlen(input) > 40)
      {
        int index = splitMessage(input);
        char firstMsg[41] = "";
        strncpy(firstMsg, input, index);
        sprintf(outgoingMsg, "%-15s [%-5s] >> %-40s (%s)", clientIP, data->userName, firstMsg, time);

        waddstr(data->incomingWindow, outgoingMsg);
        if(addNewLine)
        {
            waddstr(data->incomingWindow,"\n");
        }
        wrefresh(data->incomingWindow);
        char buffer[2048] = {};
        strcpy(buffer, outgoingMsg);
        send(data->socket, buffer, strlen(buffer), 0);
        ps += index;
      }
      sprintf(outgoingMsg, "%-15s [%-5s] >> %-40s (%s)", clientIP, data->userName, ps, time);

      waddstr(data->incomingWindow, outgoingMsg);
      if(addNewLine)
      {
          waddstr(data->incomingWindow,"\n");
      }
      wrefresh(data->incomingWindow);
      char buffer[2048] = {};
      strcpy(buffer, outgoingMsg);
      send(data->socket, buffer, strlen(buffer), 0);

      if(!strcmp(str, ">>bye<<"))
      {
        clientRunning = 0;
        break;
      }
      input[0] = 0;
    }
    else if(ret == KEY_RESIZE)
    {
      strcat(input, str);
      resizeWindows(data->outgoingWindow, data->outgoingBckgrnd, data->incomingWindow, data->incomingBckgrnd);
    }
  }
    return NULL;
}

void* receiveMsg(void* arg)
{

  //need to have a check for the server is running
  SendThreadArgs *data = (SendThreadArgs*)arg;
  char message[85] = {};
  while (clientRunning)
  {
    int receive = recv(data->socket, message, 85, 0);
    if (receive > 0)
    {
      if(clientRunning)
      {
        waddstr(data->incomingWindow, message);
        wrefresh(data->incomingWindow);
      }
    }
    else if (receive == 0)
    {
      clientRunning = 0;
    }
    memset(message, 0, sizeof(message));
  }
  return NULL;
}


//assuming the message length is over 40
int splitMessage(char* message)
{
  int retValue = 0;
  int length = strlen(message);
  int middle = 40;
  int distanceRight = 0;
  int i = middle;

  int distanceLeft = 0;

  for (int i = middle; i > 0; i--)
  {
  	if (message[i] == ' ')
  	{
  		break;
  	}
  	distanceLeft++;
   }


  retValue = middle - distanceLeft;
  if ((length - retValue) > 40)
  {
  	retValue = middle;;
  }

  //offset index, to return number of bytes to copy in
  return retValue + 1;
}
