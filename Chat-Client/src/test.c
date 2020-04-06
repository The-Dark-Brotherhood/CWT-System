//test github
#include <ncurses.h>
#include <string.h>

#define INPUT_MAX   81
#define BOX_WIDTH   70


void rectangle(int y1, int x1, int y2, int x2)
{
    mvhline(y1, x1, 0, x2-x1);
    mvhline(y2, x1, 0, x2-x1);
    mvvline(y1, x1, 0, y2-y1);
    mvvline(y1, x2, 0, y2-y1);
    mvaddch(y1, x1, ACS_ULCORNER);
    mvaddch(y2, x1, ACS_LLCORNER);
    mvaddch(y1, x2, ACS_URCORNER);
    mvaddch(y2, x2, ACS_LRCORNER);
}
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
void input_win(WINDOW *win, char *word)
{
  int i, ch;
  int maxrow, maxcol, row = 1, col = 0;

  blankWin(win);                          /* make it a clean window */
  getmaxyx(win, maxrow, maxcol);          /* get window size */
  bzero(word, INPUT_MAX);
  wmove(win, 1, 1);                       /* position cusor at top */
  //mvwgetnstr(win, 1, 1, word, 80);
  for (i = 0; (ch = wgetch(win)) != '\n'; i++)
  {
    if(ch >= 32 && ch < 127)
    {

      if(strlen(word) < (INPUT_MAX - 1))
      {
        word[i] = ch;                       /* '\n' not copied */
        if (col++ < maxcol-2)               /* if within window */
        {
          wprintw(win, "%c", word[i]);      /* display the char recv'd */
        }
        else                                /* last char pos reached */
        {
          col = 1;
          if (row == maxrow-2)              /* last line in the window */
          {
            scroll(win);                    /* go up one line */
            row = maxrow-2;                 /* stay at the last line */
            wmove(win, row, col);           /* move cursor to the beginning */
            wclrtoeol(win);                 /* clear from cursor to eol */
            box(win, 0, 0);                 /* draw the box again */
          }
          else
          {
            row++;
            wmove(win, row, col);           /* move cursor to the beginning */
            wrefresh(win);
            wprintw(win, "%c", word[i]);    /* display the char recv'd */
          }
        }
      }
    }
  }
}
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
  rectangle(y-11,0,y-1 ,x-1);
  WINDOW* subwindow = newwin(9,x-2,(y-10),1);
  scrollok(subwindow,TRUE);
  wbkgd(subwindow, COLOR_PAIR(2));

  refresh();

  //Should color change when new message received?
  refresh();
  char str[INPUT_MAX];
  //mvwprintw(subwindow, 1, 1, "subwindow");
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
