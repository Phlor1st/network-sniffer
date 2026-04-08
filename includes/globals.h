
#ifndef GLOBALS_H
#define GLOBALS_H

#include "map.h"

extern struct map *resultStat;
extern char srcIP[16];
extern char destIP[16];
extern char filterProtocol;
extern int tcp;
extern int udp;
extern int icmp;
extern int arp;
extern int other;
extern long int receiveBytes;
extern long int receivePacks;
extern int rawSocket;
extern char *interface;
extern unsigned char buffer[];
extern int dataSize;
extern int n;
extern int indicatorMode;
extern char pcapFilter[256];
extern int indicatorIntervalSec;
extern int indicatorDurationSec;
extern char indicatorOutputCsv[260];

#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define YELLOW  "\033[1;33m"
#define WHITE   "\033[1;37m"
#define GREEN   "\033[1;32m"
#define BLUE    "\033[38;5;141m"

#endif //GLOBALS_H
