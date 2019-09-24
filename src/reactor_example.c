#include <stdio.h>
#include <time.h>
#include <sys/epoll.h>

#define BUFFER_LENGTH 1024

typedef int NCALLBACK(int fd, int events, void *arg);

struct ntyevent {
    int fd;
    int events;
    void *arg;
    NCALLBACK callback; //read write
    char buffer[BUFFER_LENGTH];
    int length;
    int status;
    long last_active;
};

int nty_event_set(struct ntyevent *ev, int fd, NCALLBACK cb, void *arg) {
    ev->fd = fd;
    ev->callback = cb;
    ev->events = 0;
    ev->arg = arg;
    ev->last_active = time(NULL);
}

int nty_event_add(int epfd, int events, struct ntyevent *ev) {
    struct epoll_event ep_ev = {0};
    ep_ev.data.ptr = ev;
    ep_ev.events = ev->events = events;

    int oper;
    if(ev->status == 1) {
        oper = EPOLL_CTL_MOD;
    } else {
        oper = EPOLL_CTL_ADD;
        ev->status = 1;
    }

    epoll_ctl(epfd, oper, ev->fd, &ep_ev);
}

int nty_event_del(int epfd, struct ntyevent *ev) {

}


int main() {

}

