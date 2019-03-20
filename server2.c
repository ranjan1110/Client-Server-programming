#include "requirement.h"
//all the header files is included in this.


// function to create a new socket
void socketcreate(int *listenfd,int domain,int type,int proto)
{
    if ((*listenfd = socket (domain, type, proto)) <0)
    {
        perror("Problem in creating tcp socket\n");
        exit(2);
    }
}

//function to make socket ready to listen

void socketlisten(int *listenfd)
{
     if (listen (*listenfd, LISTENQ)<0)
    {
        perror("Problem in listening on tcp socket\n");
        exit(4);
    }
}

//function to bind socket to a port
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
    //if arguments are not correct return error.
	if(argc!=2)
    {
        printf("No server port given \nensure: <filename> <server port>\n");
        exit(1);
    }
    int listenfd, connfd,serv_port = atoi(argv[1]);
    struct message mssg;
    pid_t child_id;
    socklen_t client_add_len;
    char buf[m_len];
    struct sockaddr_in client_addr, server_addr;
    
    socketcreate(&listenfd,AF_INET, SOCK_STREAM,0);//tcp socket is made
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(serv_port);
    
    socketbind(&listenfd,&server_addr);// socket is binded with given port
    socketlisten(&listenfd);// socket is ready to accept connection
    int client_no = 0;
    
    printf("Server is running and ready to connect ..........\n\n");
    for ( ; ; )
    {
        client_add_len = sizeof(client_addr);
        connfd = accept (listenfd, (struct sockaddr *) &client_addr, &client_add_len); // new socket is made to accept the connection from the client
	
	    client_no++;
        if ( (child_id = fork ()) == 0 )  //Child process
        {
            close (listenfd); // socket made earlier is closed 
            if(recv(connfd, &mssg, m_len,0) == 0) //msg is received from client
            {
                perror("The client terminated prematurely\n");
                exit(5);
            }
            printf("client no. %d (%s) connected via TCP at port %d\n",client_no, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            printf("\tReceived type '%d' message from client no. %d (%s)\n", mssg.type, client_no, inet_ntoa(client_addr.sin_addr));	    
    	    server_addr.sin_port = htons(0); // arbitrary a port is assigned for udp

            socketcreate(&listenfd,AF_INET, SOCK_DGRAM,0); // udp socket is created 
            socketbind(&listenfd,&server_addr); // udp port is binded to the socket
    	    struct sockaddr_in localAddress;
    	    socklen_t addressLength = sizeof localAddress;
    	    getsockname(listenfd, (struct sockaddr*)&localAddress,&addressLength);  // the port which was assigned earlier is retrieved and stored in localAddress

            printf("\tPort %d assigned for UDP connection with client no. %d (%s) \n",(int) ntohs(localAddress.sin_port),client_no, inet_ntoa(client_addr.sin_addr)); 
    	    mssg.type = 2;    		
            mssg.length = ntohs(localAddress.sin_port); // udp port is saved in length variable of struct message 
            send(connfd, &mssg, sizeof(mssg), 0);  // message is sent to client with udp port number  	    
            printf("\tsent type '%d' message to client no. %d (%s)\n\n", mssg.type,client_no, inet_ntoa(client_addr.sin_addr));
            printf("client no. %d (%s) TCP connection at port %d CLOSED %d(0 for success)\n\n",client_no, inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port), close(connfd));//tcp connection is closed
    	    
            if(recvfrom(listenfd,&mssg,sizeof(mssg),0,(struct sockaddr *)&client_addr, &client_add_len) == 0) // message is received from the client 
            {
                perror("The client terminated prematurely\n");
                exit(5);
            }

    	    mssg.msg[mssg.length]='\0';
            printf("client no. %d (%s) connected via UDP at port %d || Received type '%d' message: '%s'\n",client_no, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), mssg.type, mssg.msg);           
    	    mssg.type=4;
    	    sendto(listenfd, &mssg, sizeof(mssg), 0,(struct sockaddr *)&client_addr, client_add_len); // server sending message to client via udp 
            printf("\tsent type '%d' message via UDP to client no. %d (%s)\n",mssg.type, client_no, inet_ntoa(client_addr.sin_addr));
            printf("Terminating Connection with client no. %d (%s) \n\n",client_no,inet_ntoa(client_addr.sin_addr)); // udp connection is closed
    	    fflush(stdout);
    	    
    	    return 0;
        }
        close(connfd); //if a child is not created properly then close the socket to accept connection
        
    }
    close(listenfd); //  udp connection is closed when work is completed
    return 0;
}

