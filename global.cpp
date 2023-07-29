#include "global.hpp"
#include <string>
#include <unordered_map>
#include <queue>
#include <pthread.h>
using namespace std;
string txt1="";
string txt2="";
pthread_mutex_t terminateMutex = PTHREAD_MUTEX_INITIALIZER; //mutex for the signal
bool terminateServer=false;
queue<int> buffer; // Initialize empty queue
unordered_map<string, int> party_votes; // Initialize empty unordered map
vector<string> voter_names={};//vector that will keep voter names to ensure no voter will vote twice or more
pthread_mutex_t fmtx= PTHREAD_MUTEX_INITIALIZER;;//file mutex
pthread_mutex_t sfmtx= PTHREAD_MUTEX_INITIALIZER;;//stats file mutex
pthread_mutex_t bmtx = PTHREAD_MUTEX_INITIALIZER;//buffer mutex
pthread_mutex_t vmtx = PTHREAD_MUTEX_INITIALIZER;//vector voters mutex
pthread_mutex_t pmtx= PTHREAD_MUTEX_INITIALIZER;;//map party mutex
pthread_cond_t buffer_nonempty = PTHREAD_COND_INITIALIZER; //condition variable for nonempty buffer in worker
pthread_cond_t buffer_nonfull = PTHREAD_COND_INITIALIZER;   //condition variable for nonfull buffer in master thread
