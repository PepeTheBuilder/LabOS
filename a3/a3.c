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
#define VARIANT "VARIANT$"
#define VALUE "VALUE$"
#define HELLO "HELLO$"

int varianta = 22794;
char aux;
char msg[200] = "";
int dimensionMsg = 0;
int i = 0;

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

    if (write(fdw, HELLO, 7) == 7)
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
            printf("%c",buffer[m]);
            if (buffer[m] == '$')
            {
                break;
            }
            m++;
        }
        buffer[m]='\0';
        // printf("Am citit:%s",buffer);

        if (strstr(msg, "VARIANT$"))
        {
            write(fdw, VARIANT, 9);
            write(fdw, &varianta, sizeof(varianta));
            write(fdw, VALUE, 7);
        }
        if (strstr(msg, "EXIT$"))
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
