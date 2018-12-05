#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hcq.h"
#include <stdarg.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

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
    char buf[33];
    int offset;
    char *course;
};

int dprintf(int fd, const char *format, ...);

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


/* helper function : client use Ctrl+C to disconnect
 */
void client_disconnect(struct sockname* cur_client) {
    if (cur_client->is_ta == 0) { // cur_client is Student
        if (cur_client->state > 2) {
            give_up_waiting(&stu_list, cur_client->username);
            // printf("i = %d\r\n", i);
            // printf("Student give up!!!!!!\r\n");
        }
    } else { // cur_client is Ta
        if (cur_client->state > 0) {
            // Ta *cur_ta = find_ta(ta_list, cur_ta->username);
            remove_ta(&ta_list, cur_client->username);
        }
    }

    if(close(cur_client->sock_fd) == -1) {
        perror("close fd");
        exit(1);
    }
    cur_client->sock_fd = -1;
    // printf("Client %d disconnect\r\n", cur_client->sock_fd);
}


/* helper function : close this client
 */
void client_kicked(struct sockname* cur_client) {
    if(close(cur_client->sock_fd) == -1) {
        perror("close fd");
        exit(1);
    }
    cur_client->sock_fd = -1;
    // printf("Client %d kicked\r\n", cur_client->sock_fd);
}

/* Find client in usernames with fd
 * Return the client or NULL if not found.
 */
struct sockname* find_client(int fd, struct sockname* usernames) {
    struct sockname *head = usernames;
    while(usernames != NULL && usernames->sock_fd != fd) {
        usernames = usernames->next;
    }
    struct sockname *cur_client = usernames;
    usernames = head;

    return cur_client;
}

/* Find student client for current ta
 * Return the client for that student.
 */
struct sockname* find_stu_client(Ta *cur_ta, struct sockname* usernames) {
    struct sockname *head = usernames;
    while((usernames->is_ta != 0) || (strcmp(usernames->username, cur_ta->current_student->name) != 0) || (usernames->sock_fd == -1)) {
        // printf("Check Next\r\n");
        usernames = usernames->next;
    }
    struct sockname *cur_client = usernames;
    usernames = head;

    return cur_client;
}

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
    new_client->sock_fd = client_fd;
    new_client->state = 0;
    new_client->username = NULL;
    new_client->offset = 0;
    new_client->course = NULL;


    struct sockname *head = usernames;
    while (usernames->next != NULL ) {
        usernames = usernames->next;
    }
    usernames->next = new_client;
    usernames = head;


    dprintf(client_fd, "Welcome to the Help Centre, what is your name?\r\n");

    return client_fd;
}


/* Read a message from client_index and echo it back to them.
 * Return the fd if it has been closed.
 * Return 0 if read is valid and complete.
 * Return -1 if command too long.
 * Return -2 if read is not complete.
 */
int read_from(int fd, struct sockname *usernames) {

    // read from client
    char buf[33];
    int num_read = read(fd, &buf, 32);
    if (num_read == 0) {
        return fd;
    }
    buf[num_read] = '\0';
    int offset = usernames->offset;

    // command too long
    int index = num_read + offset;
    if (index > 32 || (num_read == 32 && buf[31] != '\n')) {
        return -1;
    }

    // write in client_buf
    // char *to_write = malloc(33);
    // strcpy(to_write, buf);
    offset += sprintf(usernames->buf + usernames->offset, "%s", buf);
    // free(to_write);
    
    // read is not complete
    if(usernames->buf[offset - 1] != '\n') {
        printf("buf = %s*\r\n", buf);
        return -2;
    }

    usernames->buf[offset - 2] = '\0'; // replace \r to \0
    usernames->offset = offset - 2;
    // reach here if read is valid and complete
    return 0;
}

/* Check state for command line
 * Return the client or NULL if not found.
 */
void check_state(struct sockname* cur_client, struct sockname* usernames) {
    int state = cur_client->state;
    int offset = cur_client->offset;
    if (state == 0) { // wait for name
        cur_client->username = malloc(offset + 1);
        strcpy(cur_client->username, cur_client->buf);
        cur_client->state = 1;
        dprintf(cur_client->sock_fd, "Are you a TA or a Student (Enter T or S)?\r\n");
        
    } else if (state == 1) { // wait for role
        if (strcmp(cur_client->buf, "S") == 0) {
            cur_client->is_ta = 0;
            cur_client->state = 2;
            dprintf(cur_client->sock_fd, "Valid courses: CSC108, CSC148, CSC209\r\nWhich course are you asking about\r\n");

        } else if (strcmp(cur_client->buf, "T") == 0) {
            cur_client->is_ta = 1;
            cur_client->state = 3;
            add_ta(&ta_list, cur_client->username);
            dprintf(cur_client->sock_fd, "Valid commands for TA:\r\n\tstats\r\n\tnext\r\n\t(or use Ctrl+C to leave)\r\n");
        } else {
            dprintf(cur_client->sock_fd, "Invalid role (enter T or S)\r\n");
        }

    } else if (state == 2) { // state for student type in course
        if (strcmp(cur_client->buf, "CSC108") == 0 || strcmp(cur_client->buf, "CSC148") == 0 || strcmp(cur_client->buf, "CSC209") == 0) {
            cur_client->course = malloc(offset + 1);
            strcpy(cur_client->course, cur_client->buf);
            cur_client->state = 4;
            add_student(&stu_list, cur_client->username, cur_client->course, courses, num_courses);

            dprintf(cur_client->sock_fd, "You have been entered into the queue. While you wait, you can use the command stats to see which TAs are currently serving students.\r\n");
        } else {
            cur_client->state = -1;
            dprintf(cur_client->sock_fd, "This is not a valid course. Good-bye.\r\n");
        }
        

    } else if (state == 3) { // state for Ta type in command
        if (strcmp(cur_client->buf, "stats") == 0) { // if Ta type in stats
            char *to_print = print_full_queue(stu_list);
            dprintf(cur_client->sock_fd, to_print);

        } else if (strcmp(cur_client->buf, "next") == 0) { // if Ta type in next
            next_overall(cur_client->username, &ta_list, &stu_list);
            Ta *cur_ta = find_ta(ta_list, cur_client->username);
            struct sockname *stu_client = find_stu_client(cur_ta, usernames);
            // printf("the fd of stu_client = %d\r\n", stu_client->sock_fd);
            // printf("the name of stu_client = %s\r\n", stu_client->username);
            // printf("the state of stu_client = %d\r\n", stu_client->state);
            // printf("the course of stu_client = %s\r\n", stu_client->course);
            // printf("the is_ta of stu_client = %d\r\n", stu_client->is_ta);
            dprintf(stu_client->sock_fd, "Your turn to see the TA.\r\nWe are disconnecting you from the server now. Press Ctrl-C to close nc\r\n");
            // stu_client->state = -1;

        } else { // Invalid command
            dprintf(cur_client->sock_fd, "Incorrect syntax\r\n");
        }

    } else if (state == 4) { // state for student type in stats
        if (strcmp(cur_client->buf, "stats") == 0) {
            char *to_print = print_currently_serving(ta_list);
            dprintf(cur_client->sock_fd, to_print);
        } else {
            dprintf(cur_client->sock_fd, "Incorrect syntax\r\n");
        }
    }

    cur_client->offset = 0; // reset the offset for new command
}




int main(void) {
    if ((courses = malloc(sizeof(Course) * 3)) == NULL) {
        perror("malloc for course list\n");
        exit(1);
    }
    strcpy(courses[0].code, "CSC108");
    strcpy(courses[1].code, "CSC148");
    strcpy(courses[2].code, "CSC209");
    

    struct sockname *usernames = malloc(sizeof(struct sockname));
    usernames->is_ta = -1;
    usernames->next = NULL;
    usernames->sock_fd = -1;
    usernames->state = -2;
    usernames->username = "Head";

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
        for (int fd_index = 4; fd_index < max_fd + 1; fd_index++) {
            struct sockname* cur_client = find_client(fd_index, usernames);

            if (FD_ISSET(fd_index, &listen_fds) && cur_client != NULL) {
                // printf("fd is %d \r\n", cur_client->sock_fd);
                // Note: never reduces max_fd
                int client_closed = read_from(fd_index, cur_client);
                if (client_closed > 0) {
                    FD_CLR(cur_client->sock_fd, &all_fds);
                    client_disconnect(cur_client);
                    
                } else if (client_closed == 0) {// read complete
                    // printf("The msg is: %s\r\n", cur_client->buf);
                    check_state(cur_client, usernames);
                    if (cur_client->state == 3 && (strcmp(cur_client->buf, "next") == 0)) {
                        Ta *cur_ta = find_ta(ta_list, cur_client->username);
                        struct sockname *stu_client = find_stu_client(cur_ta, usernames);
                        FD_CLR(stu_client->sock_fd, &all_fds);
                        client_kicked(stu_client);
                        // close(4);
                        // FD_CLR(4, &all_fds);
                    }
                    if (cur_client->state == -1) { // client been kicked
                        FD_CLR(cur_client->sock_fd, &all_fds);
                        client_disconnect(cur_client);
                    }
                    // printf("The name is: %s\r\n", cur_client->username);
                    // printf("The state is: %d\r\n", cur_client->state);

                } else if (client_closed == -1) {// client is kicked because command too long
                    FD_CLR(cur_client->sock_fd, &all_fds);
                    // printf("close fd = %d\r\n", cur_client->sock_fd);
                    client_disconnect(cur_client);

                } else if (client_closed == -2) {// read not complete
                    printf("read is not complete\r\n");
                }
            }
        }
    }

    // Should never get here.
    return 1;
}
