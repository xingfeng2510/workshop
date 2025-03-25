#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MEMORY_LIMIT_MB 256
#define CGROUP_PATH "/sys/fs/cgroup/memory/cgrp1"

#define BUFLEN 110

double getCPUFreq()
{
   FILE* sysinfo;
   char* ptr;
   char buf[BUFLEN];
   char key[] = "cpu MHz";
   int keylen = sizeof( key ) - 1;
   double freq = -1;

   sysinfo = fopen( "/proc/cpuinfo", "r" );
   if( sysinfo != NULL ) {
      while( fgets( buf, BUFLEN, sysinfo ) != NULL ) {
         if( !strncmp( buf, key, keylen ) ) {
            ptr = strstr( buf, ":" );
            freq = atof( ptr+1 ) * 1000000;
            break;
         }
      }
      fclose( sysinfo );
   }
   fprintf(stderr, "Freq = %f GHz\n", freq / 1000000000);
   return freq;
}

int getCPUCount()
{
	return sysconf(_SC_NPROCESSORS_CONF);
}

void Initialize()
{
    struct stat st = { 0 };
    char mlim_path[] = CGROUP_PATH "/memory.limit_in_bytes";
    char mlim_str[64], pid_str[64];
    char tasks_path[] = CGROUP_PATH "/tasks";
    int mlim_fd = -1, tasks_fd = -1;

    // Check if the cgroup already exists
    if (stat(CGROUP_PATH, &st) == 0) {
        // If the cgroup already exists, delete it first
        if (rmdir(CGROUP_PATH) == -1) {
            perror("Error deleting existing cgroup");
            exit(EXIT_FAILURE);
        }
    }

    // Create a new cgroup
    if (mkdir(CGROUP_PATH, S_IRUSR | S_IWUSR | S_IXUSR) == -1) {
        perror("Error creating cgroup");
        exit(EXIT_FAILURE);
    }

    // Set the cgroup's memory limit
    mlim_fd = open(mlim_path, O_WRONLY);
    if (mlim_fd == -1) {
        perror("Error opening memory limit file");
        exit(EXIT_FAILURE);
    }

    snprintf(mlim_str, sizeof(mlim_str), "%d", MEMORY_LIMIT_MB * 1024 * 1024);
    if (write(mlim_fd, mlim_str, sizeof(mlim_str)) == -1) {
        perror("Error setting memory limit");
        close(mlim_fd);
        return(EXIT_FAILURE);
    }
    close(mlim_fd);

    // Add the current process to the cgroup
    tasks_fd = open(tasks_path, O_WRONLY);
    if (tasks_fd == -1) {
        perror("Error opening tasks file");
        exit(EXIT_FAILURE);
    }

    snprintf(pid_str, sizeof(pid_str), "%d", getpid());
    if (write(tasks_fd, pid_str, sizeof(pid_str)) == -1) {
        perror("Error adding the current process to cgroup");
        close(tasks_fd);
        exit(EXIT_FAILURE);
    }
    close(tasks_fd);
}
