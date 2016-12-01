#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>

#define SEM_SIZE 4 //number of semaphores in the managed set
#define SHM_SIZE 4 //number of bytes in shared memory
#define NFD 0 // need for delivery semaphore num
#define DELIVERED 1 // delivered semaphore num
#define WANTPIZZA 2 //Want pizza semaphore num
#define PIZZA_COUNT 3 //pizza count semaphore num

//#define WORKTIME 1 //time of sleep and study in seconds

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

void eat_study( int WORKTIME ) {
    printf( "Studying for %d second", WORKTIME );
    if( WORKTIME > 1 )
        printf( "s" );
    printf( "...\n" );
    sleep( WORKTIME );
    printf( "Been studying for %d second", WORKTIME );
    if( WORKTIME > 1 )
        printf( "s" );
    printf( "...\n" );
}


int main( int argc, char* argv[] ) {
    
    if ( argc != 2 ) {
        printf("Invalid parameters. Usage: %s time  - creates a student, who studies for time seconds\n", argv[0]);
        return 1;
    }
    //init WORKTIME
    int WORKTIME = atoi( argv[1] );
    
    //get semaphore id
    key_t semkey = ftok( "./manager", 0 ); //semaphore keyspace
    int semid = semget( semkey, SEM_SIZE, 0 );
    if ( semid == -1 )
        return 1;
    //get shared memory id
    key_t shmkey = ftok( "./manager", 1 ); //shared memory keyspace
    int shmid = shmget( shmkey, SHM_SIZE, 0 );
    if ( shmid == -1 )
        return 1;
    //attaching shared memory 
    //NB! This process assumes that SHM_SIZE >= 4
    int* shmem;
    shmem = shmat( shmid, 0, 0 ); 
    
    //macroses for readability is defined here
#define still_eating ( *shmem )
   // signal( SIGINT, SIG_IGN ); //ignoring the SIGINT to protect semaphors and shared variable from incorrect change
    //main cycle
    while(1) {
        //trying to take the pizza
        if ( down( semid, WANTPIZZA ) == -1 )
            return 1;
        //at the table
        if ( ( still_eating == 0 ) && ( semctl( semid, PIZZA_COUNT, GETVAL ) == 0 ) ) { // if there is no pizza left and no one is chewing
            printf("Requesting pizza\n");
            if ( up( semid, NFD ) == -1 )
                return 1;
            if ( down( semid, DELIVERED ) == -1) 
                return 1;
        }
        if ( up( semid, WANTPIZZA ) == -1 )
            return 1;
        if ( down( semid, PIZZA_COUNT ) == -1 )
            return 1;
        //now we're eating 
        ++still_eating;
        eat_study( WORKTIME );
        --still_eating;
    }
    return 0;
}
