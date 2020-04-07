#pragma once
#include <ncurses.h>
#include <curses.h>
#include <string.h>
#include <unistd.h>

#define INPUT_MAX   81
#define BOX_WIDTH   70

void blankWin(WINDOW *win);
void setUpWindows(WINDOW *win, WINDOW *winBg);
void resizeWindows(WINDOW *win, WINDOW *winBg);
