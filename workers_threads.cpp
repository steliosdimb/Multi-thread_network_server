#include "workers_threads.hpp"
#include <unistd.h>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <queue>
#include <pthread.h>
#include <unordered_map>
#include "global.hpp"
#include "workers_threads.hpp"
using namespace std;

pthread_t* create_workers(int numworkers){
    pthread_t* worker_threads = new pthread_t[numworkers];
    for(int i=0;i<numworkers;i++){
        if(pthread_create(&worker_threads[i],NULL,worker,NULL)!=0){
            perror("thread create");
            exit(1);
        }
    }
    return worker_threads;
}
void* worker(void* arg)
{
    while (1)
    {
        pthread_mutex_lock(&bmtx);//lock buffer mutex now worker is using buffer
        while(buffer.size()==0){    //while buffer queue is empty
            pthread_cond_wait(&buffer_nonempty,&bmtx);
        }
        int clientfd=buffer.front();//the client fd will be on top of the queue buffer
        buffer.pop();//took the client fd now delete it
        pthread_cond_signal(&buffer_nonfull);
        pthread_mutex_unlock(&bmtx);//release buffer mutex we dont use it now
        //check if signal was given
        pthread_mutex_lock(&terminateMutex);
        if (terminateServer) {
            pthread_mutex_unlock(&terminateMutex);
            break;
        }
        pthread_mutex_unlock(&terminateMutex);
        fstream log;
        string msg="SEND NAME PLEASE\n";
        if(send(clientfd,msg.c_str(),msg.length(),0) == -1){ //send request for name to client
            perror("send");
            close(clientfd);
            break;
        }
        char nameBuffer[200]; 
        char voteBuffer[200];
        int bytesread;
        bytesread = read(clientfd, nameBuffer, sizeof(nameBuffer));
        nameBuffer[strcspn(nameBuffer, "\n")] = '\0';
        nameBuffer[strcspn(nameBuffer, "\r")] = '\0';
        pthread_mutex_lock(&vmtx);//lock vector mutex as thread worker is using the vector
        if (find(voter_names.begin(), voter_names.end(), string(nameBuffer)) != voter_names.end()){//check if name already exists in the vector
            pthread_mutex_unlock(&vmtx);
            string msg1="ALREADY VOTED\n";
            if(send(clientfd,msg1.c_str(),msg1.length(),0) == -1){ //send response to client
                perror("send");
            }
            close(clientfd);//close client fd 
            break;
        }
        else{
            pthread_mutex_unlock(&vmtx);//unlock vector mutex 
            string msg2="SEND VOTE PLEASE\n";
            if(send(clientfd,msg2.c_str(),msg2.length(),0) == -1){ //send request for vote to client
                perror("send");
                close(clientfd);
             
                break;
            }
            
            if(read(clientfd,voteBuffer,200) == -1){ //read vote
                perror("read");
                close(clientfd);
           
                break;
            }
            voteBuffer[strcspn(voteBuffer, "\n")] = '\0';
            voteBuffer[strcspn(voteBuffer, "\r")] = '\0';
            string name(nameBuffer);
            string vote(voteBuffer);
            pthread_mutex_lock(&pmtx);//lock party vector mutex
            party_votes[vote]++;//add vote to the party
    
            pthread_mutex_unlock(&pmtx);//unlock mutex
            pthread_mutex_lock(&vmtx);//lock vector mutex 
            voter_names.push_back(name);//push in vector the name of the voter
            pthread_mutex_unlock(&vmtx);//unlock vector mutex,worker dont use vector anymore
        }
        voteBuffer[strcspn(voteBuffer, "\n")] = '\0';
        voteBuffer[strcspn(voteBuffer, "\r")] = '\0';
        string name(nameBuffer);
        string vote(voteBuffer);
        string msg3="VOTE for Party "+vote+" RECORDED\n";
        if(send(clientfd,msg3.c_str(),msg3.length(),0) == -1){ //send response to client
            perror("send");
            close(clientfd);
           
            break;
        }
        pthread_mutex_lock(&fmtx);//lock the mutex because worker thread will write
        log.open(txt1.c_str(),ios::app);
        //open file for appending so wont overwrite it
        log << name << " " << vote << endl; //write to poll log the results from the vote,name and vote*/
        log.close();
        pthread_mutex_unlock(&fmtx);//unlock file mutex now
        close(clientfd);
        
    }
    return NULL;
}
void accept_connection(int csock,int lsock,int buffersize)
{
    if ((csock = accept(lsock, NULL, NULL)) < 0)
        perror(" accept ");
    pthread_mutex_lock(&bmtx); // lock mutex ,now buffer is used by master thread
    while (buffer.size() == buffersize)
    { // wait until buffer is not full
        pthread_cond_wait(&buffer_nonfull, &bmtx);
    }
    buffer.push(csock);                    // push in front of the buffer queue the client socket (fd)
    pthread_cond_signal(&buffer_nonempty); // give a signal to worker for the new connection
    pthread_mutex_unlock(&bmtx);           // unlock buffer mutex
    //check if signal was given
    pthread_mutex_lock(&terminateMutex);
    if (terminateServer) {
        pthread_mutex_unlock(&terminateMutex);
        return;
    }
    pthread_mutex_unlock(&terminateMutex);
}
void join_threads(int numWorkerthreads,pthread_t * workers_threads){
    for(int i=0;i<numWorkerthreads;i++){
        pthread_join(workers_threads[i],NULL);//wait for all worker threads to finish
    }
    delete[] workers_threads;

}
