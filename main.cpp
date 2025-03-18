#include <stdio.h>
#include <sys/socket.h>

int main(int argc, char *argv[])
{
    int sockfd = socket(AF_INET,SOCK_RAW,0);  
    return 0;
}