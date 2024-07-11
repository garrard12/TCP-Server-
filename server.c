#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

char *dir;
void *handle_connection(void *id);

int main(int argc, char **argv) {
    // Disable output buffering
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    if (argc < 3 || strcmp(argv[1], "--directory") != 0 || strlen(argv[2]) < 1) {
        printf("Usage: %s --directory <dir>\n", argv[0]);
        return 1;
    }
    
    dir = argv[2];

    printf("Logs from your program will appear here!\n");

    int server_fd, client_addr_len;
    struct sockaddr_in client_addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        printf("Socket creation failed: %s...\n", strerror(errno));
        return 1;
    }

    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        printf("SO_REUSEADDR failed: %s \n", strerror(errno));
        return 1;
    }

    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(4221),
        .sin_addr = { htonl(INADDR_ANY) },
    };

    if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
        printf("Bind failed: %s \n", strerror(errno));
        return 1;
    }

    int connection_backlog = 10;
    if (listen(server_fd, connection_backlog) != 0) {
        printf("Listen failed: %s \n", strerror(errno));
        return 1;
    }

    while (1) {
        client_addr_len = sizeof(client_addr);
        int id = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *)&client_addr_len);
        if (id == -1) {
            printf("Accept failed: %s\n", strerror(errno));
            continue;
        }
        printf("Client connected %i\n", id);

        pthread_t new_process;
        int *pid = malloc(sizeof(int));
        if (pid == NULL) {
            printf("Memory allocation failed\n");
            close(id);
            continue;
        }
        *pid = id;
        pthread_create(&new_process, NULL, handle_connection, pid);
        pthread_detach(new_process); // Detach the thread to avoid memory leaks
    }

    printf("Closing server socket %i\n", server_fd);
    close(server_fd);
    return 0;
}

void *handle_connection(void *pid) {
    int id = *((int *)pid);
    free(pid);

    char *reply_200 = "HTTP/1.1 200 OK\r\n\r\n";
    char *reply_404 = "HTTP/1.1 404 Not Found\r\n\r\n";

    char buffer[1024];
    int message_status = read(id, buffer, sizeof(buffer) - 1);
    if (message_status < 0) {
        printf("ERROR READING %s\n", strerror(errno));
        close(id);
        return NULL;
    }
    buffer[message_status] = 0x00;
    printf("Buffer %s \n", buffer);

    char buffer_message[1024];
    strcpy(buffer_message, buffer);

    char *path = strtok(buffer, " ");
    path = strtok(NULL, " ");
    printf("Path message %s\n", path);

    char response_buffer[999];
    char *response = NULL;
    if (strstr(path, "user-agent")) {
        printf("USER AGENT %s \n", path);
        char *user_agent = strstr(buffer_message, "User-Agent:") + 12;
        user_agent = strtok(user_agent, "\r");
        printf("User agent %s\n", user_agent);

        sprintf(response_buffer, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %lu\r\n\r\n%s", strlen(user_agent), user_agent);
        response = response_buffer;
    } else if (strstr(path, "/echo")) {
        printf("String has echo\n");

        char *word_from_server = strtok(path, "/");
        word_from_server = strtok(NULL, "/");
        printf("Word from server %s\n", word_from_server);
        sprintf(response_buffer, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %lu\r\n\r\n%s", strlen(word_from_server), word_from_server);
        response = response_buffer;
    } else if (strstr(path, "/files")) {
        char *file_from_server = strtok(path, "/");
        file_from_server = strtok(NULL, "/");
        printf("File name --> %s \n", file_from_server);

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, file_from_server);
        printf("File path %s\n", full_path);

        FILE *fptr = fopen(full_path, "r");
        if (fptr == NULL) {
            printf("FILE DOESN'T EXIST %s does not exist\n", full_path);
            response = reply_404;
        } else {
            printf("File open %p\n", fptr);
            char file_content[1024];
            fgets(file_content, sizeof(file_content), fptr);
            printf("File contents %s\n", file_content);

            sprintf(response_buffer, "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: %lu\r\n\r\n%s", strlen(file_content), file_content);
            response = response_buffer;
            fclose(fptr);
        }
    } else if (strcmp(path, "/") == 0) {
        response = reply_200;
    } else {
        response = reply_404;
    }

    printf("The String path %s\n", path);
    printf("Response to server <\n%s\n>\n", response);
    send(id, response, strlen(response), 0);

    close(id);
    return NULL;
}
