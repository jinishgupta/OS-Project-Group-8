#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>

// Format file permissions
static void format_perms(mode_t mode, char *buf)
{
    buf[0] = S_ISDIR(mode)  ? 'd' :
             S_ISLNK(mode)  ? 'l' : '-';
    buf[1] = (mode & S_IRUSR) ? 'r' : '-';
    buf[2] = (mode & S_IWUSR) ? 'w' : '-';
    buf[3] = (mode & S_IXUSR) ? 'x' : '-';
    buf[4] = (mode & S_IRGRP) ? 'r' : '-';
    buf[5] = (mode & S_IWGRP) ? 'w' : '-';
    buf[6] = (mode & S_IXGRP) ? 'x' : '-';
    buf[7] = (mode & S_IROTH) ? 'r' : '-';
    buf[8] = (mode & S_IWOTH) ? 'w' : '-';
    buf[9] = (mode & S_IXOTH) ? 'x' : '-';
    buf[10] = '\0';
}

// Human readable file size
static void human_size(off_t size, char *buf)
{
    const char *units[] = {"B", "K", "M", "G", "T"};
    double s = (double)size;
    int u = 0;
    while (s >= 1024.0 && u < 4) { s /= 1024.0; u++; }
    if (u == 0) snprintf(buf, 16, "%lld%s", (long long)size, units[u]);
    else         snprintf(buf, 16, "%.1f%s", s, units[u]);
}

// Print a single entry in long format
static void print_long(const char *path, const char *name, int human)
{
    struct stat st;
    char full[1024];
    snprintf(full, sizeof(full), "%s/%s", path, name);

    if (lstat(full, &st) < 0) {
        perror(full);
        return;
    }

    char perms[11];
    format_perms(st.st_mode, perms);

    struct passwd *pw = getpwuid(st.st_uid);
    struct group  *gr = getgrgid(st.st_gid);
    char uname[32], gname[32];
    snprintf(uname, sizeof(uname), "%s", pw ? pw->pw_name : "?");
    snprintf(gname, sizeof(gname), "%s", gr ? gr->gr_name : "?");

    char tmbuf[20];
    struct tm *tm = localtime(&st.st_mtime);
    strftime(tmbuf, sizeof(tmbuf), "%b %d %H:%M", tm);

    if (human) {
        char sz[16];
        human_size(st.st_size, sz);
        printf("%s %3lu %-8s %-8s %6s %s \033[%sm%s\033[0m\n",
               perms, (unsigned long)st.st_nlink,
               uname, gname, sz, tmbuf,
               S_ISDIR(st.st_mode) ? "1;34" : "0", name);
    } else {
        printf("%s %3lu %-8s %-8s %8lld %s \033[%sm%s\033[0m\n",
               perms, (unsigned long)st.st_nlink,
               uname, gname, (long long)st.st_size, tmbuf,
               S_ISDIR(st.st_mode) ? "1;34" : "0", name);
    }
}

int cmd_ls(int argc, char *argv[])
{
    int flag_l = 0, flag_a = 0, flag_h = 0;
    const char *dir = ".";

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            for (int j = 1; argv[i][j]; j++) {
                if (argv[i][j] == 'l') flag_l = 1;
                else if (argv[i][j] == 'a') flag_a = 1;
                else if (argv[i][j] == 'h') flag_h = 1;
                else {
                    fprintf(stderr, "custom_ls: unknown option -%c\n", argv[i][j]);
                    return 1;
                }
            }
        } else {
            dir = argv[i];
        }
    }

    DIR *dp = opendir(dir);
    if (!dp) { perror(dir); return 1; }

    struct dirent *ent;
    char *names[4096];
    int count = 0;

    while ((ent = readdir(dp)) != NULL) {
        if (!flag_a && ent->d_name[0] == '.') continue;
        names[count++] = strdup(ent->d_name);
        if (count >= 4096) break;
    }
    closedir(dp);

    for (int i = 0; i < count - 1; i++)
        for (int j = i + 1; j < count; j++)
            if (strcmp(names[i], names[j]) > 0) {
                char *tmp = names[i]; names[i] = names[j]; names[j] = tmp;
            }

    for (int i = 0; i < count; i++) {
        if (flag_l) {
            print_long(dir, names[i], flag_h);
        } else {
            char full[1024];
            snprintf(full, sizeof(full), "%s/%s", dir, names[i]);
            struct stat st;
            stat(full, &st);
            if (S_ISDIR(st.st_mode))
                printf("\033[1;34m%-20s\033[0m", names[i]);
            else
                printf("%-20s", names[i]);
            if ((i + 1) % 4 == 0) printf("\n");
        }
        free(names[i]);
    }
    if (!flag_l && count % 4 != 0) printf("\n");
    return 0;
}

// Standalone entry point
#ifndef SHELL_BUILD
int main(int argc, char *argv[])
{
    return cmd_ls(argc, argv);
}
#endif