#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>


#include "../includes/map.h"
#include "../includes/packetProcrssing.h"
#include "../includes/globals.h"
#include "../includes/analysisPacket.h"

void analysisPacket(int rawSocket, int n) {

    printf("%sListening for packets...%s\n",GREEN, RESET);
    printf("%sPress %sCtrl+C%s to stop%s\n",GREEN, RED, GREEN,RESET);
    printf("%s----------------------------------------------------------------------------------------------%s\n",GREEN,
           RESET);

    resultStat = mapNew();
    while (n > 0) {
        dataSize = recv(rawSocket, buffer, 65536, 0); // функция служит для чтения данных из сокета в буффер

        if (dataSize < 0) {
            printf("%sFailed to receive data%s",RED, RESET);
            close(rawSocket);
            exit(1);
        }

        if (processPacket(buffer, dataSize) == 1) // обновляет каждый захваченный элемент
            continue;

        n--; // счетчик оставшихся пактеов
    }
}