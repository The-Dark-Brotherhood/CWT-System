#include "../inc/chat-client.h"

//Source: Sean Clarke
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

  mvwprintw(msgWinBackground, 1, (x/2 - 8), "INCOMING MESSAGES", INPUT_MAX);
  box(msgWinBackground, 0, 0);
  wrefresh(msgWinBackground);
  //set the background color for the received messages window
  wbkgd(msgWin, COLOR_PAIR(3));
  wrefresh(msgWin);

  //set the background color, box, and text for the textbox background window
  wbkgd(txtBoxBackground, COLOR_PAIR(2));
  box(txtBoxBackground, 0, 0);
  mvwprintw(txtBoxBackground, 1, (x/2 - 8), "OUTGOING MESSAGE", INPUT_MAX);
  wrefresh(txtBoxBackground);

  //set the background for the text box window
  wbkgd(txtBoxWin, COLOR_PAIR(2));
  keypad(txtBoxWin, 1);
  wrefresh(txtBoxWin);


}

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

  mvwin(msgWindow, 0, 0);
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

void placeCursor(int* cursorX, int* cursorY, WINDOW* txtBoxWin, int stringLength)
{
  int x,y;
  getmaxyx(txtBoxWin,y,x);

  *cursorY = (stringLength/x);
  *cursorX = (stringLength%x);

}
