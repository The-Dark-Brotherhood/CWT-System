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


#define INPUT_MAX     81
#define BOX_WIDTH     70
#define MESSAGE_SIZE  79
#define PORT          5000
#define IP_SIZE       20


void blankWin(WINDOW *win);
void setUpWindows(WINDOW *subwindow, WINDOW *subBackground, WINDOW *messagesWindow, WINDOW *messagesWindowBackground);
void resizeWindows(WINDOW *win, WINDOW *winBg, WINDOW *msgWindow, WINDOW *msgWindowBackground);
void placeCursor(int* cursorX, int* cursorY, WINDOW* subWindow, int stringLength);
