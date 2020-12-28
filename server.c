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
    if (signo == SIGINT)
    {
        remove("mario");
        remove("luigi");
        printf("\n");
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
    signal(SIGINT, sighandler);
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