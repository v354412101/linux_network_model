#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>

#define BUFFER_LENGTH	1024

int main(void) {
    int ret = -1;
    int server_ip_port = 10008;

    struct sockaddr_in t_sockaddr;
    memset(&t_sockaddr, 0, sizeof(t_sockaddr));
    t_sockaddr.sin_family = AF_INET;
    t_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    t_sockaddr.sin_port = htons(server_ip_port);

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listen_fd > 0);

    ret = bind(listen_fd,(struct sockaddr *) &t_sockaddr,sizeof(t_sockaddr));
    assert(ret != -1);

    ret = listen(listen_fd, 1024);
    assert(ret != -1);

    fd_set rfds, rset;;
    FD_ZERO(&rfds);
    FD_SET(listen_fd, &rfds);

    int max_fd = listen_fd;

    while(1) {
        rset = rfds;
        int nready = select(max_fd + 1, &rset, NULL, NULL, NULL);
        if (nready < 0) {
            continue;
        }

        if (FD_ISSET(listen_fd, &rset)) {
            struct sockaddr_in client_addr;
            memset(&client_addr, 0, sizeof(struct sockaddr_in));
            socklen_t client_len = sizeof(client_addr);
            int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
            if (client_fd <= 0) {
                continue;
            }

            char str[INET_ADDRSTRLEN] = {0};
            printf("recvived from %s at port %d, sockfd:%d, client_fd:%d\n", inet_ntop(AF_INET, &client_addr.sin_addr, str, sizeof(str)),
                   ntohs(client_addr.sin_port), listen_fd, client_fd);

            if (max_fd == FD_SETSIZE) {
                printf("client_fd --> out range\n");
                break;
            }
            FD_SET(client_fd, &rset);

            if (client_fd > max_fd) {
                max_fd = client_fd;
            }

            printf("sockfd:%d, max_fd:%d, client_fd:%d\n", listen_fd, max_fd, client_fd);

            if (--nready == 0) {
                continue;
            }
        }

        for (int i = listen_fd + 1;i <= max_fd; i++) {
            if (FD_ISSET(i, &rset)) {
                char buffer[BUFFER_LENGTH] = {0};
                ret = recv(i, buffer, BUFFER_LENGTH, 0);
                if (ret < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        printf("read all data");
                    }
                    FD_CLR(i, &rfds);
                    close(i);
                } else if (ret == 0) {
                    printf(" disconnect %d\n", i);
                    FD_CLR(i, &rfds);
                    close(i);
                    break;
                } else {
                    printf("Recv: %s, %d Bytes\n", buffer, ret);
                }
                if (--nready == 0) {
                    break;
                }
            }
        }
    }

    close(listen_fd);

    return 0;
}


