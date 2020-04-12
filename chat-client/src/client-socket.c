/*
*  FILE          : client-socket.c
*  PROJECT       : Assignment #4 - Can We Talk?
*  PROGRAMMER    : Gabriel Gurgel, Michael Gordon
*  FIRST VERSION : 2020-03-27
*  DESCRIPTION   : this file contains functions that have to do with socket communication
*                   with the server. The sending and receiving from the server is done by
*                   using two separate threads.
*/
#include "../inc/chat-client.h"


// FUNCTION      : getClientIP
// DESCRIPTION   : Get the IP of the client application
// source: //https://stackoverflow.com/questions/4139405/how-can-i-get-to-know-the-ip-address-for-interfaces-in-c
// PARAMETERS    :
//  char* clientIP :the ip will be stored in this destination
//
// RETURNS       :
//	void
void getClientIP(char* clientIP)
{

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

// FUNCTION      : formatTime
// DESCRIPTION   : Get the current time and format at in the specified way: HH:MM:SS
//
// PARAMETERS    :
//  char* ouput :the formatted time will be stored here
//
// RETURNS       :
//	void
void formatTime(char *output)
{
  struct timeval  now;
  struct tm*      local;

  gettimeofday(&now, NULL);
  local = localtime(&now.tv_sec);

  sprintf(output, "%02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);

}

// FUNCTION      : sendMessage
// DESCRIPTION   : This is the function that is used by the thread that sends
//                 messages to the chat server
//
// PARAMETERS    :
//  void* arg -> pointer to the struct holding arguments for the thread
//
// RETURNS       :
//	void
void* sendMessage(void* arg)
{
  int x = 0;
  int y = 0;;
  //get the max x,y
  getmaxyx(stdscr,y,x);
  SendThreadArgs *data = (SendThreadArgs*)arg;

  //get the client IP
  char clientIP[IP_SIZE] = "";
  getClientIP(clientIP);

  //offset by 1 for null terminator
  char str[INPUT_MAX+1] = "";
  char input[INPUT_MAX+1] = "";

  char time[TIME_SIZE] = "";
  int addNewLine = 0;

  //Continue to get input from the user while the client is running
  while(clientRunning)
  {
    blankWin(data->outgoingWindow);
    int ret = 0;

    //print any old input that may have been lost(e.g. due to screen resize
    mvwprintw(data->outgoingWindow, 0,0, input, INPUT_MAX);

    //place the cursor at the correct position
    placeCursor(&x, &y, data->outgoingWindow, (int)strlen(input));

    //get input from the user(up to maximum of 80 characters)
    ret = mvwgetnstr(data->outgoingWindow, y, x, str, INPUT_MAX-strlen(input));

    if(ret == OK)     //the user has hit enter
    {
      formatTime(time);
      blankWin(data->outgoingWindow);

      strcat(input, str);
      if(strlen(input) > 0)
      {
        char outgoingMsg[MESSAGE_SIZE] = "";
        char* ps = input;

        //if more than 40 characters, then message must be split up
        if(strlen(input) > MAX_CHAR_PER_MSG)
        {
          //find the index of where to split up the message
          int index = splitMessage(input);
          char firstMsg[MAX_CHAR_PER_MSG+1] = "";
          strncpy(firstMsg, input, index);

          //add the first message to the screen
          sprintf(outgoingMsg, "%-15s [%-5s] >> %-40s (%s)", clientIP, data->userName, firstMsg, time);
          waddstr(data->incomingWindow, outgoingMsg);

          //the following is done in order to make the messages look cleaner when the window size
          //is greater or less than the length of the formatted output
          if (checkIfNeedNewLine(data->outgoingWindow))
          {
            waddstr(data->incomingWindow,"\n");
          }
          wrefresh(data->incomingWindow);


          //send the first message to the server
          char buffer[2048] = {};
          strcpy(buffer, outgoingMsg);
          send(data->socket, buffer, strlen(buffer), 0);

          //offset the index so the following message send works
          ps += index;
        }

        //write the text to the messages window
        sprintf(outgoingMsg, "%-15s [%-5s] >> %-40s (%s)", clientIP, data->userName, ps, time);
        waddstr(data->incomingWindow, outgoingMsg);

        //the following is done in order to make the messages look cleaner when the window size
        //is greater or less than the length of the formatted output
        if (checkIfNeedNewLine(data->outgoingWindow))
        {
          waddstr(data->incomingWindow,"\n");
        }
        wrefresh(data->incomingWindow);

        //send the message to the server
        char buffer[2048] = {};
        strcpy(buffer, outgoingMsg);
        send(data->socket, buffer, strlen(buffer), 0);

        if(!strcmp(str, EXIT_MSG))  //the user has entered the exit message
        {
          if(clientRunning) //check that another thread hasn't already shut down the client
          {
            //set the exit status -> to print statement to user when app closes
            data->exitStatus = USER_SHUTDOWN;
          }
          clientRunning = 0;
          break;
        }
        input[0] = 0;
      }
    }
    else if(ret == KEY_RESIZE)  //the user has resized the window, save the currently entered text
    {
      //input stores any lost text
      strcat(input, str);
      //resize the windows to make application responsive
      resizeWindows(data->outgoingWindow, data->outgoingBckgrnd, data->incomingWindow, data->incomingBckgrnd);
    }
  }

    return NULL;
}

// FUNCTION      : receiveMsg
// DESCRIPTION   : This is the function that is used by the thread that receives
//                  messages from the chat server
//
// PARAMETERS    :
//  void* arg -> pointer to the struct holding arguments for the thread
//
// RETURNS       :
//	void
void* receiveMsg(void* arg)
{

  SendThreadArgs *data = (SendThreadArgs*)arg;

  char message[INPUT_MAX+1] = {};

  //continue to accept while the client is running
  while (clientRunning)
  {
    //receive int message var
    int receive = recv(data->socket, message, INPUT_MAX+1, 0);
    if (receive > 0)  //received data from the server
    {
      if(!strcmp(message, EXIT_MSG))  //did the server send it's exit status?
      {
        data->exitStatus = SERVER_SHUTDOWN; //set the client's exit status -> to print info to user
        clientRunning = 0;  //client to shut down
      }
      if(clientRunning) //only add messages to the screen if the client is running
      {
        //add to the messages window
        waddstr(data->incomingWindow, message);

        //the following is done in order to make the messages look cleaner when the window size
        //is greater or less than the length of the formatted output
        if (checkIfNeedNewLine(data->incomingWindow))
        {
          waddstr(data->incomingWindow, "\n");
        }
          wrefresh(data->incomingWindow);
      }
    }
    else if (receive == 0)  //received nothing from the server, no response from server
    {
      if(clientRunning)
      {
      //set the client exit status
      data->exitStatus = NO_SERVER_RESPONSE;
      }
      clientRunning = 0;
    }
    //reset the message.
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
  formatTime(time);
  sprintf(input, "%s [%s]", clientIP, user);
  send(server, input , strlen(input), 0);
}
