
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int kernel_issued_id = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {
                                .sin_family = AF_INET,
                                .sin_port = htons(8080),
                                .sin_addr = {.s_addr = htonl(INADDR_ANY)}
                            };

    int syscall_return_code = bind(kernel_issued_id, (struct sockaddr *)&addr, sizeof(addr));
    if (syscall_return_code < 0) {
        return 1;
    }
    printf("Server is listening on port 8080\n");

    listen(kernel_issued_id, 16);

    // usually init; condition; increment but empty means infinite loop (always true)
    for (;;) {
        // like customer service creates new room to serve client
        int new_client_room_id = accept(kernel_issued_id, NULL, NULL);
        printf("Client connected\n");

        /* 
            Fixed-sized char array for compiler to size length of string + \0 sentinel,
            and then pass the pointer to write() into array.
        */
        const char resp[] = "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK";
        write(new_client_room_id, resp, sizeof(resp) - 1); // -1 to exclude \0
        printf("HTTP/1.1 200 OK\r\n"); // HTTP header always returns with 2 byte carriage return
        close(new_client_room_id);
    }

    // close(kernel_issued_id); will reach when CLI stops
    return 0;
}