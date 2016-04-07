#include "server.h"

#define MAXLINE 1024

void do_stuff(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen);
int get_opcode(char *buf,size_t buflen, uint16_t *opcode);

int main(int argc, char **argv){

  if(argc < 2){
    printf("usage: ./server [port]\n");
    return 0;
  }

  int sockfd;
  struct sockaddr_in servaddr, cliaddr;


  int port = atoi(argv[1]);




  sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  bzero(&servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;

  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  servaddr.sin_port = htons(port);

  if(bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1){
      printf("bind failed\n");
  }
  else{
    printf("bind succeeded\n");
  }

  do_stuff(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
}

void do_stuff(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen){
  int n;
  socklen_t len;
  uint16_t opcode;
  char mesg[MAXLINE];




  data_packet *data = (data_packet*)mesg;

  for ( ; ; ) {
    uint16_t b_num;
    len = clilen;
    n = recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
    if(get_opcode(mesg,n,&opcode) == 0){
      char filename[MAXLINE];
      char mode_str[100];

      if(opcode == 1 || opcode == 2){
        //rrq or wrq
        int end = 0;
        int mode = 0;

        for(int i = 2; i < n; i++){

            if(mesg[i] != 0){
              if(!mode){
                printf("c = %c\n",mesg[i]);
                filename[i] = mesg[i];
              }
              else{
                printf("c2 = %c\n");
                mode_str[i] = mesg[i];
              }
            }
            else{
              if(!mode){
                mode = 1;
              }
              else{
                end = 1;
                break;
              }
            }


        }

        unsigned long host = ntohl(((struct sockaddr_in *)pcliaddr)->sin_addr.s_addr);
        unsigned char a = host >> 24;
        unsigned char b = (host >> 16) & 0xff;
        unsigned char c = (host >> 8) & 0xff;
        unsigned d = host & 0xff;

        char* type = opcode == 1 ? "RRQ" : "WRQ";

        printf("%s %s %s from %d.%d.%d.%d\n",type,filename,mode_str,a,b,c,d);

        err_packet err;
        err.opcode = 5;
        err.error_code = 1;
        char zero = 0;
        char *msg = "File not found.";

        size_t s_len = strlen(msg);
        char *final = malloc(s_len + 1);
        strcpy(final,msg);
        final[s_len] = zero;
        err.err_msg = final;

        char bytes[sizeof(err_packet)];
        memcpy(bytes,&err,sizeof(err_packet));

        sendto(sockfd,bytes,sizeof(b),0,pcliaddr,clilen);

        free(final);



      }
      else if(opcode == 3){
        //data



      }
      else if(opcode == 4){
        //ack
      }
      else if(opcode == 5){
        //error
      }

    }
    else{
      printf("Error decoding opcode");
    }

  }
}

int get_opcode(char *buf,size_t buflen, uint16_t *opcode){

    if(buflen < 2) return 0xffff;

    *opcode = (buf[0] << 8) + buf[1];

    return 0;

}
