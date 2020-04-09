#pragma once
#include <ncurses.h>
#include <curses.h>
#include <signal.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/if_link.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>


#define INPUT_MAX     81
#define BOX_WIDTH     70
#define MESSAGE_SIZE  79
#define PORT          5000
#define IP_SIZE       16
#define TIME_SIZE     9
#define NAME_SIZE     6
#define CONTENT_SIZE  41
#define buffersize 1024

typedef struct{
  long type;
  char address[IP_SIZE];
  char name[NAME_SIZE];
  char content[CONTENT_SIZE];
  char time[TIME_SIZE];
} ClientMessage;

void blankWin(WINDOW *win);
void setUpWindows(WINDOW *subwindow, WINDOW *subBackground, WINDOW *messagesWindow, WINDOW *messagesWindowBackground);
void resizeWindows(WINDOW *win, WINDOW *winBg, WINDOW *msgWindow, WINDOW *msgWindowBackground);
void placeCursor(int* cursorX, int* cursorY, WINDOW* subWindow, int stringLength);
void getClientIP(char* clientIP);
void format_time(char *output);
void* sendMessage(void* message);
