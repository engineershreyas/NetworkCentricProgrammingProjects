/*
 * proxy.c - A Simple Sequential Web proxy
 *
 * Course Name: 14:332:456-Network Centric Programming
 * Assignment 2
 * Student Name:______________________
 *
 * IMPORTANT: Give a high level description of your code here. You
 * must also provide a header comment at the beginning of each
 * function that describes what that function does.
 */

#include "csapp.h"
#include <strings.h>

/*
 * Function prototypes
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, int size);

int hostname_to_ip(char *  hostname, char* ip);
/*
 * main - Main routine for the proxy program
 */
int main(int argc, char **argv)
{

    /* Check arguments */
    if (argc != 2) {
	     fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
	      exit(0);
    }

    //get the port number
    char* portString = argv[1];
    int port = atoi(portString);

    //open socket on that port
    int sockfd = Open_listenfd(port);
    if(sockfd < 0) exit(1);

    //infinite loop
    while(1){

      struct sockaddr_in clientaddr;

      //accept requests
      int clilen = sizeof(clientaddr);
      int clientfd = Accept(sockfd, (SA *)&clientaddr, &clilen);
      if(clientfd < 0) exit(1);

      char buf[MAXLINE];

      recv(clientfd, buf,MAXBUF, 0);

      char file[5000];
      char method[100];
      char rest[1000];

      sscanf(buf, "%s %s %s",method,file,rest);

      char *host = strstr(file,"www");

      char *host_p1 = malloc(strlen(host) + 1);


      strcpy(host_p1,host);

      char* f = index(host,':');

      if(f == NULL) {
        host = (char *)strtok(host, "/");
      }
      else{
        host = (char *)strtok(host, ":");
      }

      char *temp = NULL;
      temp = (char *)strtok(host_p1,"/");
      temp =  (char *)strstr(host_p1, ":");


      int serverfd;
      if(temp == NULL){
        serverfd = open_clientfd(host,80);
        if(serverfd < 0) exit(1);
      }else{
        int token;
        ++temp;
        token = atoi(temp);
        serverfd = open_clientfd(host,token);
        if(serverfd < 0) exit(1);
      }


      //GET request to server
      Rio_writen(serverfd,"GET ", strlen("GET "));
      Rio_writen(serverfd,file,strlen(file));
      char* httpString = " HTTP/1.0\r\n\r\n";
      Rio_writen(serverfd,httpString,strlen(httpString));

      int r_len = 0;
      int n;

      //get result
      while((n = rio_readn(serverfd, buf, MAXLINE)) > 0 ){
        r_len += n;
        Rio_writen(clientfd,buf,n);
        bzero(buf,MAXLINE);
      }


      char logstring[10000];
      format_log_entry(logstring,&clientaddr,host,r_len);

      //log output
      FILE *fp = fopen("proxy.log", "a");
      if(fp == NULL){
        printf("Error: File or Directory not found\n");
      }
      else{
        fprintf(fp,"%s\n",logstring);
        fclose(fp);
        printf("%s\n",logstring);
      }





      close(clientfd);
      close(serverfd);

    }

    exit(0);
}




/*
 * format_log_entry - Create a formatted log entry in logstring.
 *
 * The inputs are the socket address of the requesting client
 * (sockaddr), the URI from the request (uri), and the size in bytes
 * of the response from the server (size).
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr,
		      char *uri, int size)
{
    time_t now;
    char time_str[MAXLINE];
    unsigned long host;
    unsigned char a, b, c, d;

    /* Get a formatted time string */
    now = time(NULL);
    strftime(time_str, MAXLINE, "%a %d %b %Y %H:%M:%S %Z", localtime(&now));

    /*
     * Convert the IP address in network byte order to dotted decimal
     * form. Note that we could have used inet_ntoa, but chose not to
     * because inet_ntoa is a Class 3 thread unsafe function that
     * returns a pointer to a static variable (Ch 13, CS:APP).
     */
    host = ntohl(sockaddr->sin_addr.s_addr);
    a = host >> 24;
    b = (host >> 16) & 0xff;
    c = (host >> 8) & 0xff;
    d = host & 0xff;


    /* Return the formatted log entry string */
    sprintf(logstring, "%s: %d.%d.%d.%d %s %d", time_str, a, b, c, d, uri, size);
}
