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
char con[]="HELLO$";
int sizecon = 7;
int varianta = 22794;
char buff[200]="";
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
        perror("ERROR\ncannot create the response pipe | cannot open the request pipe mkfifo\n");
    } 
    
    fdr = open(FILE_READ, O_RDONLY);
    if(fdr == -1) {
        perror("ERROR\ncannot create the response pipe | cannot open the request pipe fdr\n");
    }
    
    fdw = open(FILE_WRITE, O_WRONLY);   
    if(fdr == -1) {
        perror("ERROR\ncannot create the response pipe | cannot open the request pipe fdw\n");
    }


    if(write(fdw, &sizecon, 1) == 1) {
        if(write(fdw, &con, sizecon)==sizecon){
            printf("SUCCESS\n");
        }
        char aux=' ';
        int i =0;
        while(aux!='$'){
            read(fdr, &aux,1);
            buff[i]=aux;
            i++;
        }
        //if(strncmp(buff,"VARIANT$",9)==0){
        char text[]= "VARIANT$";
        char text2[]="VALUE$";
        char five='5';
        write(fdw, &text, 9);
        write(fdw, &five,1);
        write(fdw, &varianta,1);
        write(fdw, &text2, 7);
            //printf("%s\n",text);
        //}
        
    }
    
    
   
    // if(strstr(mesaj, "EXIT")) {		
    //         // munmap(&shared_mem, 2878785);
    // 	    // munmap(data, dim_fisier);
    //         // shm_unlink("/nJx8bhQ3");
    //         close(fd1);		           
    //         close(fdr);
    //         close(fdw);
    //         unlink(FILE_WRITE);
    //     }
    
    return 0;
}
