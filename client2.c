#include "requirement.h"

void socketcreate(int *socket_fd,int domain,int type,int proto)
{
    if ((*socket_fd = socket (domain, type, proto)) <0)
        {
            perror("Problem in creating tcp socket\n");
            exit(2);
        }
}

void socketconnect(int *socket_fd,struct sockaddr_in *server_addr)
{
    if (connect(*socket_fd, (struct sockaddr *) & *server_addr, sizeof(*server_addr))<0)
        {
            perror("Problem in connecting to the server\n");
            exit(3);
        }
}

void messagerecieve(int *socket_fd,struct message *msg)
{
    if (recv(*socket_fd, &(*msg), sizeof(*msg),0) == 0)
    {
        perror("The server terminated prematurely\n");
        exit(4);
    }
}


int main(int argc, char **argv)
{
	if (argc !=3)
    {
        printf("Server address or port no. missing\nensure: <filename> <server address> <server port>\n");
        exit(1);
    }
    int socket_fd,udp_port,server_port_no = atoi(argv[2]);
    struct sockaddr_in server_addr;
    struct message msg;
	socklen_t servlen;
   

    socketcreate(&socket_fd,AF_INET, SOCK_STREAM,0);


    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr= inet_addr(argv[1]);
    server_addr.sin_port =  htons(server_port_no);

    socketconnect(&socket_fd,&server_addr);
   
    msg.type = 1;
	msg.length = 0;
    send(socket_fd, &msg, sizeof(msg), 0);
    printf("Connected to %s via TCP at port %d || Sent type '%d' message \n",inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port), msg.type);
    messagerecieve(&socket_fd,&msg);
    printf("Connected to %s via TCP at port %d || Received type '%d' message \n",inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port), msg.type);
    
    udp_port = msg.length;
    printf("%s returned UDP port '%d' for connection \n",inet_ntoa(server_addr.sin_addr),  msg.length);
    printf("%s is closing TCP connection %d(0 if success)\n",inet_ntoa(server_addr.sin_addr), close(socket_fd));
     socketcreate(&socket_fd,AF_INET, SOCK_DGRAM,0);


    servlen = sizeof(server_addr);
    server_addr.sin_port = htons(udp_port);
    msg.type=3;
    
    printf("Connected to %s via UDP at port %d \nsend a message :",inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

    fgets(msg.msg,sizeof(msg.msg),stdin);
    msg.length = strlen(msg.msg);
	msg.msg[msg.length-1]='\0';
    sendto(socket_fd, &msg, sizeof(msg), 0,(struct sockaddr *)&server_addr, sizeof(server_addr));
    
    printf("Message sent to %s via UDP at port %d of type '%d' \n",inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port), msg.type);

    recvfrom(socket_fd,&msg,sizeof(msg),0,(struct sockaddr *)&server_addr, &servlen);
    msg.msg[msg.length]='\0';
    
    printf("%s sent a  message via UDP at port %d of type '%d' \n",inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port), msg.type);
    printf("Acknowledgement 'success' Received from %s at port %d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port)); 
    printf("%s terminating connection \n", inet_ntoa(server_addr.sin_addr));
   
    return 0;
}
