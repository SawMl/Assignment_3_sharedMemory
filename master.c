/*Author: Shawn Anthony

Compile Line: gcc master.c -o master -lrt

Date: Friday October 14, 2022

This program is master.c

Prompt:

make use of the POSIX implementation of the Linux shared memory mechanism, and use the fork() and exec() system calls to
create child processes and control the program that a child process is to execute.

When master executes, it should first output a message to identify itself.

It should then request to create a shared memory segment of a certain name xxxxx,
followed by creating n child processes, where both xxxxx and the number n are obtained from the commandline parameters.

Each child process is to execute slave, with its child number (i.e., 1, 2, etc.) and the shared memory segment name xxxxx
passed to it from the exec() system call.

The master process should output the number of slaves it has created, and wait for
all of them to terminate.

Upon receiving termination signals from all child processes, master then outputs
the content of the shared memory segment filled in by the slaves, removes the shared memory
*/
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <errno.h>
#include "myShm.h"
#include <pthread.h>
pthread_mutex_t lock;
//argc = argument count
// argv = argument vector
int main(int argc, char** argv){//enter main function
  if (pthread_mutex_init(&lock, NULL)!=0){
    printf("mutex init failure\n");
    return 1;
  }
  int p_id = getpid(); //set the parent id to p_id
  struct CLASS *shm_base; //initialize data construct
  printf("Hello. It's me, the master program. My process ID is: %d\n",p_id);//First requirement. Prints out the process ID of the master
  char* name = argv[1]; //xxxxx name of shared memory segment
  int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666); /* file descriptor, from shm_open()*/
  if (shm_fd == -1) {//check for error
      //printf(“prod: Shared memory failed: %s\n”, strerror(errno)); //inform the user of an error
      exit(1); //exit the program
  }//end check for error
  ftruncate(shm_fd,sizeof(struct CLASS));//configure the size of the shared memory segment
  shm_base = mmap(0,sizeof(struct CLASS), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); //map shared memory segment in the address space of the processes
  if (shm_base == MAP_FAILED){ //check for map failure
    printf("prod: Map failed: %s\n", strerror(errno));//inform the user of error
    exit(1); //end the program
  } //end check for map failure
  shm_base->index=0; //Initialize data construct by setting the initial index to 0
  //all children will write to response[index] before incrementing index
  munmap(shm_base,sizeof(struct CLASS));//close the map
  close(shm_fd);//close the file descriptor
  int n = atoi(argv[2]); //number of child processes to be created
  for (int i=0;i<n;i++){ //main loop for child process creation
      fork(); //duplicate process
      if (getpid()!=p_id){ //if its the child
        char num[10]; //var initialization
        sprintf(num,"%d",(i+1));//convert int into character array
        char** arg = malloc(sizeof(num)+sizeof(name)+sizeof(NULL)); //create space for commandline arguments
        arg[0]=num; //assign child number
        arg[1]=name; //assign shared memory segment name
        arg[2]=NULL; //end argument vector in NULL
        execvp("./slave",arg); //exec .slave
      } //end if its the child
  } //end main loop
  pid_t wpid; //variable used for master to wait for all slaves to finish execution
  int status = 0; //variable used for master to wait for all slaves to finish execution
  while ((wpid = wait (&status)) > 0);// wait for all child processes
  printf("Hello again! It's me, the Master! I have created %d slaves and they have all terminated\n",n); //Requirement output statement
  shm_fd = shm_open(name, O_RDONLY, 0666); //Reaccess the shared memory file descriptor
  shm_base = mmap(0,sizeof(struct CLASS),PROT_READ,MAP_SHARED, shm_fd, 0); //Read the data
  int k = shm_base->index; //for loop bound to iterate through response array
  printf("Content of shared memory segment filled by child processes:\n");
  for (int j=0;j<k;j++){ //iterate through response array
    printf("%d\n",shm_base->response[j]); //print the response[j]
  }
  munmap(shm_base,sizeof(struct CLASS)); //stop reading
  close(shm_fd); //close file descriptor
  printf("Master removed shared memory segment, and is exiting\n");
  return 0;//return on main function
}//exit main function
