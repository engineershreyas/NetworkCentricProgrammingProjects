#ifndef _SERV_H
#define _SERV_H

#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <limits.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>



typedef struct header {

  short unsigned opcode;
  char* data;

} header_packet;

typedef struct data_packet {

  uint16_t opcode;
  uint16_t block_num;
  char data[511];

} data_packet;

typedef struct ack_packet {

  uint16_t opcode;
  uint16_t block_num;

} ack_packet;

typedef struct err_packet {

  uint16_t opcode;
  uint16_t error_code;
  char* err_msg;

} err_packet;

#endif
