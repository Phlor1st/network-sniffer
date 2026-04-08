#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>

#include "../includes/interface.h"
#include "../includes/globals.h"

void listInterfaces(void) {
    struct ifaddrs *ifaddr, *ifa;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
    }

    int idx = 1;
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL ||
            ifa->ifa_addr->sa_family != AF_PACKET)
            continue;

        printf("%s%i.Interface:%s %s \n", BLUE, idx, RESET, ifa->ifa_name);
        idx++;
    }

    freeifaddrs(ifaddr);
}