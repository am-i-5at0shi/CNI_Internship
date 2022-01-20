#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include<unistd.h>
#include<time.h>
#include <sys/socket.h>
#include <sys/time.h>
#include<sys/types.h>
#include <string.h>
#include <string>
#include <chrono>
#include <iostream>
#define WINDOW 50000    // In micro-seconds
#define EARLY_WAKE 1000 
#define GUARD_VAL 600   
#define EARLY_RELEASE 120   // Time reserved for sendto() function in micro-seconds
#define MAX 80
#define MARGINE 50000       // For sync of sender_side and receiver_side 
#define PORT 6400
#define SA struct sockaddr

typedef struct{
    u_int64_t stamp;
    double throughput;
}data_block;

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;
    /////////////////////////////Throughput part////////////////
    FILE * fp;
	u_int64_t *rx_bytes_before = (u_int64_t *)malloc(sizeof(u_int64_t));
	u_int64_t *rx_bytes_after = (u_int64_t *)malloc(sizeof(u_int64_t));
	u_int64_t sleep_time, wake_time, release_time;// sleep time in microseconds 
    data_block send_it;
	struct timeval stop, start, ref_time;
    u_int64_t counter = 0, diff, i = 0;
////////////////////////////////////////////////////////////////////////

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
     // assign IP, PORT
    char s_ip[MAX];
    printf("GIVE I.P OF SERVER (FORMAT: x.y.z.a): ");
    scanf("%s",s_ip);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(s_ip);  //CHANGE IT TO SERVER'S IP ADDRESS
    servaddr.sin_port = htons(PORT);
    gettimeofday(&ref_time,NULL);
    ref_time.tv_usec = (ref_time.tv_usec + 50000);
    if(ref_time.tv_usec >= 1000000 ){
        ref_time.tv_sec += 1;
        ref_time.tv_usec = ref_time.tv_usec % 1000000;
    }
    sendto(sockfd, (void *)&ref_time,sizeof(ref_time),0,(sockaddr *)&servaddr,sizeof(servaddr));
    FILE * file = fopen("output_receiver.txt","w");
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
        
        fp = fopen("/sys/class/net/wlo1/statistics/rx_bytes","r");  // CHANGE PATH AS REQUIRED.
        fscanf(fp,"%lu",rx_bytes_after);
		fclose(fp);
        
       
        //printf("\t\trx before %ld\trx after: %ld\n",*rx_bytes_before,*rx_bytes_after);
		send_it.throughput = ((*rx_bytes_after - *rx_bytes_before) * 8.0)/(diff*1.0);   // in Mbps
        *rx_bytes_before = *rx_bytes_after;

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
