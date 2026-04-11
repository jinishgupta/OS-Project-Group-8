#ifndef SIGNAL_H
#define SIGNAL_H

// Signal numbers
#define SIGKILL   1   // Kill the process immediately
#define SIGSTOP   2   // Stop/pause the process
#define SIGCONT   3   // Continue a stopped process
#define SIGALRM   4   // Alarm/timer signal
#define SIGUSR1   5   // User-defined signal 1
#define SIGUSR2   6   // User-defined signal 2
#define NSIG      7   // Total number of signals

// Signal handler type
typedef void (*sighandler_t)(int);

// Special handler values
#define SIG_DFL  ((sighandler_t)0)   // Default handler
#define SIG_IGN  ((sighandler_t)1)   // Ignore signal

#endif // SIGNAL_H
