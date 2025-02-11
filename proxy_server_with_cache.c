#include "proxy_parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_BYTES 4096    //max allowed size of request/response
#define MAX_CLIENTS 400     //max number of client requests served at a time
#define MAX_SIZE 200*(1<<20)     //size of the cache
#define MAX_ELEMENT_SIZE 10*(1<<20)     //max size of an element in cache
typedef struct cache_element cache_element;

struct cache_element{
    char* data;        
    int len;          
    char* url;        
	time_t lru_time_track;
    cache_element* next;  
};

cache_element* find(char* url);
int add_cache_element(char* data,int size,char* url);
void remove_cache_element();

int port_number = 8080;				
int proxy_socketId;					
pthread_t tid[MAX_CLIENTS];         
sem_t seamaphore;	                
                                    
//sem_t cache_lock;			       
pthread_mutex_t lock;               


cache_element* head;                
int cache_size;             

int main(int argc,char * argv[])
{
    int client_socket_Id,client_len;
    struct sockaddr server_addr,client_addr;
    sem_init(&semaphore,0,MAX_CLIENTS);

    pthread_mutex_init(&lock,NULL);

//port number according to user 
//jo bhi command prompt pr dalenge wo as a string aayega to usko int me convert karna pdega

    if(argv==2)
    {
        port_number=atoi(argv[1]);
    }
    else{
        printf("Too few arguments");
        //exit system call exits from complete program
        exit(1);
    }
    printf("Setting Proxy Server Port : %d\n",port_number);
    proxy_socketId=socket(AF_INET,SOCK_STREAM,0);
    if(proxy_socketId<0)
    {
        perror("Failed to create a socket\n");
        exit(1);
    }
    int reuse=1;
    if (setsockopt(proxy_socketId, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0) 
        perror("setsockopt(SO_REUSEADDR) failed\n");
    
    bzero((char*)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_num); }
    server_addr.sin_addr.s_addr= INADDR_ANY;