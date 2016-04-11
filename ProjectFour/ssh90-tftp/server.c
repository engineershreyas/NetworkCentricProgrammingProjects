#include "server.h"

#define MAXLINE 516

void do_stuff(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen, int port);
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

  do_stuff(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr),port);
}

void do_stuff(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen, int port){
  int n;
  socklen_t len;
  uint16_t opcode;

  uint16_t block_num = 1;
  size_t nread;
  int offset = 0;
  FILE *fp;


  char mesg[MAXLINE];



  for ( ; ; ) {
    uint16_t b_num;
    len = clilen;
    if(sockfd != 0) printf("sockfd = %d",sockfd);
    n = recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
    if(n != -1){
    if(get_opcode(mesg,n,&opcode) == 0){
      char filename[MAXLINE];
      int f = 0;
      char mode_str[100];
      int m = 0;

      if(opcode == 1 || opcode == 2){
        //rrq or wrq
        int end = 0;
        int mode = 0;

        for(int i = 2; i < n; i++){

            if(mesg[i] != 0){
              if(!mode){
                filename[f] = mesg[i];
                printf("c = %c\n",filename[f]);
                f++;
              }
              else{
                mode_str[m] = mesg[i];
                printf("c = %c\n",mode_str[m]);
                m++;
              }
            }
            else{
              if(!mode){
                filename[f] = '\0';
                printf("mode_switch\n");
                mode = 1;
              }
              else{
                mode_str[m] = '\0';
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

        printf("%s %s %s from %d.%d.%d.%d:%d\n",type,filename,mode_str,a,b,c,d,port);
        printf("filename = %s, n = %d\n",filename,n);

        fp = fopen(filename,"r");


        if(fp == NULL){

          printf("File not found.\n");


          char bytes[20];

          bytes[0] = 0;
          bytes[1] = 5;
          bytes[2] = 0;
          bytes[3] = 1;
          bytes[4] = 'F';
          bytes[5] = 'i';
          bytes[6] = 'l';
          bytes[7] = 'e';
          bytes[8] = ' ';
          bytes[9] = 'N';
          bytes[10] = 'o';
          bytes[11] = 't';
          bytes[12] = ' ';
          bytes[13] = 'F';
          bytes[14] = 'o';
          bytes[15] = 'u';
          bytes[16] = 'n';
          bytes[17] = 'd';
          bytes[18] = '.';
          bytes[19] = '\0';

          int lol = sendto(sockfd,bytes,sizeof(bytes),0,pcliaddr,clilen);
          printf("bytes sent = %d\n",lol);


        }
        else{

          printf("File found!\n");


          /*
          char bytes[516];
          bytes[0] = 0;
          bytes[1] = 3;
          char lo = block_num & 0xFF;
          char hi = block_num >> 8;
          bytes[2] = hi;
          bytes[3] = lo;
          char dat[512];
          char dest[512];
          char final[516];
          if((nread = fread(dat,1,sizeof(dat),fp)) > 0){
            strcpy(dest,dat);
          }

          memcpy(final,bytes,4);
          memcpy(final + 4,dest,512);
          */

          char bytes[516];

          bytes[0] = 0;
          bytes[1] = 3;
          bytes[2] = 0;
          bytes[3] = 1;
          bytes[4] = 'H';
          bytes[5] = 'e';
          bytes[6] = 'y';
          bytes[7] = 'y';
          bytes[8] = ' ';
          bytes[9] = 'n';
          bytes[10] = 'c';
          bytes[11] = 'p';
          bytes[12] = ' ';
          bytes[13] = 'S';
          bytes[14] = 'U';
          bytes[15] = 'C';
          bytes[16] = 'K';
          bytes[17] = 'S';
          bytes[18] = '.';
          for(int i = 19; i < 515; i++){
            bytes[i] = 's';
          }
          bytes[515] = '\0';


          int lol = sendto(sockfd,bytes,sizeof(bytes),0,pcliaddr,clilen);
          printf("bytes sent = %d\n",lol);
          block_num++;


        }


          memset(filename,0,sizeof(filename));
          memset(mode_str,0,sizeof(filename));

          f = 0;
          m = 0;



      }
      else if(opcode == 3){
        //data



      }
      else if(opcode == 4){
        //ack
        printf("ACK\n");
      }
      else if(opcode == 5){
        //error
      }

    }
    else{
      printf("Error decoding opcode\n");
    }
  }


  }
}

int get_opcode(char *buf,size_t buflen, uint16_t *opcode){

    if(buflen < 2) return 0xffff;

    *opcode = (buf[0] << 8) + buf[1];

    return 0;

}
