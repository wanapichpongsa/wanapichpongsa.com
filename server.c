#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

char *views[] = {"/"};

char *get_slug(char *buf) {
  /*
    can't init undefined because CPU doesn't clear stack memory
    because 00000000ing a stack slot would be redundant when they are mostly
    overwritten by new vals anyways.
    therefore prev vals from prev calls will be used instead.
    Safer to init as -1 (0000 0001 -> 1111 1110 + 1 = 1111 1111) || '0x' =
    base16 (i.e., 4 bits each) + 'FF' [1 bit complement] than 0 because 0 byte is
    also when stack slot is cleared (just marked differently).
    ^ Segmenting opposite sets of 5 bit pos and neg ints
    example given by Intro to Computer Systems:
    - compiler flags as signed, MSB = 1 means negative.
    - 1s complement: 01111 -> 10000 = -15, 2s complement: 10000 + 1 -> 10001 =
    -15, 10000 = -16 instead
    - ^ 2s complement is 'superior' because it has no redundant -0 (zero is
    unsigned lol)
  */
  int start = -1, end = -1;
    for (int i = 0; buf[i] != '\0'; i++) {
        if (start == -1) {
            if (buf[i] == ' ') {
                start = i + 1;
            }
        } else {
            if (buf[i] == ' ') {
                end = i;
                break;
            }
        }
    }
    if (start == -1 || end == -1 || end <= start) {
        return NULL;
    }

    size_t len = (size_t)(end - start);
    char *slug = malloc(len + 1);
    if (!slug) return NULL;
    // buf (first byte of buf in heap) + start -> right source address
    memcpy(slug, buf + start, len);
    slug[len] = '\0';
    return slug;
}

int same(const char *req_slug, const char *view_slug) {
    /* walk until NULL or mismatch */
    while (*req_slug && *view_slug && *req_slug == *view_slug) {
        ++req_slug; 
        ++view_slug; 
    }
    printf("req_slug: %c, view_slug: %c\n", *req_slug, *view_slug);
    return *req_slug == *view_slug;
}

int send_resp(int client_id, char *resp) {
    int bytes_sent = write(client_id, resp, strlen(resp));
    if (bytes_sent < 0) {
        perror("Send failed\n");
        return 1;
    }
    return bytes_sent;
}

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
        // I wonder: What would the primitive function of malloc be?
        char *buf = malloc(cap);

        ssize_t bytes_read = read(new_client_room_id, buf, cap);
        if (bytes_read < 1) {
            perror("Read failed\n");
            return 1;
        }

        buf[bytes_read] = '\0';

        char *slug = get_slug(buf);
        if (slug == NULL) {
            perror("No slug found\n");
            return 1;
        }

        int n_views = (int)(sizeof(views) / sizeof(views[0]));
        for (int i = 0; i < n_views; i++) {
            // we can't compare pointers directly 
            int is_same = same(slug, views[i]);
            if (is_same == 1) {
                char *resp = "HTTP/1.1 200 OK\r\n"
                             "Content-Length: 12\r\n"
                             "Connection: close\r\n"
                             "\r\n"
                             "Hello World!";
                send_resp(new_client_room_id, resp);
                printf("HTTP/1.1 200 OK\r\n");
            } else {
                char *resp = "HTTP/1.1 404 Not Found\r\n"
                             "Content-Length: 12\r\n"
                             "Connection: close\r\n"
                             "\r\n"
                             "Not Found!";
                send_resp(new_client_room_id, resp);
                printf("HTTP/1.1 404 Not Found\r\n");
            }
        }
        free(buf); // free after variable lifetime ends (no longer needed)

        shutdown(new_client_room_id, SHUT_WR); /* send FIN, keep receiving */
    }

    // close(kernel_issued_id); will reach when CLI stops
    return 0;
}