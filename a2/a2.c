#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdlib.h>
#include "a2_helper.h"

// void t2init(int index, pthread_t threadV){
//     pthread_create(&threadV, NULL, t2,(int*)&index);
//     // printf("thread nr %d\n",index);
// }


void *t8(void *arg){
    int i = *(int*) arg;
    info(BEGIN,8,i);
    if(i==1){
        pthread_t thread2 ;
        int* aux = malloc(sizeof(*arg));
        *aux=2;
        pthread_create(&thread2, NULL, t8, aux);
        pthread_join(thread2, NULL);
    }
    info(END,8,i); 
    return NULL;
}

void *p8(void *arg){
    info(BEGIN,8,0);

    pthread_t thread[3];
    int* aux = malloc(sizeof(*arg));
    *aux=1;
    pthread_create(&thread[0], NULL, t8, aux);
    pthread_join(thread[0], NULL);
    for(int i=3; i<5; i++){
        int* arg = malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&thread[i-2], NULL, t8, arg);
        pthread_join(thread[i-2], NULL);
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

void *t2(void *arg){
    int i = *(int*) arg;
    info(BEGIN,2,i);
    info(END,2,i); 
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
    
    for(int i=1; i<41; i++){

        int* aux = malloc(sizeof(*arg));
        *aux = i;
        pthread_create(&thread[i], NULL, t2, aux);
        printf("thread nr %d\n",i);
        if(i-1%4==0&&i>3){
            pthread_join(thread[i+3], NULL); 
            pthread_join(thread[i+2], NULL); 
            pthread_join(thread[i+1], NULL); 
            pthread_join(thread[i], NULL); 

        }

    }
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

    pthread_t root_thread;
    pthread_create(&root_thread, NULL, p1, NULL);
    pthread_join(root_thread, NULL);


    info(END, 1, 0);

    return 0;
}
