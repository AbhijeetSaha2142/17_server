#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <signal.h>

static void sighandler(int signo)
{
    if (signo == SIGINT)
    {
        remove("mario");
        remove("luigi");
        printf("\n");
        exit(0);   
    }
}

int main(){
    signal(SIGINT, sighandler);
    mkfifo("mario", 0666);
    mkfifo("luigi", 0666);
    int inpipe = open("mario", O_WRONLY);
    if (inpipe == -1) {
        printf("errno: %d\terror: %s\n", errno, strerror(errno));
        return -1;
    }

    int outpipe = open("luigi", O_RDONLY);
    if (outpipe == -1) {
        printf("errno: %d\terror: %s\n", errno, strerror(errno));
        return -1;
    } 
    char input [256] = "";
    char output [256] = "";

    while(1){
        // recieve input
        printf("Please enter a positive integer: ");
        fgets(input, 256, stdin);
        *strchr(input, '\n') = 0;
        // write input to mario to be read by process.c
        
        int w = write(inpipe, &input, strlen(input) + 1);
        if (w == -1) {
            printf("errno: %d\terror: %s\n", errno, strerror(errno));
            break;
        }
        // recieve output from process.c that was written to luigi and print it
        int r = read(outpipe, &output, sizeof(output));
        if (r == -1) {
            printf("errno: %d\terror: %s\n", errno, strerror(errno));
            break;
        } 
        printf("phi(n): %s\n", output);
    }
    
    close(inpipe);
    close(outpipe);
    return 0;
}
