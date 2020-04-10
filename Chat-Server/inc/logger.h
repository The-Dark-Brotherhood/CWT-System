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
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>

#define LOG_PATH        "CWTServer.log"
#define LOG_LEN         180
#define TAG_LEN         5
#define TIME_LEN        50

// Function Prototypes
void getTime(char* output);
void createFilePathIfNotExists();
void writeToLog(const char* tag, const char* logMessage, const char* path);
void serverLog (const char* tag, char* fmt, ...);
