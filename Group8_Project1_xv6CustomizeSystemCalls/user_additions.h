// Add these declarations to xv6's existing user/user.h

// Struct for getprocinfo syscall
struct procinfo {
    int  pid;
    int  ppid;
    int  state;
    char name[16];
};

// New syscall prototypes
int  getprocinfo(struct procinfo *);