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
  WINDOW *subBackground;
  subBackground = newwin(10,x,(y-10),0);
  WINDOW* subwindow = newwin(7,x-2,(y-8),1);
  setUpWindows(subwindow, subBackground);


  //Should color change when new message received?
  printw("New Message\n");
  refresh();

  //box(subwindow,0,0);
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
      printw("%s\n", input);
      input[0] = 0;

    }
    else if(ret == KEY_RESIZE)
    {
      strcat(input, str);
      blankWin(subBackground);
      blankWin(subwindow);
      resizeWindows(subwindow, subBackground);
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
