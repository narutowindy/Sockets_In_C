#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>   //hostent

int error = 0;
struct addrinfo hints, *infoptr;

// The Information struct stores the results of the doParse and doLookup function
struct Information
{
  char Host[1024];
  char IP[128];
  char Port[16];   // max number of ports is 65535
  char Path[1024];
} ParseInformation;

void doLookup()
{
  error = 0;
  hints.ai_family = AF_INET;

  int result = getaddrinfo(ParseInformation.Host, NULL, &hints, &infoptr);
  if (result)
  {
    printf("Error: name or service not know\n");
    error = 1;
    return;
  }
  struct addrinfo *p;
  p = infoptr;
  while(p != NULL)
  {
    getnameinfo(p->ai_addr, p->ai_addrlen,ParseInformation.IP, sizeof(ParseInformation.IP), NULL, 0, NI_NUMERICHOST);
    p = p->ai_next;
  }
  freeaddrinfo(infoptr);
}

// This function takes for example
// "GET http://qprober.cs.columbia.edu/publications/sigmod2001-abstract.html HTTP/1.0"
void doParse(char urlcopy[])
{
  int PortIndex = 0;
  int PathIndex = 0;
  int Index = 0;
  int i;
  // Finds where the ':' is in the string
  for(i = 0; i < strlen(urlcopy); i++)
  {
    if(urlcopy[i] == ':')
    {
      Index = i + 3;
      break;
    }
  }
  char str[strlen(urlcopy)-Index-10];
  memset(&str, 0, sizeof(str));
  int size = strlen(urlcopy)-Index-10; //I use this because something str array is work and this make sure I get the right the information!
  // This deletes the "GET http://" and " HTTP/1.0"
  for(i = 0; i<strlen(urlcopy)-Index-10;i++)
  {
    str[i] = urlcopy[i+Index];
    printf("letters = %c\n", urlcopy[i+Index]);
  }
 
  int SizeofArray = strlen(str);
  // Finds if there is a port number or a path giving with the url
  for(i = 0; i < SizeofArray; i++)
  {
    if(str[i] == ':' && PortIndex == 0)
    {
      PortIndex = i;
    }
    else if(str[i] == '/' && PathIndex == 0)
    {
      PathIndex = i;
    }
  }
  
  // No port number but, Path was giving
  if(PortIndex == 0 && PathIndex > 0)
  {
    strncpy(ParseInformation.Host,str,PathIndex);
    ParseInformation.Port[0] = '8';
    ParseInformation.Port[1] = '0';
    if(strlen(str) == PathIndex + 1)
    {}
    else
      strncpy(ParseInformation.Path,str+PathIndex,size-PathIndex);
  }
  // Both port number and Path was giving
  else if(PortIndex > 0 && PathIndex > 0)
  {
    strncpy(ParseInformation.Host,str,PortIndex);
    strncpy(ParseInformation.Port,str+PortIndex+1,PathIndex-PortIndex-1);
    if(strlen(str) == PathIndex + 1 + PortIndex)
    {}
    else
      strncpy(ParseInformation.Path,str+PathIndex,size-PathIndex);
  }
  // No path but, Port number was giving
  else if(PortIndex > 0 && PathIndex == 0)
  {
    strncpy(ParseInformation.Host,str,PortIndex);
    strncpy(ParseInformation.Port,str+PortIndex+1,size-PortIndex);
  }
  // Both port number and Path were not giving
  else
  {
    strncpy(ParseInformation.Host,str,size);
    ParseInformation.Port[0] = '8';
    ParseInformation.Port[1] = '0';
  }
  doLookup();
}

int main (int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Error: Port number was not giving\n");
    exit(1);
  }
  else if(argc > 2)
  {
    printf("Error: Port number only required \n");
    exit(1);
  }

  int PortNumber = atoi(argv[1]);
  if(PortNumber <= 2000)
  {
    printf("Error: Port number should be above 2000 \n");
    exit(1);
  }

  int server_fd, client_fd;
  struct sockaddr_in server, client;
  while(1)
  {
    memset(&ParseInformation, 0, sizeof(ParseInformation)); // IDK about it 
    char buffer[65535];
    char url[1024];
    memset(&buffer, 0, sizeof(buffer));
    memset(&url, 0, sizeof(url));
    printf("fist = %s\n", url);
    int BRLock = 0;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 1)
    {
      printf("Error: Could not create socket\n");
      exit(1);
    }
  
    server.sin_family = AF_INET;
    server.sin_port = htons(PortNumber);
    server.sin_addr.s_addr = INADDR_ANY;
  
    if(bind(server_fd,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
      printf("Error: Could not bind socket\n");
      exit(1);
    }
    printf("bind done: successful\n");
  
    if(listen(server_fd, 10) < 0)
     {
      printf("Error: Could not listen\n");
      exit(1);
    }
  
    printf("stage 1 program by ka267 listening on %d\n", PortNumber);
  
    socklen_t client_len = sizeof(client);
    client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);
    if(client_fd < 0)
    {
      printf("Error: Could not connepict to the Port Number\n");
    }
    printf("Connected\n");
  
    int i;
    recv(client_fd, buffer, 65535, 0);
    puts(buffer);
    for(i = 0; i < strlen(buffer); i++)
    {
       if(buffer[i] == '\n')
      {
        break;
      }
      else
      {
        url[i] = buffer[i];
      }
    }
    doParse(url); // url
    printf("Host = %s\n", ParseInformation.Host);
    printf("Path = %s\n", ParseInformation.Path);
    printf("IP = %s\n", ParseInformation.IP);
    printf("Port = %s\n", ParseInformation.Port);
    if(error == 1)
      close(client_fd);
    else
    {
      send(client_fd, "HTTP/1.1 200 OK\r\n",16,0);
      send(client_fd, "Content-length: 65535\n",19,0);
      send(client_fd, "Content-Type: text/html\n\n",25,0);
      send(client_fd, "<html><body>",12,0);
      for(i = 0; i < strlen(buffer); i++)
      {
        if(buffer[i] == '\n')
        {
          if(BRLock == 0)
          {
            send(client_fd, "<br>", 4,0);
            BRLock = 1;
          }
        }
        else
        {
          BRLock = 0;
          send(client_fd,&buffer[i],1,0);
        }
      }
      send(client_fd, "<font color=\"red\"> HOSTIP = ",28,0);
      send(client_fd, ParseInformation.Host, strlen(ParseInformation.Host),0);
      send(client_fd, "&nbsp(", 6,0);
      send(client_fd, ParseInformation.IP, strlen(ParseInformation.IP),0);
      send(client_fd, ")<br>", 5,0);
      send(client_fd, "PORT &nbsp&nbsp&nbsp&nbsp= ",27,0);
      send(client_fd, ParseInformation.Port, strlen(ParseInformation.Port),0);
      send(client_fd, "<br>", 4,0);
      send(client_fd, "PATH &nbsp&nbsp&nbsp&nbsp= ",27,0);
      send(client_fd, ParseInformation.Path, strlen(ParseInformation.Path),0);
      send(client_fd, "</font></p>",11,0);
      send(client_fd, "</body></html>",14,0);
      close(client_fd);
    }
    PortNumber++;
  }

  return 0;
}
