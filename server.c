
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main() {
    int kernel_issued_id = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {
                                .sin_family = AF_INET,
                                .sin_port = htons(8080),
                                .sin_addr = {.s_addr = htonl(INADDR_ANY)}
                            };

    int syscall_return_code = bind(kernel_issued_id, (struct sockaddr *)&addr, sizeof(addr));
    if (syscall_return_code < 0) {
        perror("Bind failed\n");
        return 1;
    }
    printf("Server is listening on port 8080\n");

    listen(kernel_issued_id, 16);

    // usually init; condition; increment but empty means infinite loop (always true)
    for (;;) {
        /* 
            like customer service creates new room to serve client
            Client will thus think there was a 'recv' failure.
        */
        int new_client_room_id = accept(kernel_issued_id, NULL, NULL);
        printf("Client connected\n");

        // Read data otherwise TCP stack aborts connection and sends ReSet packet.
        // dynamic allocation: Instead of reserving fixed stack array, you request exact size from heap.
        // So instead, the stack has a pointer (fixed 8 bytes on 64-bit macOS) to the heap.
        size_t cap = 1024;
        char *buf = malloc(cap); // pointer to heap with byte amount TBD

        ssize_t bytes_read = read(new_client_room_id, buf, cap);
        if (bytes_read < 1) {
            perror("Read failed\n");
            return 1;
        }

        buf[bytes_read] = '\0';
        printf("bytes_read: %zu\n", bytes_read);
        free(buf); // free after variable lifetime ends (no longer needed)

        char *resp = "HTTP/1.1 200 OK\r\n"
                     "Content-Length: 12\r\n"
                     "Connection: close\r\n"
                     "\r\n"
                     "Hello World!";
        int bytes_sent = write(new_client_room_id, resp, strlen(resp));
        if (bytes_sent < 0) {
            perror("Send failed\n");
            return 1;
        }

        printf("HTTP/1.1 200 OK\r\n"); // HTTP header always returns with 2 byte carriage return
        shutdown(new_client_room_id, SHUT_WR); /* send FIN, keep receiving */
    }

    // close(kernel_issued_id); will reach when CLI stops
    return 0;
}