/*	
 *	steps:
 *	1. open a file as read  fopen() . return value needed wil be used in sendto()  .. size can be set from commands...
 *	2. send it through udp,( max 65kb per packet ) until the read from the file reaches EOF .
 *		Usage : ./a3 <client.IP> <port.NO> </path/to/filename> <Buff.size> <Delay.ms>
 * */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

void handle(int );   // define 

void handle(int err){     // declaration
	perror("Error:");
	exit -1;
}

int main(int argc, char **argv){

	if(argc < 2){
		printf("\nUsage: \n\t./a3 <client.ip> <Local.port.no> </path/to/filename> <Buff.size.bytes> <Delay.u-sec>\n ");
		return 0;
	}

	int socketfd,fp, port, sent ,rd = 0, MTU = atoi(argv[4]) ;
	char  buffer[MTU] ,*host;   

	host = argv[1];
	port = atoi( argv[2] );

	struct sockaddr_in client ;
	
	memset(&client, 0,sizeof(client));
	client.sin_family = AF_INET;
	client.sin_port = htons(port);
	client.sin_addr.s_addr = inet_addr(host);

	socketfd = socket(PF_INET , SOCK_DGRAM,0);
	
	if(socketfd < 0) handle(socketfd);

	fp = open(argv[3],O_RDONLY);
	if(fp < 0) handle(fp);
	while(	( rd = read(fp,buffer,MTU) ) > 0 ){
		sent = sendto(socketfd,buffer,rd,0,(struct sockaddr *)&client,sizeof(client));
		if(sent < 0) handle(sent);
		printf("\nData is sent is %d bytes\n",sent);
		memset(&buffer,0,sizeof(buffer));
		usleep(atoi(argv[5]));
	}
	
	close(fp);
	close(socketfd);

	return 0;
}

