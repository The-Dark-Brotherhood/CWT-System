#include "../inc/chat-client.h"

//Recommended to take in arguments for user and server

int main()
{

  //WINDOW* subwindow;
  initscr();
  cbreak();
  //signal (SIGWINCH, NULL);
  refresh();
  int x,y;
  getmaxyx(stdscr,y,x);
  WINDOW *messagesWindow = newwin(y*.8-2,x,0, 0);
  WINDOW *subBackground = newwin(y/5+3,x,(y-(y/5+3)),0);
  WINDOW *subwindow = newwin(y/5,x-2,(y-(y/5+1)),1);
  setUpWindows(subwindow, subBackground, messagesWindow);

  //Should color change when new message received?

  //int

  char str[INPUT_MAX] = "";
  char input[INPUT_MAX] = "";

  wrefresh(subwindow);
  for(;;)
  {
    blankWin(subwindow);
    int ret = 0;
    mvwprintw(subwindow, 0,0, input, 80);
    placeCursor(&x, &y, subwindow, (int)strlen(input));

    ret = mvwgetnstr(subwindow, y, x, str, 80-strlen(input));
    if(ret == OK)
    {
      blankWin(subwindow);
      if(!strcmp(str, ">>bye<<"))
      {
        break;
      }
      strcat(input, str);
      mvwprintw(messagesWindow, 1,1, input, 80);
      wrefresh(messagesWindow);
      input[0] = 0;

    }
    else if(ret == KEY_RESIZE)
    {
      strcat(input, str);
      blankWin(subBackground);
      blankWin(subwindow);
      blankWin(messagesWindow);
      resizeWindows(subwindow, subBackground, messagesWindow);
      //wcursyncup(subwindow);
    }


    // bkgd(COLOR_PAIR(1));
    // wbkgd(subwindow, COLOR_PAIR(2));


  }

  getch();
  delwin(subwindow);

  endwin();
  return(0);


}
