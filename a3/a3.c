#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FILE_WRITE "RESP_PIPE_22794"
#define FILE_READ "REQ_PIPE_22794"
#define TOTUNSTIRNG "VARIANT$22794VALUE$"
#define VARIANT "VARIANT$"
#define VALUE "VALUE$"
#define HELLO "HELLO$"
int varianta = 22794;

int main(int argc, char **argv)
{

    int fdr, fdw;
    if (mkfifo(FILE_WRITE, 0777) != 0)
    {
        perror("ERROR\ncannot create the response pipe | cannot open the request pipe mkfifo\n");
        return 1;
    }

    fdr = open(FILE_READ, O_RDONLY);
    if (fdr == -1)
    {
        perror("ERROR\ncannot create the response pipe | cannot open the request pipe fdr\n");
        return 1;
    }
    else
    {
        // printf("Am deschis fdr");
    }

    fdw = open(FILE_WRITE, O_WRONLY);
    if (fdw == -1)
    {
        perror("ERROR\ncannot create the response pipe | cannot open the request pipe fdw\n");
        return 1;
    }

    if (write(fdw, HELLO, 6) == 6)
    {
        printf("SUCCESS\n");
    }
    else
    {
        printf("NUUUUUUUUU\n");
    }


    while (1)
    {
        char buffer[250];
        int bytesRead;
        int m=0;
        while ((bytesRead = read(fdr, &buffer[m], 1)) > 0)
        {
            // printf("%c",buffer[m]);
            if (buffer[m] == '$')
            {
                break;
            }
            m++;
        }
        buffer[m]='\0';

        if (strncmp(buffer, "VARIANT",8)==0)
        {
            write(fdw, "VARIANT$", 8);  
            write(fdw, &varianta, 4);
            write(fdw, "VALUE$", 6);
        }

        if (strstr(buffer, "EXIT"))
        {
            close(fdr);
            close(fdw);
            unlink(FILE_WRITE);
            break;
        }
        break;
    }

    close(fdr);
    close(fdw);
    unlink(FILE_WRITE);

    return 0;
}
