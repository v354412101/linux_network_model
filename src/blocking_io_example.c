#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#define MAX_READ_LINE 10240

static void * client_callback(void *arg) {
    int conn_fd = *(int *)arg;
    char buff[MAX_READ_LINE] = {0};

    int recv_len = recv(conn_fd, buff, MAX_READ_LINE, 0);
    assert(recv_len > 0);

    buff[recv_len] = '\0';
    fprintf(stdout, "recv message from client: %s\n", buff);

    close(conn_fd);

    return NULL;
}

int main(void) {
    struct sockaddr_in t_sockaddr;
    memset(&t_sockaddr, 0, sizeof(t_sockaddr));
    t_sockaddr.sin_family = AF_INET;
    t_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    t_sockaddr.sin_port = htons(10008);

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listen_fd > 0);

    int ret = bind(listen_fd, (struct sockaddr *)&t_sockaddr, sizeof(t_sockaddr));
    assert(ret != -1);

    ret = listen(listen_fd, 1024);
    assert(ret != -1);

    while(1) {
        int conn_fd = accept(listen_fd, (struct sockaddr*)NULL, NULL);
        assert(conn_fd > 0);

        pthread_t th_id;
        ret = pthread_create(&th_id, NULL, client_callback, &conn_fd);
        assert(ret == 0);
    }

    close(listen_fd);
    return 0;
}