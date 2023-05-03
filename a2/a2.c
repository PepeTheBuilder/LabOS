#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdlib.h>
#include "a2_helper.h"

void *t8(void *arg){
    int i = *(int*) arg;
    info(BEGIN,8,i);
    info(END,8,i); 
    return NULL;
}

void *t2(void *arg){
    int i = *(int*) arg;
    info(BEGIN,2,i);
    info(END,2,i); 
    return NULL;
}

void *p8(void *arg){
    info(BEGIN,8,0);

    pthread_t thread[5];
    for(int i=1; i<=4; i++){
        int* arg = malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&thread[i], NULL, t8, arg);
        if(i!=1){ 
            pthread_join(thread[i], NULL);
        }
        if(i==2) {
            pthread_join(thread[2], NULL);
            pthread_join(thread[1], NULL);
        }
       
    }

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
    waitpid(pid7, NULL, 0);

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
    waitpid(pid6, NULL, 0);
    pid8 = fork();
    if (pid8 == 0) {
        p8(NULL);
        exit(0);
    }
    waitpid(pid8, NULL, 0);

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
    waitpid(pid3, NULL, 0);

    pid_t pid4;

    pid4 = fork();
    if (pid4 == 0) {
        p4(NULL);
        exit(0);
    }
    waitpid(pid4, NULL, 0);

    // pthread_t thread[40];
    // for(int i=1; i<=40; i++){
    //     int* arg = malloc(sizeof(*arg));
    //     *arg = i;
    //     pthread_create(&thread[i], NULL, t2, arg);

    //     pthread_join(thread[i], NULL);
    //     if(i == 11){
    //         pthread_join(thread[11],NULL);
    //     }
    
    // }

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

    pthread_t root_thread;
    pthread_create(&root_thread, NULL, p1, NULL);
    pthread_join(root_thread, NULL);


    info(END, 1, 0);

    return 0;
}
