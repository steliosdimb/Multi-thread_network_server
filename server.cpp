#include "server.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
void store_vars(int *port, int *numworkers, int *buffers, string *poll_log, string *poll_stats, int argc, char *argv[])
{
    if (argc != 6)
    {
        cout << "Wrong arguments,arguments must be in this kind of form ./poller [portnum] [numWorkerthreads] [bufferSize] [poll-log][poll-stats]!" << endl;
        exit(1);
    }
    for (int i = 1; i < argc; i++)
    {
        if (i == 1)
        {
            *port = atoi(argv[i]);
        }
        else if (i == 2)
        {
            *numworkers = atoi(argv[i]);
        }
        else if (i == 3)
        {
            *buffers = atoi(argv[i]);
        }
        else if (i == 4)
        {
            *poll_log = argv[i];
        }
        else if (i == 5)
        {
            *poll_stats = argv[i];
        }
    }
}
void create_socket(int * sock,int * lsock,int * csock,struct sockaddr_in myaddr,int portnum){
    if ((*lsock = socket(AF_INET, SOCK_STREAM, 0)) < 0) //create a socket
        perror(" socket ");
    //bind socket
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(portnum); //port to bind sokcket
    myaddr.sin_family = AF_INET;    //internet address family
    if (bind(*lsock, (struct sockaddr *)&myaddr, sizeof(myaddr)))
        perror(" bind ");
    if (listen(*lsock, 128) != 0)    //listen for 128 connections
        perror(" listen ");
}