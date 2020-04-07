#pragma once
#include <ncurses.h>
#include <string.h>
#include <unistd.h>

#define INPUT_MAX   81
#define BOX_WIDTH   70

void blankWin(WINDOW *win);
void input_win(WINDOW *win, char *word);
