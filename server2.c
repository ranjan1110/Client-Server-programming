#include "requirement.h"

void socketcreate(int *listenfd,int domain,int type,int proto)
{
    if ((*listenfd = socket (domain, type, proto)) <0)
    {
        perror("Problem in creating tcp socket\n");
        exit(2);
    }
}

void socketlisten(int *listenfd)
{
     if (listen (*listenfd, LISTENQ)<0)
    {
        perror("Problem in listening on tcp socket\n");
        exit(4);
    }
}

void socketbind(int *listenfd,struct sockaddr_in *server_addr)
{


    if (bind (*listenfd, (struct sockaddr *) &(*server_addr), sizeof(*server_addr))<0)
    {
        perror("Problem in binding to tcp socket\n");
        exit(3);
    }
}



int main (int argc, char **argv)
{
	if(argc!=2)
    {
        printf("No server port given \nensure: <filename> <server port>\n");
        exit(1);
    }
    int listenfd, connfd,serv_port = atoi(argv[1]);
    struct message msg;
    pid_t child_id;
    socklen_t client_add_len;
    char buf[m_len];
    struct sockaddr_in client_addr, server_addr;
    
    socketcreate(&listenfd,AF_INET, SOCK_STREAM,0);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(serv_port);
    
    socketbind(&listenfd,&server_addr);
    socketlisten(&listenfd);
    int c_no = 0;
    
    printf("Server is running and ready to connect ..........\n");
    for ( ; ; )
    {
        client_add_len = sizeof(client_addr);
        connfd = accept (listenfd, (struct sockaddr *) &client_addr, &client_add_len);
	
	c_no++;
        if ( (child_id = fork ()) == 0 )  //Child process
        {
            close (listenfd);
            if( recv(connfd, &msg, m_len,0) == 0)
            {
                perror("The client terminated prematurely\n");
                exit(5);
            }
            printf("client no. %d (%s) connected via TCP at port %d\n",c_no, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            printf("\tReceived type '%d' message from client no. %d (%s)\n", msg.type, c_no, inet_ntoa(client_addr.sin_addr));	    
    	    server_addr.sin_port = htons(0);
            socketcreate(&listenfd,AF_INET, SOCK_DGRAM,0);
            socketbind(&listenfd,&server_addr);
    	    struct sockaddr_in localAddress;
    	    socklen_t addressLength = sizeof localAddress;
    	    getsockname(listenfd, (struct sockaddr*)&localAddress,&addressLength);    	    
            printf("\tPort %d assigned for UDP connection with client no. %d (%s) \n",(int) ntohs(localAddress.sin_port),c_no, inet_ntoa(client_addr.sin_addr));
    	    msg.type = 2;    		
            msg.length = ntohs(localAddress.sin_port);
            send(connfd, &msg, sizeof(msg), 0);    	    
            printf("\tsent type '%d' message to client no. %d (%s)\n", msg.type,c_no, inet_ntoa(client_addr.sin_addr));
            printf("client no. %d (%s) TCP connection at port %d CLOSED %d(0 for success)\n",c_no, inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port), close(connfd));
    	    recvfrom(listenfd,&msg,sizeof(msg),0,(struct sockaddr *)&client_addr, &client_add_len);
    	    msg.msg[msg.length]='\0';
            printf("client no. %d (%s) connected via UDP at port %d || Received type '%d' message: '%s'\n",c_no, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), msg.type, msg.msg);           
    	    // printf("client no %d (%s) || received message at UDP port(%d) : %s\n",c_no, inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port), );
    	    msg.type=4;
    	    sendto(listenfd, &msg, sizeof(msg), 0,(struct sockaddr *)&client_addr, client_add_len);
            printf("\tsent type '%d' message via UDP to client no. %d (%s)\n",msg.type, c_no, inet_ntoa(client_addr.sin_addr));
            printf("Terminating Connection with client no. %d (%s) \n",c_no,inet_ntoa(client_addr.sin_addr));
    	    fflush(stdout);
    	    
    	    return 0;
        }
        close(connfd);
        
    }
    close(listenfd);
    return 0;
}
