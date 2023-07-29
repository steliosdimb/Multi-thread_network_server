#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;

struct thread_args
{
    string server; // Server name
    int port;      // Port number
    string vote;   // Vote string
    string name;   // name
};

void *thread_client(void *args)
{
    int sock;
    struct sockaddr_in server;
    unsigned int serverlen;
    struct hostent *rem;
    struct sockaddr *serverptr;
    struct thread_args *arg = static_cast<thread_args *>(args);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        exit(-1);
    }

    if ((rem = gethostbyname(arg->server.c_str())) == nullptr)
    {
        perror("gethostbyname");
        exit(-1);
    }

    server.sin_family = AF_INET;
    bcopy((char *)rem->h_addr, (char *)&server.sin_addr, rem->h_length);
    server.sin_port = htons(arg->port);
    serverptr = (struct sockaddr *)&server;
    serverlen = sizeof server;

    if (connect(sock, serverptr, serverlen) < 0)
    {
        perror("connect");
        exit(-1);
    }
    char nameBuffer[40];
    if (read(sock, nameBuffer, sizeof(nameBuffer)) == -1)   //send name please
    {
        perror("read");
        close(sock);
        return nullptr;
    }
    if (send(sock, arg->name.c_str(), arg->name.size(), 0) == -1)
    {
        perror("send");
    }
    char voteBuffer[40];
    if (read(sock, voteBuffer, sizeof(voteBuffer)) == -1) //send vote please
    {
        perror("read");
        close(sock);
        return nullptr;
    }
    if (send(sock, arg->vote.c_str(), arg->vote.size(), 0) == -1)
    {
        perror("send");
    }
    char acBuffer[150];
    if (read(sock, acBuffer, sizeof(acBuffer)) == -1)   //get vote name
    {
        perror("read");
        close(sock);
        return nullptr;
    }
    close(sock);
    return nullptr;
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cout << "Sorry input must be in this form: ./pollSwayer [serverName] [portNum] [inputFile.txt]" << endl;
        return 1;
    }

    string server = argv[1];
    int port = stoi(argv[2]);
    string file = argv[3];

    fstream f;
    f.open(file, ios::in); // Open the file for reading

    if (!f.is_open())
    {
        return 1;
    }
    perror("open");

    string line;
    vector<pthread_t *> threads;
    vector<thread_args*> threadArgs;
    while (getline(f, line))
    {
        string vot,nam1,nam2,nam;
        thread_args *thr = new thread_args();
        size_t pos=line.find(" ");
        nam1=line.substr(0,pos);
        line.erase(0,pos+1);
        pos=line.find(" ");
        nam2=line.substr(0,pos);
        nam=nam1+" "+nam2;
        line.erase(0,pos+1);
        vot=line;
        vot.push_back('\0');
        nam.push_back('\0');
        // Store thread arguments
        thr->server = server;
        thr->port = port;
        thr->vote = vot;
        thr->name = nam;

        pthread_t *thread = new pthread_t;
        if (pthread_create(thread, nullptr, thread_client, static_cast<void *>(thr)) != 0)
        {
            perror("thread create");
            delete thr;
            delete thread;
            return 1;
        }

        threads.push_back(thread);
        threadArgs.push_back(thr);
    }

    for (auto thread : threads)
    {
        pthread_join(*thread, nullptr);
        delete thread;
    }
    for (auto thr : threadArgs)
    {
        delete thr;
    }

    f.close();
    return 0;
}