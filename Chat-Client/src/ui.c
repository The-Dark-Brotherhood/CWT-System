#include "../inc/chat-client.h"

void blankWin(WINDOW *win)
{
  int i;
  int maxrow, maxcol;
  char clearString [81] = "";
  for(int i = 0; i < 80; i++)
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

void setUpWindows(WINDOW *subwindow, WINDOW *subBackground, WINDOW *messagesWindow, WINDOW *messagesWindowBackground)
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
  wbkgd(messagesWindowBackground, COLOR_PAIR(3));

  mvwprintw(messagesWindowBackground, 1, (x/2 - 8), "INCOMING MESSAGES", 80);
  box(messagesWindowBackground, 0, 0);
  wrefresh(messagesWindowBackground);
  //set the background color for the received messages window
  wbkgd(messagesWindow, COLOR_PAIR(3));
  wrefresh(messagesWindow);

  //set the background color, box, and text for the textbox background window
  wbkgd(subBackground, COLOR_PAIR(2));
  box(subBackground, 0, 0);
  mvwprintw(subBackground, 1, (x/2 - 8), "OUTGOING MESSAGE", 80);
  wrefresh(subBackground);

  //set the background for the text box window
  wbkgd(subwindow, COLOR_PAIR(2));
  keypad(subwindow, 1);
  wrefresh(subwindow);


}

void resizeWindows(WINDOW *win, WINDOW *winBg, WINDOW *msgWindow, WINDOW *msgWindowBackground)
{
  blankWin(win);
  blankWin(winBg);
  blankWin(msgWindowBackground);
  int x,y;
  getmaxyx(stdscr,y,x);

  //check for resize failure
  wresize(msgWindowBackground, y-(y/5+3), x);
  wresize(msgWindow, 11, x-2);
  wresize(winBg, (y/5+3), x);
  wresize(win,y/5, x-2);

  mvwin(msgWindow, 0, 0);
  mvwin(msgWindow, 2, 1);
  mvwin(win,(y-(y/5+1)),1 );
  mvwin(winBg,(y-(y/5+3)),0);

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

void placeCursor(int* cursorX, int* cursorY, WINDOW* subWindow, int stringLength)
{
  int x,y;
  getmaxyx(subWindow,y,x);

  *cursorY = (stringLength/x);
  *cursorX = (stringLength%x);

}
