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

void* sendMessage(void* arg)
{
  int x = 0;
  int y = 0;;

  getmaxyx(stdscr,y,x);
  SendThreadArgs *data = (SendThreadArgs*)arg;
  char clientIP[IP_SIZE] = "";
  getClientIP(clientIP);
  char str[INPUT_MAX+1] = "";
  char input[INPUT_MAX+1] = "";
  char time[TIME_SIZE] = "";
  int addNewLine = 0;
  while(clientRunning)
  {
    blankWin(data->outgoingWindow);
    int ret = 0;
    mvwprintw(data->outgoingWindow, 0,0, input, INPUT_MAX);
    placeCursor(&x, &y, data->outgoingWindow, (int)strlen(input));
    ret = mvwgetnstr(data->outgoingWindow, y, x, str, INPUT_MAX-strlen(input));

    if(ret == OK)
    {
      format_time(time);
      blankWin(data->outgoingWindow);

      strcat(input, str);
      if(strlen(input) > 0)
      {
        char outgoingMsg[MESSAGE_SIZE] = "";
        char* ps = input;
        if(strlen(input) > MAX_CHAR_PER_MSG)
        {
          int index = splitMessage(input);
          char firstMsg[MAX_CHAR_PER_MSG+1] = "";
          strncpy(firstMsg, input, index);
          sprintf(outgoingMsg, "%-15s [%-5s] >> %-40s (%s)", clientIP, data->userName, firstMsg, time);
          waddstr(data->incomingWindow, outgoingMsg);

          if (checkIfNeedNewLine(data->outgoingWindow))
          {
            waddstr(data->incomingWindow,"\n");
          }
          wrefresh(data->incomingWindow);
          //DEBUG
          char buffer[2048] = {};
          strcpy(buffer, outgoingMsg);
          send(data->socket, buffer, strlen(buffer), 0);
          ps += index;
        }
        sprintf(outgoingMsg, "%-15s [%-5s] >> %-40s (%s)", clientIP, data->userName, ps, time);

        waddstr(data->incomingWindow, outgoingMsg);
        if (checkIfNeedNewLine(data->outgoingWindow))
        {
          waddstr(data->incomingWindow,"\n");
        }
        wrefresh(data->incomingWindow);

        char buffer[2048] = {};
        strcpy(buffer, outgoingMsg);
        send(data->socket, buffer, strlen(buffer), 0);

        if(!strcmp(str, EXIT_MSG))
        {
          if(clientRunning)
          {
            data->exitStatus = USER_SHUTDOWN;
          }
          clientRunning = 0;
          break;
        }
        input[0] = 0;
      }
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
  char message[INPUT_MAX+1] = {};
  while (clientRunning)
  {
    int receive = recv(data->socket, message, INPUT_MAX+1, 0);
    if (receive > 0)
    {
      if(!strcmp(message, EXIT_MSG))
      {
        data->exitStatus = SERVER_SHUTDOWN;
        clientRunning = 0;
      }
      if(clientRunning)
      {
        waddstr(data->incomingWindow, message);
        if (checkIfNeedNewLine(data->incomingWindow))
        {
          waddstr(data->incomingWindow, "\n");
        }
          wrefresh(data->incomingWindow);
      }
    }
    else if (receive == 0)
    {
      if(clientRunning)
      {
      data->exitStatus = NO_SERVER_RESPONSE;
      }
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
  int middle = MAX_CHAR_PER_MSG-1;
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
  if ((length - retValue) > MAX_CHAR_PER_MSG)
  {
  	retValue = middle;;
  }
  //offset index, to return number of bytes to copy in
  return retValue + 1;
}

void sendFirstMsg(int server, char* user)
{
  char input[INPUT_MAX+1] = "";
  char time[TIME_SIZE] = "";
  char clientIP[IP_SIZE] = "";
  getClientIP(clientIP);
  format_time(time);
  sprintf(input, "%s [%s]", clientIP, user);
  send(server, input , strlen(input), 0);
}
