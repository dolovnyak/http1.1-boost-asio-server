#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <thread>

#define BUF_SIZE 128
#define PORT 8080
#define MAX_CONNECTIONS_NUMBER 10000
#define EVENTS_NUM 256
#define WORKER_THREAD_NUM 16

const char* hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

bool epoll_ctl_add(int epoll_fd, int fd, uint32_t events) {
    struct epoll_event event{};
    event.events = events;
    event.data.fd = fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
        return false;
    }
    return true;
}

bool epoll_ctl_mod(int epoll_fd, int fd, int events) {
    struct epoll_event event{};
    event.events = events;
    event.data.fd = fd;
    int ret = ::epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
    return ret;
}

void event_loop(int epoll_fd, int listening_socket_fd, struct sockaddr_in address, int current_thread) {
    /// add edge-triggered epoll listener
    socklen_t address_len = sizeof(address);
    char buf[BUF_SIZE];
    struct epoll_event events[EVENTS_NUM];


    for (;;) {
        int events_number = epoll_wait(epoll_fd, events, EVENTS_NUM, -1);
        //std::cout << "thread " << current_thread << " woke up" << std::endl;
        if (events_number == -1) {
            std::cerr << "epoll_wait failed" << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < events_number; ++i) {
            if (events[i].data.fd == listening_socket_fd) {
                /// handle new connection
                for (;;) {
                    int connection_socket = accept4(listening_socket_fd, (struct sockaddr*)&address, &address_len, SOCK_NONBLOCK | SOCK_CLOEXEC);
                    if (connection_socket == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            (void)current_thread;
//                            std::cout << "cur thread: " << current_thread << " processed all current connections" << std::endl;
                            break;
                        } else {
                            std::cerr << "accept failed" << strerror(errno) << std::endl;
                            exit(EXIT_FAILURE);
                        }
                    } else {
//                        inet_ntop(AF_INET, (char*)&(address.sin_addr), buf, sizeof(address));
//                        std::cout << "[+] connected with " << buf << ":" << ntohs(address.sin_port) << std::endl;

//                        std::cout << "accept connection " << connection_socket << " in thread " << current_thread << std::endl;
                        /// add edge-triggered epoll listener
                        if (!epoll_ctl_add(epoll_fd, connection_socket,
                                           EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP | EPOLLONESHOT)) {
                            std::cerr << "epoll_ctl_add failed" << strerror(errno) << std::endl;
                            exit(EXIT_FAILURE);
                        }
                    }
                }
            } else if (events[i].events & EPOLLIN) {
//                std::cout << "read fd " << events[i].data.fd << " in thread " << current_thread << std::endl;
                /// handle read event
                for (;;) {
                    ssize_t bytes_num = read(events[i].data.fd, buf, sizeof(buf));
                    if (bytes_num == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
//                            epoll_ctl_mod(epoll_fd, events[i].data.fd, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP | EPOLLONESHOT);
//                            std::cout << "finished reading data from connection " << events[i].data.fd << " in thread " << current_thread << std::endl;
                            bytes_num = write(events[i].data.fd, hello, strlen(hello));
                            (void)bytes_num;
                            close(events[i].data.fd);
                            break;
                        } else {
                            std::cerr << "read failed" << strerror(errno) << std::endl;
                            exit(EXIT_FAILURE);
                        }
                    } else if (bytes_num == 0) {
//                        std::cout << "bytes_num == 0 " << events[i].data.fd << std::endl;
                        close(events[i].data.fd);
                        break;
                    } else {
                        epoll_ctl_mod(epoll_fd, events[i].data.fd, EPOLLIN | EPOLLET | EPOLLONESHOT);
//                        std::cout << "AAA" << std::endl;
//                        fwrite(buf, sizeof(char), bytes_num, stdout);
//                        std::cout << "BBB" << std::endl;
                    }
                }
            } else {
                /// check if the connection is closing
                if (events[i].events & (EPOLLRDHUP | EPOLLHUP)) {
                    std::cout << "connection closed" << std::endl;
                    close(events[i].data.fd);
                    continue;
                } else {
                    std::cout << "connection error" << std::endl;
                    close(events[i].data.fd);
                    continue;
                }
            }
        }
    }
}

int main() {
    /// create non blocking socket and listen it
    int listening_socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (listening_socket_fd < 0) {
        std::cerr << "listening_socket creation failed" << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address{};
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    if (bind(listening_socket_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "bind creation failed" << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(listening_socket_fd, MAX_CONNECTIONS_NUMBER) < 0) {
        std::cerr << "listen failed" << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    int epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd < 0) {
        std::cerr << "epoll_fd creation failed" << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!epoll_ctl_add(epoll_fd, listening_socket_fd, EPOLLIN | EPOLLHUP | EPOLLET)) {
        std::cerr << "epoll_ctl_add failed" << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    std::thread workers[WORKER_THREAD_NUM];
    for (int i = 0; i < WORKER_THREAD_NUM; ++i) {
        workers[i] = std::thread(event_loop, epoll_fd, listening_socket_fd, address, i);
    }
    for (auto& worker : workers) {
        worker.join();
    }

    return 0;
}
