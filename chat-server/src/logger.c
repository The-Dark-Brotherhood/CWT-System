/*
*  FILE          : logger.c
*  PROJECT       : Assignment #3
*  PROGRAMMER    : Gabriel Gurgel & Michael Gordon
*  FIRST VERSION : 2020-03-21
*  DESCRIPTION   : Functions relating to File IO and logging
*/

#include "../inc/logger.h"


// FUNCTION      : getTime
// DESCRIPTION   : get the current time, formatted according to specifications
//                 stored in the parameter passed to function
//
// PARAMETERS    :
//	char* output -> pointer to string where formatted time will be ouput
//
//  RETURNS       :   void
void getTime(char* output)
{
  //Get time info in time_t struct
  time_t rawtime;
  struct tm *info;
  time( &rawtime );
  info = localtime(&rawtime);

  //Set the time output to match the requirements
  //first paramater is where to store outputted string
  strftime(output, 49, "%F %T",info);
}

// Reference: https://stackoverflow.com/questions/25009116/c-wrapper-function-for-sprintf-s
// FUNCTION      : serverLog
// DESCRIPTION   : Sprintf wrapper for logging.
//
// PARAMETERS    : char* tag -> log tag
//                 char* fmt -> Formated string
//
//  RETURNS      : void
void serverLog (const char* tag, char* fmt, ...)
{
  if(strlen(fmt) > LOG_LEN || strlen(tag) > TAG_LEN)
  {
    return;
  }

  char logMessage[LOG_LEN];
  va_list va;
  va_start (va, fmt);
  vsprintf (logMessage, fmt, va);
  va_end (va);

  writeToLog(tag, logMessage, LOG_PATH);
}

// FUNCTION      : writeToLog
// DESCRIPTION   : generates output for the logfile, writes to log file
//                  using a semaphore. Calls getTime() in order to get current
//                  time for log file.
//
// PARAMETERS    : char* logMessage -> Message that is going to be written to logfile
//                 har* path -> Path to the logfile
//
//  RETURNS      : void
void writeToLog(const char* tag, const char* logMessage, const char* path)
{
  //calculate the current time
  char time[TIME_LEN] = "";
  getTime(time);

  //open file, write log entry, then close file
  FILE * fp = fopen (path, "a");
  if(fp != NULL)
  {
    fprintf(fp, "[%s] [%-5s] - %s\n", time, tag, logMessage);
    fclose(fp);
  }
}
