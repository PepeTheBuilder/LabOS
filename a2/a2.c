#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include "a2_helper.h"

void *p7(void *arg){
    info(BEGIN,7,0);

    info(END,7,0);    
    return NULL;
}

void *p6(void *arg){
    info(BEGIN,6,0);

    pthread_t thread7;
    pthread_create(&thread7, NULL, p7, NULL);
    pthread_join(thread7, NULL);

    info(END,6,0);    
    return NULL;
}

void *p8(void *arg){
    info(BEGIN,8,0);

    info(END,8,0);    

    return NULL;
}

void *p4 (void *arg){
    
    info(BEGIN,4,0);

    pthread_t thread6;
    pthread_create(&thread6, NULL, p6, NULL);
    pthread_join(thread6, NULL);
        
    pthread_t thread8;
    pthread_create(&thread8, NULL, p8, NULL);
    pthread_join(thread8, NULL);


    info(END,4,0);    
    return NULL;
}

void *p3 (void *arg){
    info(BEGIN,3,0);
    info(END,3,0);    
    return NULL;
}

void *p2(void *arg){

    info(BEGIN,2,0);

    pthread_t thread3;
    pthread_create(&thread3, NULL, p3, NULL);
    pthread_join(thread3, NULL);

    pthread_t thread4;
    pthread_create(&thread4, NULL, p4, NULL);
    pthread_join(thread4, NULL);

    info(END,2,0);
    return NULL;
}

void *p5(void *arg){
    info(BEGIN,5,0);

    info(END,5,0);
    return NULL;
}

void *p1(void *arg){

    pthread_t thread2;
    pthread_create(&thread2, NULL, p2, NULL);
    pthread_join(thread2, NULL);
    
    pthread_t thread5;
    pthread_create(&thread5, NULL, p5, NULL);
    pthread_join(thread5, NULL);

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
