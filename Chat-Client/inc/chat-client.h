#pragma once
#include <ncurses.h>
#include <curses.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define INPUT_MAX   81
#define BOX_WIDTH   70

void blankWin(WINDOW *win);
void setUpWindows(WINDOW *win, WINDOW *winBg, WINDOW *msgWindow);
void resizeWindows(WINDOW *win, WINDOW *winBg, WINDOW *msgWindow);
void placeCursor(int* cursorX, int* cursorY, WINDOW* subWindow, int stringLength);
