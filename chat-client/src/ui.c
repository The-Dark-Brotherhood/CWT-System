/*
*  FILE          : ui.c
*  PROJECT       : Assignment #4 - Can We Talk?
*  PROGRAMMER    : Gabriel Gurgel, Michael Gordon
*  FIRST VERSION : 2020-03-27
*  DESCRIPTION   : The functions in this file are used to improve the UI
*                  of the client application. The application responds well 
*                  to window resizing due to using ratios when setting positions and
*                  sizes of the windows
*/

#include "../inc/chat-client.h"


// FUNCTION      : blankWin
// DESCRIPTION   : This function is used to clear the window screen
//                  Source: Sean Clarke example code
// PARAMETERS    :
//  WINDOW* win -> the window to clear
//
// RETURNS       :
//	void
//
void blankWin(WINDOW *win)
{
  int i;
  int maxrow, maxcol;
  char clearString [INPUT_MAX+1] = "";
  for(int i = 0; i < INPUT_MAX; i++)
  {
    clearString[i] = 32;
  }
  mvwprintw(win, 0, 0, clearString);
  getmaxyx(win, maxrow, maxcol);
  for (i = 1; i < maxcol-2; i++)
  {
    wmove(win, i, 1);
    refresh();
    wclrtoeol(win);
  }
  wrefresh(win);
}

// FUNCTION      : setUpWindows
// DESCRIPTION   : This function is used to setup the windows. The colors,
//                boxes, text of the windows is set up in this function
// PARAMETERS    :
//  WINDOW *txtBoxWin         -> the "textbox" window
//  WINDOW *txtBoxBackground  -> background window where user can enter text
//  WINDOW *msgWin            -> the window containing messages
//  WINDOW *msgWinBackground  -> the background window of messages window
//
// RETURNS       :
//	void
//
void setUpWindows(WINDOW *txtBoxWin, WINDOW *txtBoxBackground, WINDOW *msgWin, WINDOW *msgWinBackground)
{
  int x,y;
  getmaxyx(stdscr,y,x);

  start_color();

  //Set up the colors to use with ncurses
  init_pair(1,COLOR_WHITE,COLOR_BLACK);
  init_pair(2,COLOR_GREEN,COLOR_BLUE);
  init_pair(3,COLOR_WHITE,COLOR_GREEN);

  //Adjust the color rgb values to create custom colors
  init_color(COLOR_RED, 57,62,70);
  init_color(COLOR_BLACK, 133,157,192);
  init_color(COLOR_BLUE, 224, 243, 270);
  init_color(COLOR_GREEN,0,678, 709);
  init_color(COLOR_WHITE,933,933, 933);

  //set background color
  bkgd(COLOR_PAIR(3));
  refresh();

  //Set the background color,text, and box for the messages background window
  wbkgd(msgWinBackground, COLOR_PAIR(3));
  mvwprintw(msgWinBackground, 1, (x/2 - 8), "INCOMING MESSAGES", INPUT_MAX);  //centre the text with x/2 - 8
  box(msgWinBackground, 0, 0);
  wrefresh(msgWinBackground);

  //set the background color for the received messages window
  wbkgd(msgWin, COLOR_PAIR(3));
  wrefresh(msgWin);

  //set the background color, box, and text for the textbox background window
  wbkgd(txtBoxBackground, COLOR_PAIR(2));
  box(txtBoxBackground, 0, 0);
  mvwprintw(txtBoxBackground, 1, (x/2 - 8), "OUTGOING MESSAGE", INPUT_MAX); //centre the text with x/2 - 8
  wrefresh(txtBoxBackground);

  //set the background for the text box window
  wbkgd(txtBoxWin, COLOR_PAIR(2));
  keypad(txtBoxWin, 1);
  wrefresh(txtBoxWin);

}

// FUNCTION      : resizeWindows
// DESCRIPTION   : This function is used to resize the window screens. The
//                  dimenstions are established using % to ensure responsivness
//
// PARAMETERS    :
//  WINDOW *win                 -> the "textbox" window
//  WINDOW *winBg               -> background window where user can enter text
//  WINDOW *msgWindow            -> the window containing messages
//  WINDOW *msgWindowBackground  -> the background window of messages window
//
// RETURNS       :
//	void
//
void resizeWindows(WINDOW *win, WINDOW *winBg, WINDOW *msgWindow, WINDOW *msgWindowBackground)
{
  blankWin(win);
  blankWin(winBg);
  blankWin(msgWindowBackground);
  int x,y;
  getmaxyx(stdscr,y,x);

  //check for resize failure
  wresize(msgWindowBackground, y-(y/TXT_BOX_HEIGHT_RATIO+BCKGRND_PADDING_OFFSET), x);
  wresize(msgWindow, INCOMING_MSG_WINDOW_HEIGHT, x-2);
  wresize(winBg, (y/TXT_BOX_HEIGHT_RATIO+BCKGRND_PADDING_OFFSET), x);
  wresize(win,y/TXT_BOX_HEIGHT_RATIO, x-2);

  mvwin(msgWindowBackground, 0, BCKGRND_STARTING_X);
  mvwin(msgWindow, 2, 1);
  mvwin(win,(y-(y/TXT_BOX_HEIGHT_RATIO+1)),1 );
  mvwin(winBg,(y-(y/TXT_BOX_HEIGHT_RATIO+3)),0);

  wbkgd(msgWindow, COLOR_PAIR(3));
  mvwprintw(msgWindowBackground, 1, (x/2 - 8), "INCOMING MESSAGES", 80);

  wbkgd(winBg, COLOR_PAIR(2));
  wbkgd(win, COLOR_PAIR(2));
  box(msgWindowBackground, 0, 0);
  box(winBg, 0, 0);
  mvwprintw(winBg, 1, (x/2 - 8), "OUTGOING MESSAGE", 80);
  redrawwin(msgWindow);
  wrefresh(winBg);
  wrefresh(msgWindow);
  wrefresh(win);
  wrefresh(msgWindowBackground);
  refresh();

}

// FUNCTION      : placeCursor
// DESCRIPTION   : This function is used to place the cursor at the proper location
//                  in the text box. This is primarily used when the user resizes the screen
// PARAMETERS    :
//  int* cursorX      -> where to store the x value of cursor location
//  int* cursorY      -> where to store the y value of cursor location
//  WINDOW* txtBoxWin -> the textbox window
//  int stringLength  -> length of the input string currently in the textbox
//
// RETURNS       :
//	void
//
void placeCursor(int* cursorX, int* cursorY, WINDOW* txtBoxWin, int stringLength)
{
  int x,y;
  getmaxyx(txtBoxWin,y,x);

  *cursorY = (stringLength/x);
  *cursorX = (stringLength%x);
}


// FUNCTION      : checkIfNeedNewLine
// DESCRIPTION   : This function is used to improve the ui experience of the users
//                 If the width of the window is too long or short, a new line
//                 character should be inserted to improve readabilitiy
// PARAMETERS    :
//  WINDOW* incomingWindow -> the messages window
//
// RETURNS       :
//	void
//
int checkIfNeedNewLine(WINDOW* incomingWindow)
{
  int windowX = 0;
  int windowY = 0;
  getmaxyx(incomingWindow, windowY, windowX);

  if(windowX != MESSAGE_SIZE-1)
  {
    return 1;
  }
  return 0;
}
