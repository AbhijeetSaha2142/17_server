#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

void handshake()
{
    char private_name[256];
    sprintf(private_name, "%d", getpid());
    mkfifo(private_name, 0666);
    int status;
    int wkp = open("WKP", O_WRONLY); 
    status = write(wkp, private_name, 4*strlen(private_name)); // connection request
    int private_pipe = open(private_name, O_RDONLY);
    char ACK[256];
    status = read(private_pipe, ACK, 256); // recieve acknowledgement
    remove(private_name);
    char CONF[] = "Acknowledgement Received.";
    status = write(wkp, CONF, sizeof(CONF));
    close(wkp);
    close(private_pipe);
}   

static void sighandler(int signo)
{
    if (signo == SIGINT)
    {
        printf("\n");
        exit(0);
    }
}

int main(){
    signal(SIGINT, sighandler);
    handshake();
    int inpipe = open("mario", O_WRONLY);
    int outpipe = open("luigi", O_RDONLY);
    char input [256] = "";
    char output [256] = "";

    while(1){
        // recieve input
        printf("Please enter a positive integer: ");
        fgets(input, 256, stdin);
        *strchr(input, '\n') = 0;
        // write input to mario to be read by server.c
        int n = atoi(input);
        int w = write(inpipe, &input, strlen(input) + 1);
        // recieve output from server.c that was written to luigi and print it
        int r = read(outpipe, &output, sizeof(output));
        printf("phi(%d): %s\n\n", n, output);
    }
    
    close(inpipe);
    close(outpipe);
    return 0;
}
