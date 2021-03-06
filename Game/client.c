#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int sockfd; 
    int n, len; 
    char sendline[5], recvline[180], turn[] = "Your turn!\n", loose[] = "You loose!\n", win[] = "You win!\n", stalemate[] = "Stalemate!\n", wait[] = "Waiting for player\n"; 
    struct sockaddr_in servaddr, cliaddr; 
    if(argc != 2){
        printf("Usage: a.out <IP address>\n");
        exit(1);
    }
    if((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
        perror(NULL); 
        exit(1);
    }
    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(0);
    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sockfd, (struct sockaddr *) &cliaddr,
    sizeof(cliaddr)) < 0){
        perror(NULL);
        close(sockfd); 
        exit(1);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(51000);
    if(inet_aton(argv[1], &servaddr.sin_addr) == 0){
        printf("Invalid IP address\n");
        close(sockfd); 
        exit(1);
    }
    if(sendto(sockfd, sendline, strlen(sendline)+1,0, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
        perror(NULL);
        close(sockfd);
        exit(1);
    }
    bzero(recvline, 180);
    recvfrom(sockfd, recvline, 30, 0, (struct sockaddr *) NULL, NULL);
    printf("%s\n", recvline);
    while(1) {
        bzero(recvline, 180);
        recvfrom(sockfd, recvline, 180, 0, (struct sockaddr *) NULL, NULL);
        printf("%s", recvline);
        bzero(recvline, 180);
        recvfrom(sockfd, recvline, 180, 0, (struct sockaddr *) NULL, NULL);
        if ((strcmp(recvline, win)&&strcmp(recvline, loose)&&strcmp(recvline, stalemate)) == 0) {
            printf("%s\n", recvline);
            return(0);
        }
        bzero(recvline, 180);
        recvfrom(sockfd, recvline, 180, 0, (struct sockaddr *) NULL, NULL);
        printf("%s", recvline);
        if (strcmp(turn, recvline) == 0) {
            while(strcmp(wait, recvline) != 0) {
                gets(sendline);
                sendto(sockfd, sendline, strlen(sendline) + 1, 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
                bzero(recvline, 180);
                recvfrom(sockfd, recvline, 180, 0, (struct sockaddr *) NULL, NULL);
                printf("%s\n", recvline);
            }
        }
    }
    close(sockfd);
    return 0;
}