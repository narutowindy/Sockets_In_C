/**********************************************************************************
* *   
* *  usage:
* *   
* *  ./stage2 <portnumber>
* *     
* *  HTTP websites list:
* *  =============================
* *  http://gaia.cs.umass.edu/wireshark-labs/HTTP-wireshark-file1.html
* *  http://gaia.cs.umass.edu/wireshark-labs
* *  http://moodle.njit.edu
* *  http://opensecuritytraining.info
* *  http://exercism.io
* *  http://bbc.com
* *  
* *  Tested environment :
* *  ===========================
* *  Linux environment
* *  Firfox 59 (64-bit)
* *  and also tested on NJIT AFS
* *  using correct HTTP <IP:Port> proxy settings
* *  
* *  NOTE:
* *  ====
* *  
* *    "google-chrome" is connects to the https://apis.google.com to fetch the google sign-in user information, at the starting of an application
* *    and also some intervels, this will try to establish the connection with the https:// servers  results in the 
* *	   program termination..
* * 
* *  */



#include <stdio.h> 
#include <string.h>   //strlen 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <arpa/inet.h>   
#include <netdb.h>
#include <netinet/in.h> 
#include <errno.h> 
#include <sys/types.h> 
#include <sys/sendfile.h>
#include <fcntl.h>
#include <sys/time.h>

struct Information{						//  Information 
	char req[1024];						// > request
	char domain[1024];					// Domain 
	char path[1024];			 
}Info;									//  Info struture to hold the HTTP request

// Info is a data structure contains 3  arrays   { req[], domain[] ,path[] }

struct Information doParse(char req_in[4096]) {      //  Information
	strcpy(Info.req, req_in);                       // initialize request in req_in == Info.req 
	char *ptr,*qtr, t1[256], t2[256];
	int j = 0 , m = 0 ; 
	int i,k;
  	for(i=0; i<=1024; i++)
	{      Info.domain[i] = 0;
    		Info.path[i] =0;}
   	for(i=0; i<=256; i++)
	{  		t1[i] = 0;
    		t2[i] =0;}

    	
	if( ptr = strstr(Info.req,"Host:" )){     			// i = 6 because   "Host:"  coantains 5  chars : p pointer to i=6  
		 for( i = 6; i <= strlen(ptr); i++) {
			t1[j] = ptr[i];
			 j++;      							// j = 0 inialized
			if(ptr[i]== '\r'){			// untill carriage return, store it in to *p ...
				break;
			}
		 }
			
		 for (i=0; i < (strlen(t1)-1); i++){ 	  // t1[] == "sasdasdasd\r\0"
			Info.domain[i] = t1[i];      		 //  removing \r from tmp and assign to Info.domain
		 }
		printf("\n");	
		printf(" \"%s\" Domain has %lu chars \n ",Info.domain, strlen(Info.domain));
	}

	else if( ptr = strstr(req_in,"http")){
				for(i=7;i<strlen(ptr);i++){
					t1[j]=ptr[i];
					j++;
					if(ptr[i]== '/' ){     //  domain/path/path.html
						break;
					}
				}
			for(k=0; k<(strlen(t1)-1); k++){
    			Info.domain[k] = t1[k];
    		}

			for(;;i++){
				t2[m]=ptr[i];
				m++;
				if(ptr[i]== ' '){
					break;
					}
			}
			
			for(k=0; k<(strlen(t2)-1); k++){
    				Info.path[k] = t2[k];      
				}
			
			printf("\n");	
			puts(Info.domain);
			puts(Info.path);
			printf("%lu", strlen(Info.domain));
			printf("%lu", strlen(Info.path));	               // Domain extraction ..... 			
	}

	
			if(strstr(req_in,"Referer:")){
				qtr = strstr(req_in,"GET /");
				j=0;				
				for(i=4;i<strlen(qtr);i++){
					t2[j]=qtr[i];
					j++;
					if(qtr[i]== ' '){                    //  space char.. 
						break;
					}
				}
				for(i=0; i<(strlen(t2)-1); i++){
    					Info.domain[i] = t2[i];
    				}							
				printf("\n");	
				puts(Info.domain);
				printf("%lu", strlen(Info.domain));    // extracting path from request... 
				printf("\n");					
		}
  
	return Info;
}


// DNS Lookup
char * doLOOKup(char *hostname){
	int sockfd;  
	struct addrinfo hints, *results, *p;
	struct sockaddr_in *ip_access;	
	int rv;
	char *ipv4;	

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; 
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(hostname, "domain", &hints, &results)) != 0) {
    		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    		exit(1);
	}
	p = results;
	ip_access = (struct sockaddr_in*) p -> ai_addr;
	ipv4 = inet_ntoa(ip_access -> sin_addr);
	printf("IP address for domain %s is: %s\n",hostname, ipv4);
	printf("\n\n\n");
	freeaddrinfo(results);
	return ipv4;
	
}

char * proxyclient( char addr_in[] ){
	char *address, *host_addr;
	char response[10800];
	int proxy_client;
 	char * resp_out = response;
	
	doParse(addr_in);
	//host_addr == hostname 
	address =  doLOOKup(Info.domain);
	//begin in proxy CLIENT:
	if((proxy_client = socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
	perror(" Client's Socket >: ");
	exit(0);	
	}
	
	printf(" %lu ", sizeof(address));

	struct sockaddr_in remote_address;
	remote_address.sin_family = AF_INET;
	remote_address.sin_port = htons(80);
	inet_aton(address, (struct in_addr *)&(remote_address.sin_addr.s_addr));
	
	//Connecting the server. TCP connection establishes. 
	connect(proxy_client, (struct sockaddr *)&remote_address, sizeof(remote_address));
	
	//Sending the HTTP Request to the Web-Server. 
	send(proxy_client, Info.req, sizeof(Info.req), 0);
	printf("Request sent to server: %s\n", Info.req);
	
	//Recieving the Response from the Web-Server. 
	recv(proxy_client, &response, sizeof(response), 0);

	printf("response froms server: %s\n", response);
	close(proxy_client);		
	return resp_out;
}


    
int main(int argc , char *argv[])  
{  	
		if(argc != 2){
				printf("\n\n\tUsage: ./stage2 <portnumber>  \n\n\tex: port= 8080 is a random port number\n");
				return -1 ;
		}
	
    	struct sockaddr_in address;
    	int port = atoi(argv[1]);	  
    	int main_socket , addrlen , nw_socket;
		int actv, i, on =1 ;
		int read_val , sd ;  
    	int max_sd , client_socket[32] , max_cli= 32 ; 
    	char act_response[10800];
	
     	char buffer[1024];  //data buffer of 1K 
	
        printf("HTTP proxy is running.. \n\n\n");
    	fd_set readfds;  
        printf(" Project : HTTP Proxy Server \n Prof: Kumar Mani\n Assignment 2 \n Group Members: Koustav,Srinath,Bharat.\n \n\n\n");

    	char *message = "Connection to Proxy Iniatialized...\r\n";  
    
    	for (i = 0; i < max_cli; i++)  
        	client_socket[i] = 0;  
        
    	if( ( main_socket = socket(AF_INET , SOCK_STREAM , 0) ) < 0)  {  
        	perror("Error:socket ");  
        	exit(0);  
    	}  
    
   	 if( setsockopt(main_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0 )  
    	{  
        	perror("Error: ");  
        	exit(0);  
   	    }  
    
    	address.sin_family = AF_INET;  
    	address.sin_addr.s_addr = INADDR_ANY;  
    	address.sin_port = htons(port);  
        
    	if ( bind ( main_socket, (struct sockaddr *)&address, sizeof(address) ) < 0)  {  
       			 perror("Error: bind :");  
        		exit(0);  
    	}  
    	
	printf("Listener on port %d \n", port);  
        
    if (listen(main_socket, 5) < 0){  
       		 perror("Error:");  
        	exit(0);  
    }  
        
    	//accept the incoming connection 
    addrlen = sizeof(address);  
    puts("Waiting for connections ...");  
        
	while(1){
			  
				FD_ZERO(&readfds);  
				FD_SET(main_socket, &readfds);  
				max_sd = main_socket;  
					
				for ( i = 0 ; i < max_cli ; i++){  
					sd = client_socket[i];  
					if(sd > 0)  
						FD_SET( sd , &readfds);  
					if(sd > max_sd)  
						max_sd = sd;  
				}  
			
				actv = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
				if ((actv < 0) && (errno!=EINTR)) 
					printf(" Error: ");  
				  
					
				if (FD_ISSET(main_socket, &readfds)){  
					if ((nw_socket = accept(main_socket,(struct sockaddr *)&address, (socklen_t*)&addrlen))<0){  
						perror("Error: accept");  
					}  
					
				 printf(" new sock no. %d , ip is : %s , port : %d \n" , nw_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); 
						
				  for (i = 0; i < max_cli; i++)  {  
						if( client_socket[i] == 0 ){  
							client_socket[i] = nw_socket;  
							printf("Adding to list of sockets as %d\n" , i);    
							}        /// ???
						break;
					}  
				}  
					
				for (i = 0; i < max_cli; i++){  
					sd = client_socket[i];  		
					if (FD_ISSET( sd , &readfds)){  

							if ((read_val = read( sd , buffer, 1024)) == 0){  
								getpeername(sd , (struct sockaddr*)&address ,(socklen_t*)&addrlen);  
								printf("Disconnected by ip %s and the port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
							   close( sd );  
							   client_socket[i] = 0;  
							   
							}
							else{
								buffer[read_val] = '\0';  
								strcpy(act_response,(proxyclient(buffer)));          
								if(strcmp(buffer, "GET / HTTP/1.1\r\n")){
								write(sd, act_response, strlen(act_response));
								 }
							
							} 
					}  
					break;
				}  
    }  
        
    return 0;  
}  
