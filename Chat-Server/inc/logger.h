/*
*  FILE          : logger.h
*  PROJECT       : Assignment #3
*  PROGRAMMER    : Gabriel Gurgel & Michael Gordon
*  FIRST VERSION : 2020-03-21
*  DESCRIPTION   : Shared constants, includes, and structures for the Hoochamacallit
*                  logger functionality
*/

#pragma once
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

#define LOG_FOLDER_PATH "/tmp/"
#define LOG_LEN         256
#define TIME_LEN        50

// Function Prototypes
void getTime(char* output);
void createFilePathIfNotExists();
void writeToLog(char* logMessage, const char* path);
