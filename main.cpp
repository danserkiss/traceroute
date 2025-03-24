#include "header.h"

int main(int argc, char *argv[])
{
    int j,max_hops=DEFAULT_MAX_HOPS;
    char *domain_name,*ip=NULL;
    while ((j=getopt(argc,argv,"d:i:m:h"))!=-1)
    {
        switch (j)
        {
            case 'h':
                print_help();
                exit(1);
            case '?':
                printf("unknown option\n");
                exit(1);
                break;
            case ':':
                printf("You must enter some value for option %c",optopt);
                exit(1);
                break;
            case 'm':
                max_hops = atoi(optarg);
                break;
            case 'd':
                domain_name=optarg;
                if(gethostbyname(domain_name)==NULL)
                {
                    printf("Unkown domain name!\n");
                    exit(1);
                }
                else
                {
                    ip=inet_ntoa(*(in_addr*)gethostbyname(domain_name)->h_addr);
                }
                break;
            case 'i':
                ip=optarg;
                break;

        }
    }
    
    if(ip==NULL)
    {
        printf("You should enter ip address ,using option '-i'\n");
        exit(1);
    }

    int sockfd = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);  
    if(sockfd==-1)
    {
        printf("Failed creating RAW socket. Try execute program with sudo\n");
        exit(1);
    }

    void *packet=malloc(sizeof(icmphdr));
    if (packet==NULL)
    {
        printf("Error allocating memory for packet\n");
        exit(1);
    }
    struct sockaddr_in dest;
    struct icmphdr *icmp_heder = (struct icmphdr *)packet;

    memset(&dest,0,sizeof(dest));
    dest.sin_family=AF_INET;
    dest.sin_addr.s_addr=inet_addr(ip);

    memset(packet,0,sizeof(packet));
    icmp_heder->type=ICMP_ECHO;
    icmp_heder->code=0;
    icmp_heder->un.echo.id=0;
    icmp_heder->un.echo.sequence=1;
    icmp_heder->checksum=0;
    icmp_heder->checksum = checksum(icmp_heder, sizeof(packet));
 
    socklen_t destlen=sizeof(dest); 
  
    void *buf=malloc(sizeof(struct icmphdr));
    if (buf==NULL)
    {
        printf("Error allocating memory for buffer\n");
        exit(1);
    }

    struct sockaddr_in from;
    socklen_t fromlen=sizeof(sockaddr_in);

    int ttl=1;
    hostent * temp;
    

    printf("\t Tracing route to %s \t max_hops = %d (default %d)\n",ip,max_hops,DEFAULT_MAX_HOPS);
    while(ttl<=max_hops)
    {
        struct timeval timeout;
        timeout.tv_sec=1;
        timeout.tv_usec=0;

        setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(timeout));
        setsockopt(sockfd,IPPROTO_IP,IP_TTL,&ttl,sizeof(ttl));
        
        time_t time_arr[TIMENUMS];
        for(int k=0;k<3;k++)
        {
            struct timeval start,end;
            gettimeofday(&start,NULL);
            sendto(sockfd,packet,sizeof(packet),0,(struct sockaddr *)&dest,destlen);
            int recv_res=recvfrom(sockfd,buf,sizeof(buf),0,(struct sockaddr *)&from ,&fromlen);
            if(recv_res==-1)
            {time_arr[k]=-1;
                continue;}
            gettimeofday(&end,NULL);
            time_t time1=start.tv_sec*1000+start.tv_usec/1000;
            time_t time2=end.tv_sec*1000+end.tv_usec/1000;
            time_arr[k]=time2-time1;
        }

        print_ip_domain(ttl,inet_ntoa(from.sin_addr),time_arr);
        ttl++;
        if(from.sin_addr.s_addr==dest.sin_addr.s_addr)
        {
            break;
        }
   
    }
    
    free(packet);
    free(buf);
    close(sockfd);
    return 0;
}


