#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/errno.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>


void catch(int status)
{
    if (status == -1)
    {
        printf("Error (%d): %s\n", errno, strerror(errno));
    }
}


void handshake()
{
    char private_name[256];
    sprintf(private_name, "%d", getpid());
    mkfifo(private_name, 0666);
    int status;
    int wkp = open("WKP", O_WRONLY); 
    status = write(wkp, private_name, strlen(private_name)); // connection request
    catch(status);
    int private_pipe = open(private_name, O_RDONLY);
    char ACK[256];
    status = read(private_pipe, ACK, 256); // recieve acknowledgement
    catch(status);
    remove(private_name);
    char CONF[] = "Acknowledgement Received\n";
    status = write(wkp, CONF, sizeof(CONF));
    catch(status);
    close(wkp);
    close(private_pipe);
}   

static void sighandler(int signo)
{
    if (signo == SIGINT)
    {
        exit(0);
    }
}

int main(){
    signal(SIGINT, sighandler);
    handshake();
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
