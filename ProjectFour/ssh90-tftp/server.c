#include "server.h"

#define MAXLINE 1024

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
  char mesg[MAXLINE];
  uint16_t block_num = 1;
  size_t nread;
  int offset = 0;
  FILE *fp;



  data_packet *data = (data_packet*)mesg;

  for ( ; ; ) {
    uint16_t b_num;
    len = clilen;
    n = recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
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
                f++;
              }
              else{
                mode_str[m] = mesg[i];
                m++;
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

        printf("%s %s %s from %d.%d.%d.%d:%d\n",type,filename,mode_str,a,b,c,d,port);

        fp = fopen(filename,"r");

        if(fp == NULL){

          printf("File not found.\n");

          err_packet err;
          err.opcode = 5;
          err.error_code = 1;
          char zero = 0;
          char *msg = "File not found.";

          size_t s_len = strlen(msg);
          char *final = malloc(s_len + 1 + 1);
          strcpy(final,msg);
          final[s_len] = '\0';
          final[s_len + 1] = 0;
          err.err_msg = final;

          //char bytes[sizeof(err)];
          //memcpy(bytes,&err,sizeof(err));

          char bytes[20];

          bytes[0] = 0
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

          free(final);

        }
        else{

          printf("File found!\n");

          data_packet data;


          char dat[511];
          if((nread = fread(dat,1,sizeof(dat),fp)) > 0){
            strcpy(data.data,dat);
          }
          data.opcode = 3;
          data.block_num = block_num;


          char bytes[sizeof(data)];
          memcpy(bytes,&data,sizeof(data));

          int lol = sendto(sockfd,bytes,sizeof(bytes),0,pcliaddr,clilen);
          printf("bytes sent = %d\n",lol);


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

int get_opcode(char *buf,size_t buflen, uint16_t *opcode){

    if(buflen < 2) return 0xffff;

    *opcode = (buf[0] << 8) + buf[1];

    return 0;

}
