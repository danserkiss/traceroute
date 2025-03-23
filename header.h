#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/time.h>


#define DEFAULT_MAX_HOPS 30
#define ICMP_SENT 0
#define ICMP_RECEIVED 1
#define TIMENUMS 3


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

void print_time_delay(time_t *time_arr) 
{
    for (int i=0;i<TIMENUMS;i++)
    {
        printf("time %d : %-4ld ms\t", i + 1,time_arr[i]);
    }
    printf("\n");
} 


void print_ip_domain(int num,char *ip,int var,time_t *time_arr) {
    in_addr_t ip_addr = inet_addr(ip);
    hostent *domain_by_ip = gethostbyaddr((const void*)&ip_addr,sizeof(ip_addr),AF_INET);
    
    if (var==ICMP_RECEIVED)
    {
        if(domain_by_ip==NULL){
            printf("%-3d %-16s domain : %-40s \t",num,ip,"unknown");
            print_time_delay(time_arr);
        }
        else{
            printf("%-3d %-16s domain : %-40s \t",num,ip,domain_by_ip->h_name);
            print_time_delay(time_arr);
        }
    }
    else
    {    
        if(domain_by_ip==NULL){
        printf("%-3d %-16s domain : unknown\n",num,ip);
        }
        else{
        printf("%-3d : %-16s domain : %-40s \n",num,ip,domain_by_ip->h_name);
        }
    }
}

void print_help()
{
    printf("Usage:\n my_traceroute [-m max_hops] [-i IP (Use IPv4)] [-d domain]\n");
    printf("%-10s Show this help message\n","-h");
    printf("%-10s Traceroute by IPv4 address\n","-i");
    printf("%-10s Traceroute by domain\n","-d");
    printf("%-10s Set MAX_HOPS\n","-m");

}
