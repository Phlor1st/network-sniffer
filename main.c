#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include "includes/map.h"
#include "includes/globals.h"
#include "includes/rawSocket.h"
#include "includes/parseArguments.h"
#include "includes/analysisPacket.h"
#include "includes/indicator.h"

char srcIP[16] = {0}, destIP[16] = {0};
char filterProtocol = 'X'; // переменная хранит тип фильтруемого протокола
int tcp = 0, udp = 0, icmp = 0, other = 0, otherEth = 0, arp = 0; // счетчики для разных протоколов
long int receiveBytes = 0, receivePacks = 0; // полученные байты и пакеты

struct map *resultStat;

int rawSocket;
char *interface = NULL;

unsigned char buffer[UINT16_MAX]; // Размер окна TCP должен быть не более чем 16-разрядным значением
int dataSize = 0; // байты при приеме
int n = INT32_MAX; //количество пакетов для захвата
int indicatorMode = 0;
char pcapFilter[256] = {0};
int indicatorIntervalSec = 1;
int indicatorDurationSec = 0;
char indicatorOutputCsv[260] = "activity.csv";
volatile sig_atomic_t stopRequested = 0;
// количество пакетов по умолчанию бесконечно, поэтому установите максимальное значение

void sigintHandler(int sig);

int main(int argc, char *argv[]) {

    int parseResult = parseArguments(argc, argv);
    if (!parseResult)
        return 0;

    signal(SIGINT, sigintHandler);

    if (interface == NULL) {
        printf("%sInterface is required%s\n",RED, RESET);
        printf("%sUsage help:%s %s -h\n", GREEN, RESET, argv[0]);
        return 1;
    }

    if (indicatorMode) {
        return runIndicatorMode();
    }

    printf("%sInterface:%s %s\n", BLUE, RESET, interface);
    printf("%sFiltering for Protocol:%s %c\n",BLUE, RESET, filterProtocol);
    printf("%sNumber of packets:%s %d\n",BLUE, RESET, n);
    if (strlen(srcIP) > 0) {
        printf("%sFiltering for Source IP:%s %s\n",BLUE, RESET, srcIP);
    }
    if (strlen(destIP) > 0) {
        printf("%sFiltering for Destination IP:%s %s\n",BLUE, RESET, destIP);
    }

    rawSocket = createRawSocket(interface);

    analysisPacket(rawSocket, INT32_MAX);
    close(rawSocket);
    mapPrint(resultStat);
    mapClose(resultStat);
    printf("%s----------------------------------------------------------------------------------------------%s\n",GREEN,
           RESET);
    printf("%sSocket closed%s\n",GREEN, RESET);
    printf("%sFinished%s\n",GREEN, RESET);

    return 0;
}

// обработка сигнала прерывания
void sigintHandler(int sig) {
    (void)sig;
    stopRequested = 1;
    indicatorHandleSigint();
}
