/**
 * 
 * 
 * 
 * 
 * 
 * 
*/

#include "lwip/opt.h"

#if LWIP_SOCKET && (LWIP_IPV4 || LWIP_IPV6)

#include "lwip/sockets.h"
#include "lwip/sys.h"

#include <string.h>
#include <stdio.h>

// Set your IP Here 
#ifndef SOCK_TARGET_HOST4
#define SOCK_TARGET_HOST4  "192.168.0.1" 
#endif
// Set your Port Here 
#ifndef SOCK_TARGET_PORT
#define SOCK_TARGET_PORT  80
#endif

int main() {
    int s;
    int ret;
    int opt;
    struct sockaddr_in addr;
    fd_set readset;
    struct timeval tv;
    char rxbuf[1024]; // Adjust buffer size as needed

    /* set up address to connect to */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = PP_HTONS(SOCK_TARGET_PORT);
    inet_aton(SOCK_TARGET_HOST4, &addr.sin_addr);

    /* create the socket */
    s = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        DEBUGF("lwip_socket"); // <-- Pseudo code
        return 1;
    }

    /* nonblocking */
    opt = lwip_fcntl(s, F_GETFL, 0);
    if (opt < 0) {
        DEBUGF("lwip_fcntl"); // <-- Pseudo code
        return 1;
    }
    opt |= O_NONBLOCK;
    ret = lwip_fcntl(s, F_SETFL, opt);
    if (ret < 0) {
        perror("lwip_fcntl");DEBUGF("lwip_fcntl"); // <-- Pseudo code
        return 1;
    }

    /* connect */
    while (true)
    {
        ret = lwip_connect(s, (struct sockaddr*)&addr, sizeof(addr));
        if (ret < 0 && errno != EINPROGRESS) {
            perror("lwip_connect"); DEBUGF("lwip_fcntl"); // <-- Pseudo code
            return 1;
        }

    }
    
   
    while (1) {
        FD_ZERO(&readset);
        FD_SET(s, &readset);
        tv.tv_sec = 1; // Adjust timeout as needed
        tv.tv_usec = 0;

        ret = lwip_select(s + 1, &readset, NULL, NULL, &tv);
        if (ret < 0) {
            DEBUGF("lwip_select"); // <-- Pseudo code
            break;
        }

        if (ret == 0) {
            printf("No data received, retrying...\n"); 
            continue;
        }

        if (FD_ISSET(s, &readset)) {
            ret = lwip_read(s, rxbuf, sizeof(rxbuf) - 1);
            if (ret <= 0) {
               DEBUGF("lwip_read"); // <-- Pseudo code
                break;
            }

            rxbuf[ret] = '\0';
            printf("Received data: %s\n", rxbuf); 
        }
    }

    /* close */
    ret = lwip_close(s);
    if (ret < 0) {
         DEBUGF("lwip_close"); // <-- Pseudo code
        return 1;
    }

    return 0;
}
#endif /* LWIP_SOCKET */