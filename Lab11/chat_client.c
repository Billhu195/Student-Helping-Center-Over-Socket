#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#ifndef PORT
  #define PORT 50639
#endif
#define BUF_SIZE 128

int main(void) {
    // Create the socket FD.
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("client: socket");
        exit(1);
    }

    // Set the IP and port of the server to connect to.
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) < 1) {
        perror("client: inet_pton");
        close(sock_fd);
        exit(1);
    }

    // Connect to the server.
    if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("client: connect");
        close(sock_fd);
        exit(1);
    }

    char username[BUF_SIZE + 1];
    int num_name = read(STDIN_FILENO, username, BUF_SIZE);
    username[num_name] = '\0';   

    int write_user = write(sock_fd, username, BUF_SIZE);
    if (write_user < num_name) {
        perror("name write");
    }

    // Read input from the user, send it to the server, and then accept the
    // echo that returns. Exit when stdin is closed.
    char buf[BUF_SIZE + 1];
    int max_fd = sock_fd;
    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(max_fd, &all_fds);
    FD_SET(STDIN_FILENO, &all_fds);
    while (1) {
        fd_set listen_fds = all_fds;

        if( select(max_fd+1, &listen_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            close(sock_fd);
            return -1;
        }

        if(FD_ISSET(STDIN_FILENO, &listen_fds))
        {
            int bytes_read = read(STDIN_FILENO, buf, BUF_SIZE);
            buf[bytes_read] = '\0'; 
            write(sock_fd, buf, bytes_read);
        }

        if(FD_ISSET(sock_fd, &listen_fds))
        {
            int bytes_read = read(sock_fd, buf, BUF_SIZE);
            buf[bytes_read] = '\0';
            write(STDOUT_FILENO, buf, bytes_read);
        }

        
    }

    close(sock_fd);
    return 0;
}
