// FYI: h is system headers.
#include <stdio.h>
// BSD socket networking APIs:
// https://people.eecs.berkeley.edu/~culler/WEI/labs/lab5-sockets/sockets-intro.html
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
// UNIX Standard std syscalls: read, write, close, fork, exec, wait, etc.
#include <unistd.h>

// which path in the computer leads to the function that inits http servers?
int main() {
    // Think about common server URL: 127.0.0.1:8080

    // think: 2 sides of protocol: client/server, talk/listen

    /*
        create a TCP/IPv4 socket() => returns an int file-descriptor
       (kernel-issued id) of a socket type. 
       FYI:
        - headers already give you AF_INET = 2 (IPv4)
        - and SOCK_STREAM = 1 (TCP)
        - 0 tells kernel "pick the default protocol"
    */
    int kernel_issued_id = socket(AF_INET, SOCK_STREAM, 0);

    /* 
    sin_* = socket address struct attributes
    fill in the server's bind address:
        - sin_family → address family (IPv4)
        - sin_port   → port 8080, converted to network byte order
        - sin_addr   → 0.0.0.0 ("any interface") 
    
    FYI: sockaddr_in is like Typescript interface
    */
    struct sockaddr_in addr = {
                                .sin_family = AF_INET,
                                .sin_port = htons(8080),
                                .sin_addr = {.s_addr = htonl(INADDR_ANY)}
                            };

    /* 
        attach the socket to that local address/port

        SYNTAX: bind expects 2nd param of type (struct sockaddr) so we
        pass the pointer (memory address) to addr, letting compiler know it is of required type.
        We don't want to cut the bytes of addr because we may reference it multiple times.

        addr in stack, and in stack, there is no sentinel (special end-marker) e.g., \0
        that lets fn know to stop reading. therefore, we need to pass the unique size of the struct.
    */
    int syscall_return_code = bind(kernel_issued_id, (struct sockaddr *)&addr, sizeof(addr));
    // syscall: userland space -> kernel space (priviledged work: open, read, write, bind)
    if (syscall_return_code < 0) {
        return 1; // throw
    }
    printf("Server is listening on port 8080\n");

    /* mark the socket as passive, queue up to 16 pending connections */
    listen(kernel_issued_id, 16);

    /* 
        Original listening socket (on declared addr) pauses until client TCP handshake finishes.
        After, kernel via accept():
        1. Create brand new socket for client
        2. Returns new integer file descriptor
    */
    int client = accept(kernel_issued_id, NULL, NULL);
    printf("Client connected\n");

    /* 
        Fixed-sized char array for compiler to size length of string + \0 sentinel,
        and then pass the pointer to write() into array.
    */
    const char resp[] = "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n\r\nOK";
    write(client, resp, sizeof(resp) - 1); // -1 to exclude \0
    printf("HTTP/1.1 200 OK\r\n"); // HTTP header always returns with 2 byte carriage return

    /*
    Handshake sequences that can occur with this function:

        seq: 32 bit counter TCP side chooses at connection start.
        Labels first byte in every segment so other side can order bytes,
        confirm receipt.

        ack: The “acknowledgment number” field says, “I have successfully
        received every byte up to sequence X, so the next byte I expect is X + 1.”

        TCP 3-way connect (when you run curl localhost:8080)
        Client → Server (SYN) “Let’s open seq X.”
        Server → Client (SYN-ACK) “OK, seq Y; I ack X+1.”
        Client → Server (ACK) “Ack Y+1.”

        Interlude: Makes sense to have 3-way handshake because both sides know
        message is received.

        → connection is ESTABLISHED; accept() returns.

        TCP 4-way close (close(client))
        Server → Client (FIN + last data) “I’m done sending.”
        Client → Server (ACK) “Got your FIN.”
        Client → Server (FIN) “I’m done too.” (occurs after curl reads the
        reply) 
        Server → Client (ACK) “FIN received.” → both sides leave, connection
        is fully closed.
    */

    close(client); // close connection to that client
    close(kernel_issued_id); // stop listening
    return 0;
}