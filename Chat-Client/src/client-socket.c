#include "../inc/chat-client.h"

void getClientIP(char* clientIP)
{

  //https://stackoverflow.com/questions/4139405/how-can-i-get-to-know-the-ip-address-for-interfaces-in-c
  struct ifaddrs *ifap, *ifa;
  struct sockaddr_in *sa;
  char *addr;
  getifaddrs (&ifap);
  for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
      if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET) {
          sa = (struct sockaddr_in *) ifa->ifa_addr;
          addr = inet_ntoa(sa->sin_addr);
          if(!strcmp(ifa->ifa_name, "ens33"))
          {
            strcpy(clientIP, addr);
          }
      }
  }
  freeifaddrs(ifap);
}

//https://stackoverflow.com/questions/5141960/get-the-current-time-in-c
void format_time(char *output)
{
  struct timeval  now;
  struct tm*      local;

  gettimeofday(&now, NULL);
  local = localtime(&now.tv_sec);

  sprintf(output, "%02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);

}
