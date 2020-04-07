#include "../inc/chat-client.h"

//Recommended to take in arguments for user and server

int main()
{

  int x,y;
  WINDOW *sub;
  initscr();
  cbreak();
  refresh();


  getmaxyx(stdscr,y,x);

  start_color();
  //Colors for main window
  init_pair(1,COLOR_WHITE,COLOR_BLACK);
  //Colors for subwindow->textbox
  init_pair(2,COLOR_GREEN,COLOR_BLUE);

  bkgd(COLOR_PAIR(1));
  refresh();

  //Color palette from: https://colorhunt.co/palette/2763
  init_color(COLOR_RED, 57,62,70);
  init_color(COLOR_BLACK, 133,157,192);
  init_color(COLOR_BLUE, 224, 243, 270);
  init_color(COLOR_GREEN,0,678, 709);
  init_color(COLOR_WHITE,933,933, 933);

  WINDOW* subBackground = newwin(10,x,(y-10),0);
  wbkgd(subBackground, COLOR_PAIR(2));
  box(subBackground, 0, 0);
  mvwprintw(subBackground, 1, (x/2 - 8), "OUTGOING MESSAGE", 80);
  wrefresh(subBackground);

  //position subwindow based on x and y of parent window
  WINDOW* subwindow = newwin(7,x-2,(y-8),1);
  wbkgd(subwindow, COLOR_PAIR(2));
  keypad(subwindow, 1);
  refresh();

  //Should color change when new message received?
  printw("New Message\n");
  refresh();
  //box(subwindow,0,0);
  char str[INPUT_MAX];

  wrefresh(subwindow);
  char clearString [81] = "";
  for(int i = 0; i <= 80; i++)
  {
    clearString[i] = 32;
  }
  for(;;)
  {
    blankWin(subwindow);
    int ret = 0;
    ret = mvwgetnstr(subwindow, 0, 0, str, 80);
    if(ret == OK)
    {


      mvwprintw(subwindow, 0, 0, clearString);
      wrefresh(subwindow);
      refresh();
      if(!strcmp(str, ">>bye<<"))
      {
        break;
      }
    }
    else if(ret == KEY_RESIZE)
    {
      blankWin(subBackground);
      blankWin(subwindow);
      resizeWindows(subwindow, subBackground);
      wcursyncup(subwindow);


    }


    //printw("%s\n", str);
    // bkgd(COLOR_PAIR(1));
    // wbkgd(subwindow, COLOR_PAIR(2));


  }

  getch();
  delwin(subwindow);

  endwin();
  return(0);


}
