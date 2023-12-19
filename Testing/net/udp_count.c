
#include "network.h"
#include "tools.h"


int main()
{
    int    cc, sock;
    struct sockaddr_in addr;
    char   packet[5];
    unsigned int* sz;

    sz = (unsigned int*)(&packet[1]);

    sock = udp_server_socket_ipv4(12000);

    Loop {
        cc = udp_recv_sockaddr_in(sock, packet, 5, &addr);
        print_message("%02x: %d\n", packet[0], *sz);
    }
}
