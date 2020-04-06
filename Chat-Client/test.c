//test github
#include <ncurses.h>

#define BOX_WIDTH   70
int main()
{

  int x,y;
  WINDOW *sub;
  initscr();
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
  wbkgd(subwindow, COLOR_PAIR(2));

  refresh();

  //Should color change when new message received?

  box(subwindow,0,0);
  mvwprintw(subwindow, 1, 1, "subwindow");
  refresh();
  wrefresh(subwindow);

  getch();
  delwin(subwindow);

  endwin();
  return(0);

  // start_color();
  // init_pair(1,COLOR_YELLOW,COLOR_BLUE);
  // bkgd(COLOR_PAIR(1));
  //
  // addch(ACS_ULCORNER);
  // for(int i = 0; i < BOX_WIDTH; i++)
  // {
  //   addch(ACS_HLINE);
  // }
  // addch(ACS_URCORNER);
  // addch('\n');
  // for(int i = 0; i <= 2; i++)
  // {
  //   addch(ACS_VLINE);
  //   for(int i = 0; i < BOX_WIDTH; i++)
  //   {
  //     addch(' ');
  //   }
  //   addch(ACS_VLINE);
  //   addch('\n');
  // }
  // addch(ACS_LLCORNER);
  // for(int i = 0; i < BOX_WIDTH; i++)
  // {
  //   addch(ACS_HLINE);
  // }
  // addch(ACS_LRCORNER);
  // addch('\n');
  // move(1,1);
  // refresh();
  // getch();

  // bkgd(COLOR_PAIR(2));
  // refresh();
  // getch();
  //
  // bkgd(COLOR_PAIR(3));
  // refresh();
  // getch();


}
