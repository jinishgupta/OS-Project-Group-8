#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  printf("semaphore test\n");

  // Create a semaphore initialized to 0 (child must block until parent posts)
  int id = semget(42, 0);
  if(id < 0){
    printf("semget failed\n");
    exit(1);
  }
  printf("semaphore created: id=%d\n", id);

  int pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    semclose(id);
    exit(1);
  }

  if(pid == 0){
    // Child: wait on semaphore (will block until parent posts)
    printf("child: waiting on semaphore...\n");
    if(semwait(id) < 0){
      printf("child: semwait failed\n");
      exit(1);
    }
    printf("child: acquired semaphore, proceeding\n");
    exit(0);
  } else {
    // Parent: do some work, then release the child
    printf("parent: doing work...\n");
    // Simulate work with a small delay using pause
    pause(5);
    printf("parent: posting semaphore\n");
    if(sempost(id) < 0){
      printf("parent: sempost failed\n");
      semclose(id);
      exit(1);
    }

    // Wait for child to finish
    int status;
    wait(&status);
    printf("parent: child exited\n");

    // Destroy semaphore
    if(semclose(id) < 0){
      printf("semclose failed\n");
      exit(1);
    }
    printf("semaphore test passed\n");
    exit(0);
  }
}
