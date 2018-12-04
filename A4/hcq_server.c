#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hcq.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#ifndef PORT
  #define PORT 50640
#endif
#define MAX_BACKLOG 5
#define BUF_SIZE 128

#define INPUT_BUFFER_SIZE 256
#define INPUT_ARG_MAX_NUM 3  
#define DELIM " \n"


struct sockname {
    int state;
    int sock_fd;
    int is_ta;
    char *username;
    struct sockname *next;
    char buf[32];
    int offset;
};

/* Print a formatted error message to stderr.
 */
void error(char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
}

// Use global variables so we can have exactly one TA list and one student list
Ta *ta_list = NULL;
Student *stu_list = NULL;

Course *courses;  
int num_courses = 3;


/* Accept a connection. Note that a new file descriptor is created for
 * communication with the client. The initial socket descriptor is used
 * to accept connections, but the new socket is used to communicate.
 * Return the new client's file descriptor or -1 on error.
 */
int accept_connection(int fd, struct sockname *usernames) {

    int client_fd = accept(fd, NULL, NULL);
    if (client_fd < 0) {
        perror("server: accept");
        close(fd);
        exit(1);
    }

    struct sockname *new_client = malloc(sizeof(struct sockname));
    new_client->is_ta = -1;
    new_client->next = NULL;
    new_client->sock_fd = fd;
    new_client->state = 0;
    new_client->username = NULL;

    if(usernames == NULL) {
        usernames = new_client;
    } else {
        struct sockname *head = usernames;
        while (usernames->next != NULL) {
            usernames = usernames->next;
        }
        usernames->next = new_client;
        usernames = head;
    }

    dprintf(client_fd, "Welcome to the Help Centre, what is your name\r\n");

    return client_fd;
}


/* Read a message from client_index and echo it back to them.
 * Return the fd if it has been closed or 0 otherwise.
 */
int read_from(int fd, struct sockname *usernames) {

    // 读 client
    char buf[32];
    int num_read = read(fd, &buf, 32);
    if (num_read == 0) {
        return fd;
    }

    // command 太长
    int index = num_read + usernames->offset;
    if (index > 32) {
        return -1;
    }

    // 写进 buf
    usernames->offset += sprintf(usernames->buf + usernames->offset, buf);
    
    // 没读完
    if(usernames->buf[index] != '\n') {
        return -2;
    }

    return 0;
}


int main(void) {
    if ((courses = malloc(sizeof(Course) * 3)) == NULL) {
        perror("malloc for course list\n");
        exit(1);
    }
    strcpy(courses[0].code, "CSC108");
    strcpy(courses[1].code, "CSC148");
    strcpy(courses[2].code, "CSC209");
    

    struct sockname *usernames = NULL;

    // Create the socket FD.
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("server: socket");
        exit(1);
    }

    // Set information about the port (and IP) we want to be connected to.
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    // This should always be zero. On some systems, it won't error if you
    // forget, but on others, you'll get mysterious errors. So zero it.
    memset(&server.sin_zero, 0, 8);

    int on = 1;
    int status = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR,
                            (const char *) &on, sizeof(on));
    if(status == -1) {
        perror("setsockopt -- REUSEADDR");
    }

    // Bind the selected port to the socket.
    if (bind(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("server: bind");
        close(sock_fd);
        exit(1);
    }

    // Announce willingness to accept connections on this socket.
    if (listen(sock_fd, MAX_BACKLOG) < 0) {
        perror("server: listen");
        close(sock_fd);
        exit(1);
    }

    // The client accept - message accept loop. First, we prepare to listen to multiple
    // file descriptors by initializing a set of file descriptors.
    int max_fd = sock_fd;
    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(sock_fd, &all_fds);

    while (1) {
        // select updates the fd_set it receives, so we always use a copy and retain the original.
        fd_set listen_fds = all_fds;
        int nready = select(max_fd + 1, &listen_fds, NULL, NULL, NULL);
        if (nready == -1) {
            perror("server: select");
            exit(1);
        }

        // Is it the original socket? Create a new connection ...
        if (FD_ISSET(sock_fd, &listen_fds)) {
            int client_fd = accept_connection(sock_fd, usernames);
            if (client_fd > max_fd) {
                max_fd = client_fd;
            }
            FD_SET(client_fd, &all_fds);
        }

        // Next, check the clients.
        // NOTE: We could do some tricks with nready to terminate this loop early.
        for (int fd_index = 0; fd_index < max_fd; fd_index++) {
            if (FD_ISSET(fd_index, &listen_fds)) {

                struct sockname *head = usernames;
                while(usernames->sock_fd != fd_index) {
                    usernames = usernames->next;
                }
                struct sockname *cur_client = usernames;
                usernames = head;

                // Note: never reduces max_fd
                int client_closed = read_from(fd_index, cur_client);
                if (client_closed > 0) {
                    FD_CLR(client_closed, &all_fds);
                    printf("Client %d disconnected\n", client_closed);
                } else if (client_closed == 0) {
                    // 正常接收
                    printf("The msg is: %s", cur_client->buf);
                } else if (client_closed == -1) {
                    // client 被踢了
                    if(close(fd_index) == -1) {
                        perror("close fd: read_from");
                        exit(1);
                    }
                    printf("Client %d kicked\n", client_closed);
                } else if (client_closed == -2) {
                    // 没读完
                    printf("read is not complete");
                }
            }
        }
    }

    // Should never get here.
    return 1;
}
