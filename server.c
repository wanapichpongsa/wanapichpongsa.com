#include <stdio.h>
#include <time.h> // /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/time.h

// arbitary listener that passively runs in the background

/*
    the req will come from another server entity (shell interacting with a
   network) to start with, it'll be easier to think of LAN?
*/

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
    /*
        like how Galileo measured the quotient of a day (hours, minutes,
       seconds) seconds) using water, how do we measure the number of iterations
       (script lifetime) in a second?

           1. You have to collect an approximate amount of your measurement from
       the start until end of a time period you have empirical markers of its
       start and finish.

           Lucky for us, we can just connect to the computer's digital clock and
       ask it to run this script for the duration of a second.

           We can use a library, but how do we do this without a library?
           1. The software location of the clock.
           2. Borrow/build a filesystem/operating system connector to the clock.
    */
    int iterations = 0;
    time_t start = time(NULL);
    while (time(NULL) == start) {
        iterations++;
    }
    printf("%lld iterations in 1 second\n", (long long)iterations);
    // 9386303 iterations in 1 second
    return 0;
}