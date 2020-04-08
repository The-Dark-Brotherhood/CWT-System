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

// FUNCTION      : writeToLog
// DESCRIPTION   : generates output for the logfile, writes to log file
//                  using a semaphore. Calls getTime() in order to get current
//                  time for log file.
//
// PARAMETERS    : char* logMessage -> Message that is going to be written to logfile
//                 har* path -> Path to the logfile
//
//  RETURNS      : void
void writeToLog(char* logMessage, const char* path)
{
  createFilePathIfNotExists();

  //calculate the current time
  char time[TIME_LEN] = "";
  getTime(time);

  //open file, write log entry, then close file
  FILE * fp = fopen (path, "a");
  if(fp != NULL)
  {
    fprintf(fp, "[%s] : %s\n", time, logMessage);
    fclose(fp);
  }
}

// FUNCTION      : createFilePathIfNotExists
// DESCRIPTION   : this function creates a filepath if that filepath
//               - filepath set in #defines of header file
//
// PARAMETERS    :  NADA
//
//  RETURNS      : void
void createFilePathIfNotExists()
{
  struct stat st = {0};
  if (stat(LOG_FOLDER_PATH, &st) == -1)
  {
    mkdir(LOG_FOLDER_PATH, 0700);
  }
}
