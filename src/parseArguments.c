#include "../includes/parseArguments.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


#include "../includes/map.h"
#include "../includes/globals.h"
#include "../includes/interface.h"


int parseArguments(int argc, char *argv[]) {
    int opt; // чтобы проверить параметры командной строки (options)
    // анализ параметров командной строки
    while ((opt = getopt(argc, argv, "i:s:d:p:f:b:t:o:m:lh")) != -1) {
        // передача параметров в программу argv - значениея аргументов, argc - счетчик
        switch (opt) {
            case 'i':
                interface = optarg;
                break;

            case 'l':
                listInterfaces();
                return 0;

            case 'p':
                if (strcmp(optarg, "TCP") == 0) {
                    filterProtocol = 'T';
                } else if (strcmp(optarg, "UDP") == 0) {
                    filterProtocol = 'U';
                } else if (strcmp(optarg, "ICMP") == 0) {
                    filterProtocol = 'I';
                } else if (strcmp(optarg, "ARP") == 0) {
                    filterProtocol = 'A';
                } else {
                    printf("%sInvalid protocol%s\n", RED, RESET);
                    return 0;
                }
                break;

            case 's':
                // 255.255.255.255 --> 15 characters
                strncpy(srcIP, optarg, 15);
                srcIP[15] = '\0'; // null terminator, since strncpy doesn't add it
                break;

            case 'd':
                strncpy(destIP, optarg, 15);
                destIP[15] = '\0';
                break;

            case 'h':
                printf("%sUsage:%s %s -i interface [-m (SNIFFER|INDICATOR)] [options] [-l]\n",
                           GREEN, RESET, argv[0]);
                printf("%sOptions:%s\n",GREEN, RESET);
                printf("\t%s-i:%s  Interface\n", BLUE, RESET);
                printf("\t%s-l:%s  List of interface\n", BLUE, RESET);
                printf("\t%s-m:%s  Mode: SNIFFER (default) or INDICATOR\n", BLUE, RESET);
                printf("\t%s-p:%s  Protocol (Default is watch for All)\n", BLUE, RESET);
                printf("\t%s-s:%s  Filter for Source IP address\n", BLUE, RESET);
                printf("\t%s-d:%s  Filter for Destination IP address\n", BLUE, RESET);
                printf("\t%s-f:%s  pcap filter string (for INDICATOR mode)\n", BLUE, RESET);
                printf("\t%s-b:%s  Indicator bucket interval in seconds (default 1)\n", BLUE, RESET);
                printf("\t%s-t:%s  Indicator duration in seconds (0 = until Ctrl+C)\n", BLUE, RESET);
                printf("\t%s-o:%s  Indicator output CSV file\n", BLUE, RESET);
                return 0;

            case 'f':
                strncpy(pcapFilter, optarg, sizeof(pcapFilter) - 1);
                pcapFilter[sizeof(pcapFilter) - 1] = '\0';
                break;

            case 'b':
                indicatorIntervalSec = atoi(optarg);
                break;

            case 't':
                indicatorDurationSec = atoi(optarg);
                break;

            case 'o':
                strncpy(indicatorOutputCsv, optarg, sizeof(indicatorOutputCsv) - 1);
                indicatorOutputCsv[sizeof(indicatorOutputCsv) - 1] = '\0';
                break;

            case 'm':
                if (strcmp(optarg, "INDICATOR") == 0) {
                    indicatorMode = 1;
                } else if (strcmp(optarg, "SNIFFER") == 0) {
                    indicatorMode = 0;
                } else {
                    printf("%sInvalid mode%s. Use SNIFFER or INDICATOR\n", RED, RESET);
                    return 0;
                }
                break;

            default:
                printf("%sUsage help:%s %s -h\n", GREEN, RESET, argv[0]);
                return 0;
        }
    }
    return 1;
}