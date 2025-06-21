#include <stdio.h>
/*
    Building a HTTP server closest to binary:

   Linux uses BSD (Berkeley Software Distribution) socket
    whereas Mac's Darwin has a BSD inspired userland (everything outside of kernel).

    Kernel is the OS core managing hardware.

    The big question: How are transistor binary states (physical location in chip)
    referenced to photon rendered 1s and 0s?

    How: Transistor switch (on/off) -> 5V (1) 0V (0) to part called 'control'
    Control: gate (control terminal), source (input terminal), drain (output terminal).
    
    Ah, so terminal GUI is a digital alias for the transistor's terminal function.

    -> Logic Gates (AND, OR, NOT) <- Combinations that allow binary states to scale in aggregate (but I don't know how they work). 
    -> Circuits -> Registers -> CPU

    CPU:
        - ALU: Arithmetic Logic Unit
        - Control Unit: Decode instructions and control the flow of data
        - Registers: Temporary storage for data
        - Cache: Temporary storage for frequently used data

    This includes shell (command interpreter),
    compilers (AST -> IR (Intermediate Representation) -> Assembly -> Machine Code)
    and system calls.

*/


// arbitary listener that passively runs in the background
void listener(int req_header) {
    if (req_header == 0) {
        printf("GET\n");
    }
    else if (req_header == 1) {
        printf("POST\n");
    }
    else if (req_header == 2) {
        printf("PUT\n");
    }
    else if (req_header == 3) {
        printf("DELETE\n");
    }
    else {
        printf("Invalid request header\n");
    }
}

int main() {
    int seconds = 60;
    while (seconds > 0) {
        // the req will come from another server entity (shell interacting with a network)
        // to start with, it'll be easier to think of LAN?
        if (seconds % 10 == 0) {
            listener(0);
        }
        seconds--;
    }
    return 0;
}