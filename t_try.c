#include <inttypes.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>

#define N 10

static void *run(void *arg) {
    uintptr_t job = (uintptr_t)arg;
    printf("Job %" PRIuPTR "\n", job);
    return NULL;
}

int main(int argc, char *argv[]) {
   pthread_t threads[N];
   for (uintptr_t i=0; i<N; ++i) {
       pthread_create(threads+i, NULL, run, (void*)i);
   }

   for (uintptr_t i=0; i<N; ++i) {
       pthread_join(threads[i], NULL);
   }

   return 0;
}