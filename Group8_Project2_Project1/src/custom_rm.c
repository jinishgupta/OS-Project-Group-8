// custom_rm.c — A custom implementation of the rm command
//
// Supports:
//   custom_rm <file>       Remove a file
//   custom_rm -r <dir>     Recursively remove a directory tree
//   custom_rm -f <path>    Force: ignore errors for non-existent paths
//   custom_rm -rf <path>   Recursive + force (combined)
//
// Pattern follows other custom_*.c files in this project.

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

// Forward declarations
static int rm_path(const char *path, int recursive, int force);
static int rm_recursive(const char *path, int force);

// Remove a single file or symlink
static int rm_file(const char *path, int force)
{
    if(unlink(path) < 0){
        if(force && errno == ENOENT)
            return 0;  // non-existent + force = OK
        fprintf(stderr, "custom_rm: cannot remove '%s': %s\n",
                path, strerror(errno));
        return 1;
    }
    return 0;
}

// Recursively remove directory tree rooted at 'path'
static int rm_recursive(const char *path, int force)
{
    DIR *dp = opendir(path);
    if(!dp){
        // Could be a file (not a dir) — try unlink
        if(errno == ENOTDIR)
            return rm_file(path, force);
        if(force && errno == ENOENT)
            return 0;
        fprintf(stderr, "custom_rm: cannot open directory '%s': %s\n",
                path, strerror(errno));
        return 1;
    }

    struct dirent *ent;
    int ret = 0;
    char child[4096];

    while((ent = readdir(dp)) != NULL){
        // Skip . and ..
        if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;

        snprintf(child, sizeof(child), "%s/%s", path, ent->d_name);

        struct stat st;
        if(lstat(child, &st) < 0){
            fprintf(stderr, "custom_rm: cannot stat '%s': %s\n",
                    child, strerror(errno));
            ret = 1;
            continue;
        }

        if(S_ISDIR(st.st_mode)){
            // Recurse into sub-directory
            if(rm_recursive(child, force) != 0)
                ret = 1;
        } else {
            // Remove file / symlink / special file
            if(unlink(child) < 0){
                fprintf(stderr, "custom_rm: cannot remove '%s': %s\n",
                        child, strerror(errno));
                ret = 1;
            }
        }
    }
    closedir(dp);

    // Now remove the (now-empty) directory itself
    if(rmdir(path) < 0){
        fprintf(stderr, "custom_rm: cannot remove directory '%s': %s\n",
                path, strerror(errno));
        ret = 1;
    }
    return ret;
}

// Dispatch: remove a single path, honouring -r and -f flags
static int rm_path(const char *path, int recursive, int force)
{
    struct stat st;
    if(lstat(path, &st) < 0){
        if(force && errno == ENOENT)
            return 0;
        fprintf(stderr, "custom_rm: cannot stat '%s': %s\n",
                path, strerror(errno));
        return 1;
    }

    if(S_ISDIR(st.st_mode)){
        if(!recursive){
            fprintf(stderr, "custom_rm: '%s' is a directory (use -r to remove)\n", path);
            return 1;
        }
        return rm_recursive(path, force);
    }
    return rm_file(path, force);
}

// Public entry point — called by shell.c
int cmd_rm(int argc, char *argv[])
{
    int recursive = 0;
    int force     = 0;
    int file_start = 1;
    int ret = 0;

    // Parse flags
    for(int i = 1; i < argc; i++){
        if(argv[i][0] == '-'){
            file_start = i + 1;
            for(int j = 1; argv[i][j]; j++){
                char c = argv[i][j];
                if(c == 'r' || c == 'R') recursive = 1;
                else if(c == 'f')        force     = 1;
                else {
                    fprintf(stderr, "custom_rm: unknown option '-%c'\n", c);
                    fprintf(stderr, "usage: custom_rm [-r] [-f] <path>...\n");
                    return 1;
                }
            }
        } else {
            // First non-flag argument — all remaining are paths
            file_start = i;
            break;
        }
    }

    if(file_start >= argc){
        if(force)
            return 0;   // rm -f with no targets is OK
        fprintf(stderr, "usage: custom_rm [-r] [-f] <path>...\n");
        return 1;
    }

    for(int i = file_start; i < argc; i++){
        if(rm_path(argv[i], recursive, force) != 0)
            ret = 1;
    }
    return ret;
}

// Standalone entry point (suppressed when built into the shell)
#ifndef SHELL_BUILD
int main(int argc, char *argv[])
{
    return cmd_rm(argc, argv);
}
#endif
