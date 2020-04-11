/*
*  FILE          : chat-client.h
*  PROJECT       : Assignment #4
*  PROGRAMMER    : Gabriel Gurgel, Michael Gordon
*  FIRST VERSION : 2020-03-27
*  DESCRIPTION   : Contains the include for the common include file, global constant defines,
*                  global variable extern, and function prototypes
*/

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

//global used for threads to know if client is running
extern int clientRunning;

#define INPUT_MAX                   80
#define MAX_CHAR_PER_MSG            40
#define MESSAGE_SIZE                79
#define PORT                        5000
#define IP_SIZE                     16
#define TIME_SIZE                   9
#define NAME_SIZE                   5
#define CONTENT_SIZE                41
#define INNER_WINDOW_PADDING        2
#define BCKGRND_PADDING_OFFSET      3
#define INCOMING_MSG_WINDOW_HEIGHT  11
#define TXT_BOX_HEIGHT_RATIO        5
#define BCKGRND_STARTING_X          0
#define INNER_WINDOW_STARTING_X     1
#define USER_FLAG_LENGTH            5
#define SERVER_FLAG_LENGTH          7
#define PORT_FLAG_LENGTH            5
#define SERVER_EXIT_MSG             ">>bye<<"


typedef struct {
    WINDOW* outgoingWindow;
    WINDOW* outgoingBckgrnd;
    WINDOW* incomingWindow;
    WINDOW* incomingBckgrnd;
    char* userName;
    int socket;
} SendThreadArgs;

void blankWin(WINDOW *win);
void setUpWindows(WINDOW *txtBoxWin, WINDOW *txtBoxBackground, WINDOW *msgWin, WINDOW *msgWinBackground);
void resizeWindows(WINDOW *txtBoxWin, WINDOW *txtBoxBackground, WINDOW *msgWin, WINDOW *msgWinBackground);
void placeCursor(int* cursorX, int* cursorY, WINDOW* txtBoxWin, int stringLength);
void getClientIP(char* clientIP);
void format_time(char *output);
void* sendMessage(void* arg);
void sendFirstMsg(int server, char* user);
void* receiveMsg(void* arg);
int splitMessage(char* message);
int checkIfNeedNewLine(WINDOW* incomingWindow);
