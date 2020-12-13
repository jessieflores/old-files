/*********
ICS 451 Spring 2020
lab 3
Jessie Flores
*********/

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define LENGTH 20
#define PORT 12345

void print_header(unsigned char hdr[LENGTH]);

int main(int argc, char *argv[]) {

  int sock, window = 17520;
  unsigned char recbuff[LENGTH], faketcphdr[LENGTH];
  struct sockaddr_in servaddr;
  unsigned short servport, local_port;
  srandom(time(NULL));
  unsigned int rnum = random();

  servport = PORT;

  // Create a socket stream using TCP
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket() failed");
    exit(1);
  }
  // Construct server address structure
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(servport);
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  printf("\nConnecting to server..\n");

  // Make a connection to the server
  if (connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("connect() failed");
    exit(1);
  }
  else printf("Connected to server!\n");

  socklen_t len = sizeof(servaddr);

  if(getsockname(sock, (struct sockaddr *)&servaddr, &len) < 0) {
    perror("getsockname() failed");
    exit(1);
  }
  else local_port = ntohs(servaddr.sin_port);

  // fill out fake tcp header
  faketcphdr[0] = (local_port >> 8) + '\0'; // Source port
  faketcphdr[1] = (local_port & (unsigned char)0x00FF) + '\0';
  faketcphdr[2] = (servport >> 8) + '\0'; // Destination port
  faketcphdr[3] = (servport & (unsigned char)0x00FF) + '\0';
  faketcphdr[4] = (rnum >> 24) + '\0'; // Sequence number
  faketcphdr[5] = (rnum >> 16) + '\0';
  faketcphdr[6] = (rnum >> 8) + '\0';
  faketcphdr[7] = (rnum & (unsigned)0x000000FF);
  faketcphdr[8] = '\0'; // Acknowledgement number
  faketcphdr[9] = '\0';
  faketcphdr[10] = '\0';
  faketcphdr[11] = '\0';
  faketcphdr[12] = '\0'; // Data offset + Reserved + NS flag
  faketcphdr[13] = 2 + '\0'; // Rest of flags
  faketcphdr[14] = (window >> 8) + '\0'; // Window size
  faketcphdr[15] = (window & 0x00FF) + '\0';
  faketcphdr[16] = (unsigned char)0xFF; // TCP checksum
  faketcphdr[17] = (unsigned char)0xFF;
  faketcphdr[18] = '\0'; // urg pointer
  faketcphdr[19] = '\0';

  printf("Sending fake TCP header..\n");

  if (send(sock, faketcphdr, sizeof(faketcphdr), 0) < 0) {
    perror("send() failed");
    exit(1);
  }
  printf("Sent.\n");

  int n;
  unsigned int server_seq;

  // Receive message from server
  printf("Receiving packet from server..\n\n");
  printf("Step 2: (2nd packet)\n\n");
  n = recv(sock, recbuff, LENGTH, 0);

  print_header(recbuff);

  unsigned char temp1, temp2;

  // Switch sequence and acknowledgement number
  temp1 = recbuff[2];
  temp2 = recbuff[3];

  recbuff[2] = recbuff[0];
  recbuff[3] = recbuff[1];

  recbuff[0] = temp1;
  recbuff[1] = temp2;

  server_seq = recbuff[4] << 24 | recbuff[5] << 16 | recbuff[6] << 8 | recbuff[7];
  server_seq++;

  // set new sequence number
  recbuff[4] = recbuff[8];
  recbuff[5] = recbuff[9];
  recbuff[6] = recbuff[10];
  recbuff[7] = recbuff[11];

  // set new Acknowledgement number
  recbuff[8] = (server_seq >> 24) + '\0';
  recbuff[9] = (server_seq >> 16) + '\0';
  recbuff[10] = (server_seq >> 8) + '\0';
  recbuff[11] = (server_seq & (unsigned)0x000000FF);

  // unset syn flag
  recbuff[13] = recbuff[13] & 0b00010000;

  send(sock, recbuff, sizeof(recbuff), 0);

  // ** Part 2 ** //
  printf("*** Part 2 ***\n\n");
  char recbuff2[32];
  n = recv(sock, recbuff2, sizeof(recbuff2), 0);

  printf("Recieved resolved IP address of hostname www.example.com from server. The IP address is: %s\n\n", recbuff2);

  // Create request line to send
  char request[128];
  sprintf(request, "GET /index.html HTTP/1.0\r\nHost: %s\r\n\r\n", recbuff2);

  close(sock);
  printf("Connection closed.\n\n");

  // make 2nd connection to recieved IP address
  struct sockaddr_in servaddr2;
  char recbuff3[2048];

  int sock2;
  if ((sock2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket() failed");
    exit(1);
  }
  // Construct server address structure
  bzero(&servaddr2, sizeof(servaddr2));
  servaddr2.sin_family = AF_INET;
  servaddr2.sin_port = htons(80);
  servaddr2.sin_addr.s_addr = inet_addr(recbuff2);

  printf("Connecting to (%s)..\n", recbuff2);

  // Make a connection to recieved IP address
  if (connect(sock2, (struct sockaddr *)&servaddr2, sizeof(servaddr2)) < 0) {
    perror("connect() failed");
    exit(1);
  }
  else printf("Connected!\n");

  printf("Sending GET request..\n\n");
  send(sock2,request,strlen(request),0);

  // Display results of GET request
  n = recv(sock2, recbuff3, sizeof(recbuff3), 0);
  printf("%s\n", recbuff3);

  close(sock2);
  printf("Connection closed.\n");

}

/* function that takes a 20 byte unsgined char array and turns the bytes into
   human readable format. Bitwise operations and bit masks are used
   achieve this.
*/
void print_header(unsigned char hdr[LENGTH]) {

  unsigned short src_port, dest_port, data_offset, reserved, win_size, chk_sum, urg_ptr;
  unsigned int seq_num, ack_num;

  short reserved_mask = 0b00001110, ns_mask = 0b00000001;

  bool ns, cwr, ece, urg, ack, psh, rst, syn, fin;

  src_port = hdr[0] << 8 | hdr[1];
  dest_port = hdr[2] << 8 | hdr[3];

  seq_num = hdr[4] << 24 | hdr[5] << 16 | hdr[6] << 8 | hdr[7];
  ack_num = hdr[8] << 24 | hdr[9] << 16 | hdr[10] << 8 | hdr[11];

  data_offset = hdr[12] >> 4;
  reserved = (hdr[12] & reserved_mask) >> 1;
  ns = (hdr[12] & ns_mask);

  cwr = (hdr[13] & 0b10000000) >> 7;
  ece = (hdr[13] & 0b01000000) >> 6;
  urg = (hdr[13] & 0b00100000) >> 5;
  ack = (hdr[13] & 0b00010000) >> 4;
  psh = (hdr[13] & 0b00001000) >> 3;
  rst = (hdr[13] & 0b00000100) >> 2;
  syn = (hdr[13] & 0b00000010) >> 1;
  fin = (hdr[13] & 0b00000001);

  win_size = hdr[14] << 8 | hdr[15];
  chk_sum = hdr[16] << 8 | hdr[17];
  urg_ptr = hdr[18] << 8 | hdr[19];

  printf("Raw TCP Header:\n");
  int i;
  for(i = 0; i < 20; i++ ) {
    if(i % 4 == 0) {
      printf("\n");
    }
    printf(" %02X", hdr[i]);
  }
  printf("\n\nTCP Header:\n");
  printf("\n Source port: %u", src_port);
  printf("\n Destination port: %u", dest_port);
  printf("\n Sequence number: %u", seq_num);
  printf("\n Acknowledgement number: %u", ack_num);
  printf("\n Data offset: %u", data_offset);
  printf("\n Reserved: %u", reserved);
  printf("\n\n Control flags:");
  printf("\n NS:  %u\n CWR: %u\n ECE: %u\n URG: %u\n ACK: %u", ns, cwr, ece, urg, ack);
  printf("\n PSH: %u\n RST: %u\n SYN: %u\n FIN: %u\n", psh, rst, syn, fin);
  printf("\n Window size: %u", win_size);
  printf("\n TCP checksum: %u", chk_sum);
  printf("\n Urgent pointer: %u\n\n", urg_ptr);

}
