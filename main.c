#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h> 

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
fd_set fdset, readfds, writefds;
int sockfd;
int nfds;
int users;
char buffer[1024];
char clients[1024];

int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int	i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

void send_message(int fd)
{
    for(int i = 0; i <= nfds; i++)
        if (i != fd && i != sockfd)
            send(i, "hello", 6, 0);
}


int main() {
	int connfd, len;
	struct sockaddr_in servaddr, cli; 

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(8081); 
  
	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n");
	if (listen(sockfd, 10) != 0) {
		printf("cannot listen\n"); 
		exit(0); 
	}
	len = sizeof(cli);
    nfds = sockfd;
    users = 0;
    FD_ZERO(&fdset);
    FD_SET(sockfd, &fdset);
    bzero(clients, 1024);
    bzero(buffer, 1024);
    printf("SOCKEFD: %d\n", sockfd);
    
    while (1)
    {
        readfds = writefds = fdset;
        if (select(nfds + 1, &readfds, NULL, NULL, NULL) < 0)
            continue; 
        printf("!");
        
        for(int fd = 0; fd <= nfds; fd++)
        {
            //printf("%d/|", fd);
            if (FD_ISSET(fd, &readfds) && fd == sockfd)
            {
                connfd = accept(sockfd, (struct sockaddr *)&cli, (unsigned int *)&len);
                if (connfd < 0) { 
                    printf("server acccept failed...\n"); 
                    exit(0); 
                } 
                else
                {
                    FD_SET(connfd, &fdset);
                    nfds = (connfd > nfds ? connfd : nfds);
                    clients[connfd] = users;
                    printf("client just arrived. %d..\n", users);
                    users++;
                    break;
                }
            }
            else if (FD_ISSET(fd, &readfds))
            {
                if (recv(fd, buffer, 10, 0) <= 0)
                {
                    printf("client just left %d...\n", clients[fd]);
                    bzero(buffer, 1024);
                    close(fd);
                    FD_CLR(fd, &fdset);
                }
                else
                {
                    send_message(fd);
                    // printf("message from client %d\n", clients[fd]);
                }
            }
        }
        printf("----\n");
    }
}
