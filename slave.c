/*Author: Shawn Anthony

Date: Friday October 14, 2022

This program is a slave to master.c

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
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//argc -> argument count
//argv -> argument vector
int main(int argc, char** argv){ //main function
  struct CLASS *ptr; //declare a struct CLASS pointer
  char* name = argv[1]; //Retrieve the name of the shared memory segment from the command line arguments
  printf("Hello! This is slave! My PID is: %d\n",getpid()); //introduces themselves
  printf("I am child number %s, received shared memory name %s\n",argv[0],name);
  pthread_mutex_lock(&mutex);
  int shm_fd = shm_open(name, O_RDWR, 0666); //initialize file descriptor
  ptr = mmap(0,sizeof(struct CLASS), PROT_WRITE, MAP_SHARED, shm_fd, 0); //assign the struct class pointer to the location in memory specified by mmap
  //int x = atoi(argv[0]); //Initialize x to the child number assigned from master via command line arguments
  ptr->response[ptr->index] = atoi(argv[0]); //write to the shared memory segment
  ptr->index+=1; //increment the shared memory segments index variable
  pthread_mutex_unlock(&mutex);
  printf("child number %s has written child number to shared memory\n",argv[0]); //Output status
  munmap(ptr,sizeof(struct CLASS)); //stop writing to the shared memory segment
  close(shm_fd); //close the file descriptor
  printf("child number %s closed access to shared memory and terminates\n",argv[0]);
  //free(argv);
  return 0; //return on main function
}//main function
