#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>

#define SEM_SIZE 4 //number of semaphores in the managed set
#define SHM_SIZE 4 //number of bytes in shared memory
#define WANTPIZZA 2 //Want pizza semaphore num


int main( void ) {
    //create section
    key_t semkey = ftok( "./manager", 0 ); //semaphore keyspace
    key_t shmkey = ftok( "./manager", 1 ); //shared memory keyspace
    int semid = semget( semkey, SEM_SIZE, IPC_CREAT | 0666 );
    if ( semid == -1 )
        return 1;
    int shmid = shmget( shmkey, SHM_SIZE, IPC_CREAT | 0666 );
    if ( shmid == -1 )
        return 1;
    //init section
    int j;
    
    //semaphore init
    int initarr[SEM_SIZE];
    for ( j = 0; j < SEM_SIZE; ++j )
        initarr[j] ^= initarr[j];
    semctl( semid, 1, SETALL, initarr );
    
    //the specific semaphore WANTPIZZA must be initialized with 1
    
    semctl( semid, WANTPIZZA, SETVAL, 1 );
    
    //shared memory init
    char* shmem = shmat( shmid, 0, 0 ); //mapping the shared memory to init
    for ( j = 0; j < SHM_SIZE; ++j )
        shmem[j] ^= shmem[j];
    shmdt( shmem );
    //pause section
    raise( SIGSTOP );
    //remove section
    semctl( semid, 1, IPC_RMID );
    shmctl( shmid, IPC_RMID, NULL );
    return 0;
}
