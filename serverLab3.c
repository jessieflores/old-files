/*********
ICS 451 Spring 2020
lab 3
Jessie Flores
*********/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#define MAXPENDING 5
#define LENGTH 20
#define PORT 12345

void print_header(unsigned char hdr[LENGTH]);

int main(int argc, char *argv[]) {

   int servsock, clntsock, valread;
   struct sockaddr_in servaddr, clntaddr;
   unsigned short servport;
   unsigned char buffer[LENGTH];

   servport = PORT;

   // Create a socket stream using TCP
   if ((servsock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
     perror("socket() failed");
     exit(1);
   }

   bzero(&servaddr, sizeof(servaddr));

   // Construct server address structure
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
   servaddr.sin_port = htons(servport);

   // Bind to server address
   if (bind(servsock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
     perror("bind() failed");
     exit(1);
   }
   // Make socket listen for connections
   if (listen(servsock, MAXPENDING) < 0) {
     perror("listen() failed");
     exit(1);
   }

   unsigned int len = sizeof(clntaddr);
   printf("\nWaiting for client to connect..\n");

   // Connect to client
   if ((clntsock = accept(servsock, (struct sockaddr *) &clntaddr, &len)) < 0) {
     perror("accept() failed");
     exit(1);
   }
   else
     printf("Connected to client!\n");

   socklen_t len2 = sizeof(servaddr);
   unsigned short local_port;

   // Get source port number
   if(getsockname(servsock, (struct sockaddr *)&servaddr, &len2) < 0) {
     perror("getsockname() failed");
     exit(1);
   }
   else local_port = ntohs(servaddr.sin_port);

   printf("Receiving packet from client..\n");
   valread = read(clntsock, buffer, LENGTH);

   printf("\n*** Part 1 ***\n\nStep 1: (1st packet)\n\n");
   print_header(buffer);

   unsigned int server_seq, client_seq;
   srandom(time(NULL) + 1); // add 1 to differentiate client seed
   server_seq = random();

   // modify source port, ack flag, sequence, and Acknowledgement number
   buffer[2] = buffer[0];
   buffer[3] = buffer[1];

   buffer[0] = (local_port >> 8) + '\0';
   buffer[1] = (local_port & (unsigned char)0x00FF) + '\0';

   client_seq = buffer[4] << 24 | buffer[5] << 16 | buffer[6] << 8 | buffer[7];
   client_seq++;

   // set new Acknowledgement number
   buffer[8] = (client_seq >> 24) + '\0';
   buffer[9] = (client_seq >> 16) + '\0';
   buffer[10] = (client_seq >> 8) + '\0';
   buffer[11] = (client_seq & (unsigned)0x000000FF);

   // set server sequence number
   buffer[4] = (server_seq >> 24) + '\0';
   buffer[5] = (server_seq >> 16) + '\0';
   buffer[6] = (server_seq >> 8) + '\0';
   buffer[7] = (server_seq & (unsigned)0x000000FF);

   // set ack flag
   buffer[13] = buffer[13] | 0b00010000;

   // Send message to client
   printf("Sending reply packet..\n");
   send(clntsock, buffer, sizeof(buffer), 0);
   printf("Sent.\n");

   printf("Receiving packet from client..\n");
   valread = read(clntsock, buffer, LENGTH);
   printf("\nStep 3: (3rd and final packet)\n\n");
   print_header(buffer);

   // ** Part 2 ** //
   struct addrinfo hints, *infoptr;

   memset(&hints,0,sizeof(hints));
   hints.ai_socktype = SOCK_DGRAM;
   hints.ai_family = AF_INET;

   int result = getaddrinfo("www.example.com", NULL, &hints, &infoptr);
   if(result) {
     perror("getaddrinfo() failed");
     exit(1);
   }
   struct sockaddr_in *addr;
   addr = (struct sockaddr_in *)infoptr->ai_addr;

   char ip_addr[strlen(inet_ntoa((struct in_addr)addr->sin_addr)) + 1];
   strcpy(ip_addr, inet_ntoa((struct in_addr)addr->sin_addr));

   printf("Sending DNS resolved IP address for www.example.com..\n");
   send(clntsock, ip_addr, sizeof(ip_addr), 0);
   printf("Sent.\n\n");

   close(servsock);
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
