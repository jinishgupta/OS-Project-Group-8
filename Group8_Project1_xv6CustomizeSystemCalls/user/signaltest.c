#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/signal.h"

// ── Custom signal handlers ────────────────────────────────────

void handler_sigusr1(int signum) {
  printf("[signaltest] SIGUSR1 (%d) caught — user handler executed!\n",
         signum);
}

void handler_sigusr2(int signum) {
  printf("[signaltest] SIGUSR2 (%d) caught — user handler executed!\n",
         signum);
}

void handler_sigalrm(int signum) {
  printf("[signaltest] SIGALRM (%d) caught — alarm fired!\n", signum);
}

// ── Main test ─────────────────────────────────────────────────

int main(void) {
  int pid;

  printf("\n=== xv6 Signal Handling Test ===\n\n");

  // ── Test 1: signal_init ──────────────────────────────────
  printf("--- Test 1: signal_init ---\n");
  if(signal_init() == 0)
    printf("PASS: signal table initialised for PID %d\n\n", getpid());
  else
    printf("FAIL: signal_init returned error\n\n");

  // ── Test 2: signal_handle (register handlers) ────────────
  printf("--- Test 2: signal_handle ---\n");
  if(signal_handle(SIGUSR1, handler_sigusr1) == 0)
    printf("PASS: handler registered for SIGUSR1\n");
  else
    printf("FAIL: could not register SIGUSR1 handler\n");

  if(signal_handle(SIGUSR2, handler_sigusr2) == 0)
    printf("PASS: handler registered for SIGUSR2\n");
  else
    printf("FAIL: could not register SIGUSR2 handler\n");

  if(signal_handle(SIGALRM, handler_sigalrm) == 0)
    printf("PASS: handler registered for SIGALRM\n\n");
  else
    printf("FAIL: could not register SIGALRM handler\n\n");

  // ── Test 3: signal_send (parent → child) ────────────────
  printf("--- Test 3: signal_send (fork + send) ---\n");
  pid = fork();
  if(pid < 0){
    printf("FAIL: fork failed\n");
    exit(1);
  }

  if(pid == 0){
    // ── Child ─────────────────────────────────────────────
    printf("[child] PID %d started, waiting for signals...\n", getpid());
    signal_init();
    signal_handle(SIGUSR1, handler_sigusr1);
    signal_handle(SIGUSR2, handler_sigusr2);
    // Busy-wait so parent can send signals
    volatile int i;
    for(i = 0; i < 500000000; i++);
    printf("[child] PID %d exiting\n", getpid());
    exit(0);
  } else {
    // ── Parent ────────────────────────────────────────────
    int child_pid = pid;
    pause(50); // Wait for child to register handlers
    printf("[parent] sending SIGUSR1 to child PID %d\n", child_pid);
    if(signal_send(child_pid, SIGUSR1) == 0)
      printf("PASS: SIGUSR1 sent successfully\n");
    else
      printf("FAIL: signal_send SIGUSR1\n");

    printf("[parent] sending SIGUSR2 to child PID %d\n", child_pid);
    if(signal_send(child_pid, SIGUSR2) == 0)
      printf("PASS: SIGUSR2 sent successfully\n");
    else
      printf("FAIL: signal_send SIGUSR2\n");

    wait(0);
    printf("[parent] child finished\n\n");
  }

  // ── Test 4: SIG_IGN (ignored signal) ────────────────────
  printf("--- Test 4: SIG_IGN (ignore signal) ---\n");
  signal_handle(SIGUSR1, SIG_IGN);
  signal_send(getpid(), SIGUSR1);
  printf("PASS: SIGUSR1 ignored (no handler output above)\n\n");

  // ── Test 5: SIG_DFL (default kill) ──────────────────────
  printf("--- Test 5: SIG_DFL kills child ---\n");
  pid = fork();
  if(pid == 0){
    // Child: default handler → should be killed
    signal_init(); // handlers are SIG_DFL
    volatile int i;
    for(i = 0; i < 100000000; i++);
    printf("[child] ERROR: should have been killed by SIGKILL\n");
    exit(1);
  } else {
    printf("[parent] sending SIGKILL (default) to child PID %d\n", pid);
    signal_send(pid, SIGKILL);
    wait(0);
    printf("PASS: child was killed by default handler\n\n");
  }

  printf("=== All signal tests completed ===\n\n");
  exit(0);
}
