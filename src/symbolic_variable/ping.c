/*
*Read ping result as symbolic variables, if the address is not reachable,the bomb would be triggered.
*Evaluation:
--Triton: fail
--Angr:
--BAP:
*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/select.h>

//note, to allow root to use icmp sockets, run:
//sysctl -w net.ipv4.ping_group_range="0 10"

int ping_it(struct in_addr *dst)
{
    struct icmphdr icmp_hdr;
    struct sockaddr_in addr;
    int sequence = 0;
    int sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_ICMP);
    if (sock < 0) {
        perror("socket");
        return 0;
    }

    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr = *dst;

    memset(&icmp_hdr, 0, sizeof icmp_hdr);
    icmp_hdr.type = ICMP_ECHO;
    icmp_hdr.un.echo.id = 1234;//arbitrary id

    int count = 0;
    for(; count < 4; count ++) {
        unsigned char data[2048];
        int rc;
        struct timeval timeout = {3, 0}; //wait max 3 seconds for a reply
        fd_set read_set;
        socklen_t slen;
        struct icmphdr rcv_hdr;

        icmp_hdr.un.echo.sequence = sequence++;
        memcpy(data, &icmp_hdr, sizeof icmp_hdr);
        memcpy(data + sizeof icmp_hdr, "hello", 5); //icmp payload
        rc = sendto(sock, data, sizeof icmp_hdr + 5,
                        0, (struct sockaddr*)&addr, sizeof addr);
        if (rc <= 0) {
            perror("Sendto");
            break;
        }
        puts("Sent ICMP\n");

        memset(&read_set, 0, sizeof read_set);
        FD_SET(sock, &read_set);

        //wait for a reply with a timeout
        rc = select(sock + 1, &read_set, NULL, NULL, &timeout);
        if (rc == 0) {
            puts("Got no reply\n");
            continue;
        } else if (rc < 0) {
            perror("Select");
            break;
        }

        //we don't care about the sender address in this example..
        slen = 0;
        rc = recvfrom(sock, data, sizeof data, 0, NULL, &slen);
        if (rc <= 0) {
            perror("recvfrom");
            break;
        } else if (rc < sizeof rcv_hdr) {
            printf("Error, got short ICMP packet, %d bytes\n", rc);
            break;
        }
        memcpy(&rcv_hdr, data, sizeof rcv_hdr);
        if (rcv_hdr.type == ICMP_ECHOREPLY) {
            printf("ICMP Reply, id=0x%x, sequence =  0x%x\n",
                            icmp_hdr.un.echo.id, icmp_hdr.un.echo.sequence);
        } else {
            printf("Got ICMP packet with type 0x%x ?!?\n", rcv_hdr.type);
        }
    }
    return 1;
}

#include "a_tester.h"

int sym_checker(char* s) {
    struct in_addr dst;

    if (inet_aton(s, &dst) == 0) {
        perror("inet_aton");
        printf("%s isn't a valid IP address\n", s);
        return 1;
    }

    if (ping_it(&dst)){
        return BOMB_ENDING;
    }else{
        return NORMAL_ENDING;
    }
    return 0;
}
