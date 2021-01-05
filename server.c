/*
Server Design
Handshake
A procedure to ensure that a connection has been established between 2 programs.
Both ends of the connection must verify that they can send and receive data to and from each other.
3 way handshake
Client sends a message to the server. (server knows it can receive)
    Server sends a response to the client. (client knows it can recieve and send)
    Client sends a response back to the server. (server knows it can receive and send)
    Setup
    Server creates a FIFO (Well Known Pipe) and waits for a connection.
    Client creates a “private” FIFO.
    sprintf(buffer, "%d", getpid() );
    Handshake
    Client connects to server and sends the private FIFO name. Client waits for a response from the server.
    Server receives client’s message and removes the WKP.
    Server connects to client FIFO, sending an initial acknowledgement message.
    Client receives server’s message, removes its private FIFO.
    Client sends response to server.
    Operation
    Server and client send information back and forth.
    Reset
    Client exits, server closes any connections to the client.
    Server recreates the WKP waits for another client.
*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>

static void sighandler(int signo)
{
    if (signo == SIGPIPE)
    {
        printf("Client disconnected. Setting up new handshake for next client...\n");
        setup_new_handshake();
    }

    if (signo == SIGINT)
    {
        remove("mario");
        remove("luigi");
        remove("wkp");
        exit(0);
    }
}

void handshake(){
    printf("Waiting for a Client Connection...\n");
    mkfifo("WKP", 0666); // make Well Known Pipe
    int fd = open("WKP", O_RDONLY); 
    char private_name[256];
    int status;
    status = read(fd, private_name, 256); // read in the name of the FIFO that the Client made
    check_error(status);
    int private_pipe = open(private_name, O_WRONLY);
    remove("WKP");
    
    printf("Received Connection Request\n\n");

    printf("Starting Handshake with Client (Pipe=%s)\n\n", private_name);
    
    char ACK[] = "Acknowledgement of Private Pipe being received";
    status = write(private_pipe, ACK, sizeof(ACK));
    check_error(status);

    char CONF[256];

    status = read(fd, CONF, 256); // confirmation of acknowledgement reception
    check_error(status);
    printf("Handshake established! Confirmation message: %s\n\n", CONF);
    close(fd);
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
    signal(SIGINT, sighandler);
    handshake();
    mkfifo("mario", 0666);
    mkfifo("luigi", 0666);
    int inpipe = open("mario", O_RDONLY);
    if (inpipe == -1) {
        printf("errno: %d\terror: %s\n", errno, strerror(errno));
        return -1;
    }

    int outpipe = open("luigi", O_WRONLY);
    if (outpipe == -1) {
        printf("errno: %d\terror: %s\n", errno, strerror(errno));
        return -1;
    } 
    char input [256] = "";
    char output [256] = "";
    while(1){
        // read input from mario
        int r = read(inpipe, &input, sizeof(input));
        if (r == -1) {
            printf("errno: %d\terror: %s\n", errno, strerror(errno));
            break;
        }
        int ans = phi(atoi(input));
        sprintf(output, "%d", ans);

        // write processed input to luigi
        int w = write(outpipe, &output, sizeof(output));
        if (w == -1) {
            printf("errno: %d\terror: %s\n", errno, strerror(errno));
            break;
        }
    }

    close(inpipe);
    close(outpipe);
    return 0;
}
