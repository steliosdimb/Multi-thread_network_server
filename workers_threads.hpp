#include <pthread.h>
#include <queue>
#include <string>
#include <vector>
using namespace std;
pthread_t* create_workers(int);
void * worker(void *);
void accept_connection(int,int,int);
void join_threads(int,pthread_t*);
