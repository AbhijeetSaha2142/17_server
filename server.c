#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>

void handshake(){
    printf("Waiting for a Client Connection...\n\n");
    mkfifo("WKP", 0666); // make Well Known Pipe
    int fd = open("WKP", O_RDONLY); 
    char private_name[258];
    int status;
    status = read(fd, private_name, 258); // read in the name of the FIFO that the Client made
    int private_pipe = open(private_name, O_WRONLY);

    remove("WKP");
    
    printf("Received Connection Request\n\n");

    printf("Starting Handshake with Client (Pipe=%s)\n\n", private_name);
    
    char ACK[] = "Acknowledgement of Private Pipe being received";
    status = write(private_pipe, ACK, sizeof(ACK));

    char CONF[256];

    status = read(fd, CONF, 256); // confirmation of acknowledgement reception

    printf("Handshake completed. Confirmation message: %s\n\n", CONF);
    printf("Ready to process inputs:\n\n");
    close(fd);
}

static void sighandler(int signo)
{
    if (signo == SIGPIPE)
    {
        printf("------------------------------------------------------------\n");
        printf("Client disconnected. Restarting Handshake Protocol...\n\n");
        handshake();
    }

    if (signo == SIGINT)
    {
        remove("mario");
        remove("luigi");
        remove("WKP");
        exit(0);
    }
}

int phi(int n){ 
    int ans = n; 
    for (int p = 2; p * p <= n; ++p) { 
        if (n % p == 0) {
            while (n % p == 0) 
                n /= p; 
            ans -= ans / p; 
        } 
    } 
    if (n > 1) 
        ans -= ans / n; 
    return ans; 
} 

int main(){
    mkfifo("mario", 0666);
    mkfifo("luigi", 0666);
    signal(SIGINT, sighandler);
    signal(SIGPIPE, sighandler);
    handshake();
    int inpipe = open("mario", O_RDONLY);

    int outpipe = open("luigi", O_WRONLY);
    char input [256] = "";
    char output [256] = "";
    while(1){
        // read input from mario
        int r = read(inpipe, &input, sizeof(input));
        printf("Computing phi(%d)...\n\n", atoi(input));
        int ans = phi(atoi(input));
        sprintf(output, "%d", ans);

        // write processed input to luigi
        int w = write(outpipe, &output, sizeof(output));
    }

    close(inpipe);
    close(outpipe);
    remove("mario");
    remove("luigi");
    return 0;
}
