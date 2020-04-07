#include "../inc/chat-client.h"

void blankWin(WINDOW *win)
{
  int i;
  int maxrow, maxcol;

  getmaxyx(win, maxrow, maxcol);
  for (i = 1; i < maxcol-2; i++)
  {
    wmove(win, i, 1);
    refresh();
    wclrtoeol(win);
    wrefresh(win);
  }
  //box(win, 0, 0);             /* draw the box again */
  wrefresh(win);
}
void setUpWindows(WINDOW *win, WINDOW *winBg)
{
  int x,y;
  getmaxyx(stdscr,y,x);
  winBg = newwin(10,x,(y-10),0);
  wbkgd(winBg, COLOR_PAIR(2));
  box(winBg, 0, 0);
  mvwprintw(winBg, 1, (x/2 - 8), "OUTGOING MESSAGE", 80);
  wrefresh(winBg);

  win = newwin(7,x-2,(y-8),1);
  wbkgd(win, COLOR_PAIR(2));
  keypad(win, 1);
  refresh();
}

void resizeWindows(WINDOW *win, WINDOW *winBg)
{
  int x,y;
  getmaxyx(stdscr,y,x);

  //check for resize failure
  wresize(winBg, 10, x);
  wresize(win,7, x-2);

  mvwin(win,(y-8),1 );
  mvwin(winBg,(y-10),0 );

  wbkgd(winBg, COLOR_PAIR(2));
  wbkgd(win, COLOR_PAIR(2));
  box(winBg, 0, 0);
  mvwprintw(winBg, 1, (x/2 - 8), "OUTGOING MESSAGE", 80);
  wrefresh(winBg);
  wrefresh(win);
  //wredrawln(stdscr, 0, y);
  refresh();



}
