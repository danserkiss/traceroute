#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>

unsigned short checksum(void *, int);

int main(int argc, char *argv[])
{
    int sockfd = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);  

    char packet[1024];
    struct sockaddr_in dest;
    struct icmphdr *icmp_heder = (struct icmphdr *)packet;

    memset(&dest,0,sizeof(dest));
    dest.sin_family=AF_INET;
    dest.sin_addr.s_addr=inet_addr(argv[1]);

    memset(packet,0,sizeof(packet));
    icmp_heder->type=ICMP_ECHO;
    icmp_heder->code=0;
    icmp_heder->un.echo.id=0;
    icmp_heder->un.echo.sequence=1;
    icmp_heder->checksum=0;
    icmp_heder->checksum = checksum(icmp_heder, sizeof(packet));
 
    socklen_t destlen=sizeof(dest); 
  
    char buf[1024];
    struct sockaddr_in from;
    socklen_t fromlen=sizeof(sockaddr_in);

    int max_hops=64;
    int ttl=1;
    hostent * temp;
    

    printf("icmp echo sent to %s\n",argv[1]);
    while(ttl<=max_hops)
    {
        setsockopt(sockfd,IPPROTO_IP,IP_TTL,&ttl,sizeof(ttl));
        sendto(sockfd,packet,sizeof(packet),0,(struct sockaddr *)&dest,destlen);
        
        if (recvfrom(sockfd,buf,sizeof(buf),0,(struct sockaddr *)&from ,&fromlen)!=-1)
        {
            char * temp_ip=inet_ntoa(from.sin_addr);
            char* domain=(char*)malloc(30*sizeof(char));
            temp=gethostbyaddr(&from.sin_addr,sizeof(in_addr),AF_INET);
            if(temp!=NULL)
            {
                domain=temp->h_name;
            }
            else
            {
                domain="unknown";
            }
            printf("recived reply from : %-16s domain : %-30s\n",temp_ip,domain);
        }
        else
        {
            printf("Error reciving ICMP reply\n");
        }
        ttl++;
        if(from.sin_addr.s_addr==dest.sin_addr.s_addr)
        {
            break;
        }
   
    }
    close(sockfd);
    return 0;
}


unsigned short checksum(void *packet_header, int packet_len) {
    unsigned short *buf = (unsigned short*)packet_header;
    unsigned int sum = 0;

    for (int i=0; i < packet_len/2; i++) 
    {
        sum += buf[i];
    }
 
    if (packet_len % 2 == 1) 
    {
         sum += ((unsigned char*)buf)[packet_len-1];
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
 
    return (unsigned short)~sum;
}
