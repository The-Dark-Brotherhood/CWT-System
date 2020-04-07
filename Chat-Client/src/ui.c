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
  box(win, 0, 0);             /* draw the box again */
  wrefresh(win);
}
void input_win(WINDOW *win, char *word)
{
  int i, ch;
  int maxrow, maxcol, row = 1, col = 0;
  int y, x;
  blankWin(win);                          /* make it a clean window */
  getmaxyx(win, maxrow, maxcol);          /* get window size */
  bzero(word, INPUT_MAX);
  wmove(win, 1, 1);                       /* position cusor at top */
  for (i = 0; (ch = wgetch(win)) != '\n'; i++)
  {
    switch(ch)
    {
      case KEY_UP:
      break;
      case KEY_DOWN:
      break;
      case KEY_LEFT:
      break;
      case KEY_RIGHT:
      break;
      default:
      if(ch == KEY_BACKSPACE || ch == KEY_DC || ch == 127)
      {
        getyx(win, y, x);
        if(x == 1 && y > 1)
        {
          wmove(win, y-1, maxcol-2);
          wprintw(win, " ");
          wmove(win, y-1, maxcol-2);

        }
        else if(x != 1)
        {
          wmove(win, y, x-1);
          wprintw(win, " ");
          wmove(win, y, x-1);
        }
        if(i != 0)
        {
          i--;
          word[i] = 0;
          i--;
          wrefresh(win);
        }


      }
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
    usleep(50000);
  }
}
