#include "../inc/chat-client.h"

//Recommended to take in arguments for user and server

int main()
{

  initscr();
  cbreak();
  //signal (SIGWINCH, NULL);
  refresh();
  int x,y;
  getmaxyx(stdscr,y,x);
  WINDOW *messagesWindowBackground = newwin(y-(y/5+3), x, 0, 0);
  WINDOW *messagesWindow = newwin(11, x-2, 2, 1);
  scrollok(messagesWindow, TRUE);
  WINDOW *subBackground = newwin(y/5+3,x,(y-(y/5+3)),0);
  WINDOW *subwindow = newwin(y/5,x-2,(y-(y/5+1)),1);

  setUpWindows(subwindow, subBackground, messagesWindow, messagesWindowBackground);

  //Should color change when new message received?

  int numMessages = 0;

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
      waddstr(messagesWindow, input);
      waddstr(messagesWindow, "\n");
      wrefresh(messagesWindow);
      input[0] = 0;

    }
    else if(ret == KEY_RESIZE)
    {
      strcat(input, str);

      resizeWindows(subwindow, subBackground, messagesWindow, messagesWindowBackground);
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
