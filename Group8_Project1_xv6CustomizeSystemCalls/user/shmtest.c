// shmtest.c — User-space test for the xv6 shared memory system calls
//
// Test flow:
//   1. Parent calls shmget() to create a shared region (key=42)
//   2. Parent calls shmat() to map it — writes a magic value
//   3. Parent forks a child
//   4. Child calls shmget(42) + shmat() — reads the magic value
//   5. Child detaches and exits
//   6. Parent verifies, detaches, exits

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define SHM_KEY  42
#define SHM_SIZE 4096
#define MAGIC    0xDEADBEEF

int
main(void)
{
    printf("shmtest: starting shared memory test\n");

    // Step 1 & 2: Parent creates and maps the shared region
    int id = shmget(SHM_KEY, SHM_SIZE);
    if(id < 0){
        printf("shmtest: shmget failed\n");
        exit(1);
    }
    printf("shmtest: shmget returned id=%d\n", id);

    int *addr = (int *)shmat(id);
    if(addr == 0){
        printf("shmtest: shmat failed\n");
        exit(1);
    }
    printf("shmtest: parent mapped shared region at %p\n", addr);

    // Parent writes magic value
    *addr = (int)MAGIC;
    printf("shmtest: parent wrote 0x%x to shared memory\n", *addr);

    // Step 3: Fork
    int pid = fork();
    if(pid < 0){
        printf("shmtest: fork failed\n");
        exit(1);
    }

    if(pid == 0){
        // === CHILD ===
        int cid = shmget(SHM_KEY, SHM_SIZE);
        if(cid < 0){
            printf("shmtest: child shmget failed\n");
            exit(1);
        }

        int *caddr = (int *)shmat(cid);
        if(caddr == 0){
            printf("shmtest: child shmat failed\n");
            exit(1);
        }

        printf("shmtest: child mapped shared region at %p\n", caddr);
        printf("shmtest: child read 0x%x from shared memory\n", *caddr);

        if((unsigned int)*caddr == MAGIC){
            printf("shmtest: PASS — child read the correct value written by parent\n");
        } else {
            printf("shmtest: FAIL — child read wrong value: 0x%x\n", *caddr);
        }

        shmdt(caddr);
        exit(0);
    } else {
        // === PARENT — wait for child ===
        int status;
        wait(&status);
        printf("shmtest: parent: child exited\n");

        shmdt(addr);
        printf("shmtest: parent detached shared region\n");
        printf("shmtest: done\n");
        exit(0);
    }
}
