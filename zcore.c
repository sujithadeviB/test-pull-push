
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "zlib.h"
#include <string.h>
#include <time.h>

#ifndef DATE_FMT
#define DATE_FMT        "%Y-%m-%d.%H-%M-%S"
#endif

// ulimit -c unlimited
// echo "|/usr/bin/zcore %e %p" > /proc/sys/kernel/core_pattern

int main(int argc, char *argv[])
{
        ssize_t nread;
        char buf[1024];
        gzFile file;
        char filename[500];
        time_t timeStamp = time(NULL);
        struct tm *tm = localtime(&timeStamp);
        char date_buf[500];
        char *executable;
        char *pid_str;

        if (argc != 3) {
                fprintf(stderr, "Usage: %s <executable> <pid>\n", argv[0]);
                return 1;
        }
        // Remove all files except the last 10 instances
        system(CLEAN_PROG);

        executable = argv[1];
        pid_str = argv[2];
        strftime(date_buf, sizeof(date_buf), DATE_FMT, tm);
        snprintf(filename, 500, CORE_DIR "/%s.%s.%s.core.gz", executable, date_buf, pid_str);
        file = gzopen(filename, "wb");
        if (file == NULL) {
                fprintf(stderr, "gzopen error\n");
                exit(1);
        }

        while ((nread = read(STDIN_FILENO, buf, 1024)) > 0) {
                if( gzwrite(file, buf, nread) == 0 )
                        exit( 1 );
        }

        gzclose(file);

        FILE* conf_file = fopen("/etc/harmonic/nsg/cfg/zcore.conf", "r");

        if ( conf_file != NULL )
        {
                char line [ 256 ];
                while ( fgets ( line, sizeof line, conf_file ) != NULL ) // Read a line
                {
                        system( line );
                }
                fclose ( conf_file );
        }

        exit(0);
}
