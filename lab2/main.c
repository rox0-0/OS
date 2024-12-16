#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

typedef struct {
    int **arrays;
    int *result;
    int num_arrays;
    int array_length;
    int start;
    int end;
    bool row_split;
    pthread_mutex_t *mutex;
} thread_args_t;

void *process_arrays(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;

    if (args->row_split) {
        
        for (int i = args->start; i < args->end; ++i) {
            for (int j = 0; j < args->array_length; ++j) {
                pthread_mutex_lock(args->mutex);
                args->result[j] += args->arrays[i][j];
                pthread_mutex_unlock(args->mutex);
            }
        }
    } else {

        for (int i = args->start; i < args->end; ++i) {
            for (int j = 0; j < args->num_arrays; ++j) {
                pthread_mutex_lock(args->mutex);
                args->result[i] += args->arrays[j][i];
                pthread_mutex_unlock(args->mutex);
            }
        }
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <num_arrays> <array_length> <max_threads>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int num_arrays = atoi(argv[1]);
    int array_length = atoi(argv[2]);
    int max_threads = atoi(argv[3]);

    if (num_arrays <= 0 || array_length <= 0 || max_threads <= 0) {
        fprintf(stderr, "Invalid parameters. All values must be positive.\n");
        return EXIT_FAILURE;
    }

    int **arrays = (int **)malloc(num_arrays * sizeof(int *));
    for (int i = 0; i < num_arrays; i++) {
        arrays[i] = (int *)malloc(array_length * sizeof(int));
        for (int j = 0; j < array_length; j++) {
            arrays[i][j] = rand() % 10; 
        }
    }
    int *result = (int *)calloc(array_length, sizeof(int));


    printf("Generated arrays:\n");
    for (int i = 0; i < num_arrays; i++) {
        for (int j = 0; j < array_length; j++) {
            printf("%d ", arrays[i][j]);
        }
        printf("\n");
    }

 
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

   
    bool row_split = (num_arrays / (double)array_length) > 2.0;
    printf("Using %s strategy\n", row_split ? "row_split" : "column_split");

   
    pthread_t *threads = (pthread_t *)malloc(max_threads * sizeof(pthread_t));
    thread_args_t *thread_args = (thread_args_t *)malloc(max_threads * sizeof(thread_args_t));

    int work_size = row_split ? num_arrays : array_length;
    for (int i = 0; i < max_threads; i++) {
        thread_args[i].arrays = arrays;
        thread_args[i].result = result;
        thread_args[i].num_arrays = num_arrays;
        thread_args[i].array_length = array_length;
        thread_args[i].start = i * work_size / max_threads;
        thread_args[i].end = (i + 1) * work_size / max_threads;
        thread_args[i].row_split = row_split;
        thread_args[i].mutex = &mutex;

        if (pthread_create(&threads[i], NULL, process_arrays, &thread_args[i]) != 0) {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }

  
    for (int i = 0; i < max_threads; i++) {
        pthread_join(threads[i], NULL);
    }

  
    printf("Result array: \n");
    for (int i = 0; i < array_length; i++) {
        printf("%d ", result[i]);
    }
    printf("\n");


    for (int i = 0; i < num_arrays; i++) {
        free(arrays[i]);
    }
    free(arrays);
    free(result);
    free(threads);
    free(thread_args);


    pthread_mutex_destroy(&mutex);

    return EXIT_SUCCESS;
}
