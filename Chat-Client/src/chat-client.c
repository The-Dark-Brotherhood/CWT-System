//test github
#include "../inc/chat-client.h"



int main()
{

  int x,y;
  WINDOW *sub;
  initscr();
  cbreak();
  refresh();
  noecho();


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

  //position subwindow based on x and y of parent window
  WINDOW* subwindow = newwin(10,x,(y-10),0);
  scrollok(subwindow,TRUE);
  wbkgd(subwindow, COLOR_PAIR(2));
  keypad(subwindow, 1);
  refresh();

  //Should color change when new message received?
  printw("New Message\n");
  refresh();
  box(subwindow,0,0);
  char str[INPUT_MAX];

  wrefresh(subwindow);

  for(;;)
  {
    input_win(subwindow, str);
    printw("%s\n", str);
    bkgd(COLOR_PAIR(1));
    refresh();

    if(!strcmp(str, ">>bye<<"))
    {
      break;
    }

  }

  getch();
  delwin(subwindow);

  endwin();
  return(0);


}
