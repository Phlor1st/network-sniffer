#include "../includes/packetProcrssing.h"
#include "../includes/map.h"
#include "../includes/globals.h"

#include <stdio.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>      // преобразует IP-адрес в строку и наоборот (inet_ntoa)
#include <netinet/if_ether.h>


int processPacket(unsigned char *buffer, int size) {
    //заголовок полились пакеты(набор байт)
    struct iphdr *ipHead = (struct iphdr *) (buffer + sizeof(struct ethhdr));
    struct ethhdr *ethHead = (struct ethhdr *) buffer;

    char *protocol;

    char localSrcIP[16], localDestIP[16];

    struct sockaddr_in src, dest; //IP-адреса источника и назначения в пакете
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = ipHead->daddr; //ip

    memset(&src, 0, sizeof(src));
    src.sin_addr.s_addr = ipHead->saddr;

    receiveBytes += ntohs((ipHead)->tot_len); // Обновление статистики полученных байт и пакетов
    receivePacks += 1;

    printf(
        "\r%sСaptured packs:%s TCP %d, UDP %d, ICMP %d, ARP %d, Other %d, %sTotal packs:%s %ld, %sTotal bytes:%s %ld ",
        GREEN, RESET, tcp, udp, icmp, arp, other, GREEN, RESET, receivePacks, YELLOW, RESET,
        receiveBytes);
    fflush(stdout); //чтобы строка на месте стояла

    strncpy(localSrcIP, inet_ntoa(src.sin_addr), 16); // преобразование IP-адресов в строковый формат
    strncpy(localDestIP, inet_ntoa(dest.sin_addr), 16);

    //Если запросы сами на себя - не учитываем
    if (strcmp(localSrcIP, localDestIP) == 0)
        return 1;

    //Если задан фильтр на SourceIP - проверяем полное соответсвие
    if (srcIP[0] != '\0' && strcmp(srcIP, localSrcIP) != 0)
        return 1;

    //Если задан фильтр на DestinationIP - проверяем полное соответсвие
    if (destIP[0] != '\0' && strcmp(destIP, localDestIP) != 0)
        return 1;

    // переключение на основе протокола кадра
    // Определение протокола Ethernet
    switch (ntohs(ethHead->h_proto)) {
        case ETH_P_IP: // IPv4

            // Если фильтр установлен на ARP, игнорируем пакет IP
                // if protocol is ARP, return
                    if (filterProtocol == 'A')
                        return 1;

        switch (ipHead->protocol) {
            case 1: // ICMP
                if (filterProtocol != 'I' && filterProtocol != 'X')
                    return 1;

            icmp++;
            protocol = "ICMP";
            break;

            case 6: // TCP
                    if (filterProtocol != 'T' && filterProtocol != 'X')
                        return 1;

            tcp++;
            protocol = "TCP";
            break;

            case 17: // UDP
                    if (filterProtocol != 'U' && filterProtocol != 'X')
                        return 1;

            udp++;
            protocol = "UDP";
            break;

            default: // Other Protocols
                // filter for All
                    if (filterProtocol != 'X')
                        return 1;
            other++;
            protocol = "Other";
            break;
        }
        break;


        case ETH_P_ARP: // ARP
            if (filterProtocol != 'A' && filterProtocol != 'X')
                return 1;
        // Если задано исходный или целевой IP, игнорируем ARP пакет
        if (srcIP[0] != '\0' || destIP[0] != '\0')
            return 1;

        arp++;
        protocol = "ARP";
        break;

        default:
            if (filterProtocol != 'X')
                return 1;

        if (srcIP[0] != '\0' || destIP[0] != '\0')
            return 1;

        other++;
        protocol = "Other";
        break;
    }

    //Добавление записи
    char key[50];
    snprintf(key, sizeof(key), "%s|%s|%s", localSrcIP, localDestIP, protocol);
    mapAdd(key, ntohs((ipHead)->tot_len), resultStat);
    return 0;
}
