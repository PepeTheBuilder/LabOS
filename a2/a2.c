#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <fcntl.h>
#include "a2_helper.h"

sem_t sem,thread11,*t6_complete,*t8_end;
pthread_t tredu;
int ok=0,end8=0,end6=0;

void P(sem_t *sem)
{
    sem_wait(sem);
}

void V(sem_t *sem)
{
    sem_post(sem);
}

void *t2(void *arg){
    int i = *(int*) arg;
    P(&sem);
    info(BEGIN,2,i);
    if(i==1||i==2||i==3||i==11){
        V(&thread11);
        while(ok==0&&i!=11){
            
        }
        P(&thread11);
    }
    info(END,2,i);
    V(&sem);
    pthread_exit(NULL);
}

void *t6(void *arg) {
    int i = *(int*) arg;
    if (i == 2) {
        printf("astept pt end8\n");
        P(t8_end);
        printf("GATA end8 \n");
    }
    info(BEGIN, 6, i);

    info(END, 6, i);
    if (i == 5) {
        end6 = 1;
        printf("end6 = 1\n");
        V(t6_complete); // signal completion of p6 thread[6]
    }
    pthread_exit(NULL);
}

void *t8(void *arg) {
    int i = *(int*) arg;
    if (i == 3) {
        printf("astept pt end6\n");
        P(t6_complete); // wait for completion of p6 thread[6]
        printf("GATA end6 \n");
    }
    if(i==2){
        
    }
    info(BEGIN, 8, i);
    if (i == 1) {
        pthread_t thread2;
        int* aux = malloc(sizeof(*arg));
        *aux = 2;
        pthread_create(&thread2, NULL, t8, aux);
        pthread_join(thread2, NULL);
        info(END, 8, 1);
        pthread_exit(NULL);
    }else
    info(END, 8, i);
    if (i == 3) {
        V(t8_end);
        printf("end8=1\n");
    }
    pthread_exit(NULL);
}


void *p8(void *arg){
    info(BEGIN,8,0);

    pthread_t thread[4];
    int* aux = malloc(sizeof(*arg));
    *aux=1;
    printf("Incep tredu 1\n\n");
    pthread_create(&thread[0], NULL, t8, aux);
    for(int i=3; i<5; i++){
        int* arg = malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&thread[i-2], NULL, t8, arg);
    }

    // wait for all threads in p8 to complete
    pthread_join(thread[1], NULL);
    pthread_join(thread[2], NULL);
    pthread_join(thread[0], NULL);
    

    info(END,8,0);    

    return NULL;
}

void *p7(void *arg){
    info(BEGIN,7,0);

    info(END,7,0);    
    return NULL;
}

void *p6(void *arg){
    info(BEGIN,6,0);

    pid_t pid7;

    pid7 = fork();
    if (pid7 == 0) {
        p7(NULL);
        exit(0);
    }
    pthread_t thread[6];
    for(int i=1; i<6; i++){

        int* aux = malloc(sizeof(*arg));
        *aux = i;
        pthread_create(&thread[i], NULL, t6, aux);
    }
    for(int i=1; i<6; i+=4){ 
        if(i==3)continue; 
        pthread_join(thread[i],NULL);
        
    }
    tredu=thread[3];

    waitpid(pid7, NULL, 0);
    
    // wait for threads in p8 to complete
    pthread_join(tredu, NULL);
    
    info(END,6,0);    
    return NULL;
}

void *p5(void *arg){
    info(BEGIN,5,0);

    info(END,5,0);
    return NULL;
}

void *p4(void *arg){
    info(BEGIN, 4, 0);

    pid_t pid6, pid8;
    pid6 = fork();
    if (pid6 == 0) {
        p6(NULL);
        exit(0);        
    }
    pid8 = fork();
    if (pid8 == 0) {
        p8(NULL);
        exit(0);

    }
    
    waitpid(pid8, NULL, 0);printf("Ba sa facut procesul 8\n\n");
    waitpid(pid6, NULL, 0);printf("Ba sa facut procesul 6\n\n");

    info(END, 4, 0);
    return NULL;
}

void *p3(void *arg){
    info(BEGIN,3,0);
    info(END,3,0);    
    return NULL;
}

void *p2(void *arg){

    info(BEGIN,2,0);


    pid_t pid3;

    pid3 = fork();
    if (pid3 == 0) {
        p3(NULL);
        exit(0);
    }
    pthread_t thread[43];
    
    sem_init(&sem, 0, 4); 
    sem_init(&thread11,0,0);
    
    for(int i=1; i<41; i++){

        int* aux = malloc(sizeof(*arg));
        *aux = i;
        pthread_create(&thread[i], NULL, t2, aux);
    }

    for(int i=1; i<41; i+=4){  
        if(i==1) pthread_join(thread[4],NULL);  
        else{  
        if(i+2==11){
            int value=1;
            while(value==4)
            {
                sem_getvalue(&thread11,&value);
            }
        pthread_join(thread[11],NULL);
            ok=1;
        pthread_join(thread[1],NULL);
        pthread_join(thread[2],NULL);
        pthread_join(thread[3],NULL);

        }
        else{
            pthread_join(thread[i+2],NULL);
        }
        pthread_join(thread[i+3],NULL);
        pthread_join(thread[i+1],NULL);
        pthread_join(thread[i],NULL);
        }
        
    }

    for(int i=1; i<41; i++)
        sem_destroy(&sem);
    waitpid(pid3, NULL, 0);

    pid_t pid4;

    pid4 = fork();
    if (pid4 == 0) {
        p4(NULL);
        exit(0);
    }
    waitpid(pid4, NULL, 0);
    
    info(END,2,0);
    return NULL;
}

void *p1(void *arg){

    pid_t pid2;

    pid2 = fork();
    if (pid2 == 0) {
        p2(NULL);
        exit(0);
    }
    waitpid(pid2, NULL, 0);

    
    pid_t pid5;

    pid5 = fork();
    if (pid5 == 0) {
        p5(NULL);
        exit(0);
    }
    waitpid(pid5, NULL, 0);

    return NULL;
}

int main(){
    init();

    info(BEGIN, 1, 0);
    t6_complete=sem_open("t6.3_done",O_CREAT,0644, 0);
    t8_end=sem_open("p8.3_done",O_CREAT,0644, 0);
    

    pthread_t root_thread;
    pthread_create(&root_thread, NULL, p1, NULL);
    pthread_join(root_thread, NULL);

    info(END, 1, 0);

    sem_close(t6_complete);
    sem_close(t8_end);
    unlink("/dev/shm/t6.3_done");
    unlink("/dev/shm/p8.3_done");
    return 0;
}
