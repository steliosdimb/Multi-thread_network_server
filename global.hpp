#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <pthread.h>
using namespace std;
extern string txt1;
extern string txt2;
extern unordered_map<string, int> party_votes; //map that will keep party votes
extern vector<string> voter_names;//vector that will keep voter names to ensure no voter will vote twice or more
extern queue<int> buffer; //buffer will be a queue to store fds from each new connection
extern bool terminateServer; //boolean to check if a signal was given
extern pthread_mutex_t terminateMutex; //mutex for the signal
extern pthread_mutex_t fmtx;
extern pthread_mutex_t bmtx;
extern pthread_mutex_t vmtx;
extern pthread_mutex_t pmtx;
extern pthread_cond_t buffer_nonempty;
extern pthread_cond_t buffer_nonfull;
extern pthread_mutex_t sfmtx;