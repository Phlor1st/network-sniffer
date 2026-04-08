#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <linux/if_packet.h>
#include <errno.h>
#include <netinet/if_ether.h>

#include "../includes/map.h"
#include "../includes/globals.h"
#include "../includes/rawSocket.h"


int createRawSocket(char *interface) {
    int sock;

    sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); // указывает на работу на сетевом уровне, позволяет получать сырые покеты, сокет принимает пакеты от всех типов

    if (sock < 0) {
        printf("%sSocket creation failed (%s)%s\n", RED, strerror(errno), RESET);
        close(sock);
        return 1;
    }
    printf("%sSocket created%s\n", GREEN, RESET);

    // Привязываем сокет к интерфейсу
    if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, interface, strlen(interface) + 1) < 0) {
        printf("%sBind to device failed (%s)%s\n", RED, strerror(errno), RESET);
        close(sock);
        return 1;
    }

    return sock;
}

