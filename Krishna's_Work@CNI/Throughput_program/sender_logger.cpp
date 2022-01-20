#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include<unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <string>
#include <chrono>
#include <iostream>
#define WINDOW 50000    // In micro-seconds
#define EARLY_WAKE 1000 
#define GUARD_VAL 600   
#define EARLY_RELEASE 120   // Time reserved for sendto() function in micro-seconds
#define MAX 80 
#define PORT 6400
#define SA struct sockaddr

typedef struct{
    u_int64_t stamp;
    double throughput;
}data_block;

int main()
{
	int sockfd, connfd, len;
    struct sockaddr_in servaddr, client_addr;
    int cli_adr_len = sizeof(client_addr);
	FILE * fp;
	u_int64_t *tx_bytes_before = (u_int64_t *)malloc(sizeof(u_int64_t));
	u_int64_t *tx_bytes_after = (u_int64_t *)malloc(sizeof(u_int64_t));
	u_int64_t sleep_time, wake_time, release_time;// sleep time in microseconds 
    data_block send_it;
	struct timeval stop, start, ref_time;
    u_int64_t counter = 0, diff, i = 0;
  
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
  
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
  
    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    recvfrom(sockfd, (void *)&ref_time, sizeof(ref_time), MSG_WAITALL, (struct sockaddr *)&client_addr, (socklen_t *)&cli_adr_len);
        
    FILE * file = fopen("output_sender.txt","w");
    do{gettimeofday(&start,NULL);}while((ref_time.tv_sec - start.tv_sec) > 0 || (ref_time.tv_usec - start.tv_usec) > 0);
    while(i<100){
        i++;      // for teminating loop. 
        counter++; //sending throughput message to other-side
        gettimeofday(&start, NULL);    
        sleep_time = (ref_time.tv_sec - start.tv_sec)*1000000 + (ref_time.tv_usec - start.tv_usec) + (counter*WINDOW) - EARLY_WAKE;
		bool done = false;
		while (1) {
            
			if (!done) {
				struct timespec ts;
				ts.tv_sec = 0;
				ts.tv_nsec = sleep_time * 1000;
				nanosleep(&ts, &ts);
				done = true;
			}
			gettimeofday(&stop, NULL);
			diff = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
            wake_time = (ref_time.tv_sec*1000000 + ref_time.tv_usec + (counter * WINDOW) - GUARD_VAL);
			if ((stop.tv_usec + stop.tv_sec*1000000) > wake_time) {
				break;
			}
		}
        
        fp = fopen("/sys/class/net/wlo1/statistics/tx_bytes","r");  // CHANGE PATH AS REQUIRED.
        fscanf(fp,"%lu",tx_bytes_after);
		fclose(fp);
        
       
        //printf("\t\ttx before %ld\ttx after: %ld\n",*tx_bytes_before,*tx_bytes_after);
		send_it.throughput = ((*tx_bytes_after - *tx_bytes_before) * 8.0)/(diff*1.0);   // in Mbps
        *tx_bytes_before = *tx_bytes_after;

        while(1){
            gettimeofday(&stop, NULL);
            release_time = (ref_time.tv_sec*1000000 + ref_time.tv_usec + counter*WINDOW) - EARLY_RELEASE;
			if ((stop.tv_usec + stop.tv_sec*1000000) > release_time) {
				break;
			}
       
        }
        gettimeofday(&stop,NULL);
        send_it.stamp = 1000000 * stop.tv_sec + stop.tv_usec;
        //sendto(sockfd, (void *)(& send_it), sizeof(data_block), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

        fprintf(file, "throughput:\t\t%d\tMbps.\ntime:\t\t%lu\tmicro-seconds.\n\n", (int)(send_it.throughput), send_it.stamp);    // Only for debugging...

	

    }
    // close the socket
    fclose(file);
    close(sockfd);  
}