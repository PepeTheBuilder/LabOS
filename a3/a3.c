#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

char FILE_WRITE[] = "RESP_PIPE_22794";
char FILE_READ[] =  "REQ_PIPE_22794";
char con[]="HELLO\0";
int sizecon = 7;
int nr_meu = 22794;
char mesaj[200]="";
int dim_cuv = 0;
int shared_mem;
char *sharedMem = NULL;
//2.6
int fd1=-1;
char *data = NULL;
off_t dim_fisier;

int main(int argc, char **argv){ 

    int fdr, fdw; 
    if(mkfifo(FILE_WRITE, 0777) != 0) {
        perror("ERROR\ncannot create/open pipe mkfifo\n");
    } 
    
    fdr = open(FILE_READ, O_RDONLY);
    if(fdr == -1) {
        perror("ERROR\ncannot create/open pipe fdr\n");
    }
    
    fdw = open(FILE_WRITE, O_WRONLY);   
    if(fdr == -1) {
        perror("ERROR\ncannot create/open pipe fdw\n");
    }


    if(write(fdw, &sizecon, 1) == 1) {
        if(write(fdw, &con, sizecon)==sizecon){
            printf("SUCCESS\n");
        }
    }
    
    
   
    if(strstr(mesaj, "EXIT")) {		
            munmap(&shared_mem, 2878785);
    	    munmap(data, dim_fisier);
            shm_unlink("/dURY42tR");
            close(fd1);		           
            close(fdr);
            close(fdw);
            unlink(FILE_WRITE);
        }
    
    return 0;
}