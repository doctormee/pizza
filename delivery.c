#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>


#define NFD 0 // need for delivery semaphore num
#define DELIVERED 1 // delivered semaphore num
#define PIZZA_COUNT 3 //pizza count semaphore num
//#define DELTIME 5 //delivery time
//#define N 7 // no of pizza slices

int up( int semid, int x ) {
    struct sembuf oper;
    oper.sem_num = x;
    oper.sem_op = 1;
    oper.sem_flg = 0;
    return semop( semid, &oper, 1 );
} 
int down( int semid, int x ) {
    struct sembuf oper;
    oper.sem_num = x;
    oper.sem_op = -1;
    oper.sem_flg = 0;
    return semop( semid, &oper, 1 );
} 

int main( int argc, char* argv[] ) {
    if( argc != 3 ) {
        printf("Invalid parameters. Usage: %s time n - creates a pizza delivery, which delivers in time seconds and cuts pizza into n slices\n", argv[0]);
        return 1;
    }
    
    //init DELTIME and N
    int DELTIME = atoi( argv[1] );
    int N = atoi( argv[2] );
    //get semaphore id
    key_t semkey = ftok( "./manager", 0 );
    int semid = semget( semkey, 2, 0 );
    if ( semid == -1 )
        return 1;
    //main cycle
    // signal( SIGINT, SIG_IGN ); //ignoring the SIGINT to protect semaphors  from incorrect change
    while(1) {
        if ( down( semid, NFD ) == -1 ) 
            return 1;
        printf( "Delivery in %d second", DELTIME );
        if( DELTIME > 1 )
            printf( "s" );
        printf( "...\n" );
        sleep( DELTIME );
        semctl( semid, PIZZA_COUNT, SETVAL, N );
        printf( "Successful delivery, pizza cut in %d slice", N );
        if ( N > 1 )
            printf( "s" );
        printf( "\n" );
        if( up( semid, DELIVERED ) == -1 )
            return 1;
    }
    return 0;
}