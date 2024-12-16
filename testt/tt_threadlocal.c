#include <stdio.h>
#include <pthread.h>

__thread int thread_local_var = 0; // 线程局部变量
//thread_local int thread_local_var = 0;


void* thread_function(void* arg) {
    printf("Thread local var: %d\n", thread_local_var);
    thread_local_var = *(int*)arg;
    printf("Thread local var after assignment: %d\n", thread_local_var);
    return NULL;
}

int main() {
    pthread_t thread;
    int value = 10;
    pthread_create(&thread, NULL, thread_function, &value);
    pthread_join(thread, NULL);
    printf("Main thread local var: %d\n", thread_local_var);
    return 0;
}
