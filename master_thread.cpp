    #include <iostream>
    #include <netdb.h>
    #include <string.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <fstream>
    #include <unistd.h>
    #include <sstream>
    #include <csignal>
    #include <pthread.h>
    #include <algorithm>
    #include <unordered_map>
    #include <string>
    #include <vector>
    #include <signal.h>
    #include "server.hpp"
    #include "workers_threads.hpp"
    #include "global.hpp"
    using namespace std;
    void signalHandler(int signum) {
        pthread_mutex_lock(&terminateMutex);
        if(signum==SIGINT){
            terminateServer=true;
        }
        pthread_mutex_unlock(&terminateMutex);
        // Open stats file for writing
        pthread_mutex_lock(&sfmtx);
        fstream statsFile;
        statsFile.open(txt2,ios::app);

        // Write party votes to stats file
        pthread_mutex_lock(&pmtx);
        for (auto const &pair : party_votes) {
            statsFile << pair.first << "  " << pair.second << endl;
        }
        pthread_mutex_unlock(&pmtx);
        // Calculate and write total votes
        pthread_mutex_lock(&vmtx);
        int totalVotes = voter_names.size();
        statsFile << "TOTAL " << totalVotes << endl;
        pthread_mutex_unlock(&vmtx);
        // Close the stats file
        statsFile.close();
        pthread_mutex_unlock(&sfmtx);

        // Terminate the server gracefully
        exit(0);
    }   
    int main(int argc, char *argv[])
    {
        signal(SIGINT, signalHandler);
        int portnum;
        int numWorkerthreads;
        int buffersize;
        vector<string>parties;
        vector<int>parties_votes;
        string poll_log;
        string poll_stats;
        store_vars(&portnum, &numWorkerthreads, &buffersize, &poll_log, &poll_stats, argc, argv); //store variables from command line
        txt1=poll_log; //store name of poll log in the global variable txt1
        txt2=poll_stats;
        int sock, lsock, csock;
        struct sockaddr_in myaddr;
        if (lsock == -1) {
            perror("socket");
            return 1;
        }
        create_socket(&sock,&lsock,&csock,myaddr,portnum);
        pthread_t *workers_threads;
        workers_threads=create_workers(numWorkerthreads);
        while(1){
            pthread_mutex_lock(&terminateMutex);
            if (terminateServer) { //check if signal was given
                pthread_mutex_unlock(&terminateMutex);
                break;
            }
            pthread_mutex_unlock(&terminateMutex);
            accept_connection(csock,lsock,buffersize);//accept connection
            if (csock == -1) {
                perror("accept");
                break;
            }
        }
        join_threads(numWorkerthreads,workers_threads);
        close(lsock);//close listening socket
        
    }


