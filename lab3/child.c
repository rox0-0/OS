#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <stdio.h>

#define SHM_NAME "/my_shared_memory"
#define SEM_PARENT_NAME "/sem_parent"
#define SEM_CHILD_NAME "/sem_child"
#define SHM_SIZE 1024

#define BUFFER_SIZE 512

typedef enum {
    SUCCESS = 0,
    INVALID_INPUT,
    DIVISION_BY_ZERO,
    INT_OVERFLOW,
} ERROR_CODES;

ERROR_CODES string_to_int(const char *str_number, int *int_result) {
    if (str_number == NULL || int_result == NULL)
        return INVALID_INPUT;

    char *endptr;
    errno = 0;
    long result = strtol(str_number, &endptr, 10);

    if ((result == LONG_MAX || result == LONG_MIN) && errno == ERANGE)
        return INT_OVERFLOW;
    else if (*endptr != '\0' || result > INT_MAX || result < INT_MIN)
        return INVALID_INPUT;

    *int_result = (int)result;
    return SUCCESS;
}

void error_print(const char *error_str) {
    if (error_str == NULL) {
        write(STDOUT_FILENO, "ERROR\n", 6);
    } else {
        write(STDOUT_FILENO, error_str, strlen(error_str));
    }
}

void print_division_result(int result) {
    char buffer[BUFFER_SIZE];
    int length = snprintf(buffer, sizeof(buffer), "Division result: %d\n", result);
    write(STDOUT_FILENO, buffer, length);
}

int main() {
 
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        error_print("Failed to open shared memory\n");
        exit(EXIT_FAILURE);
    }

    void *shm_ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        error_print("Failed to map shared memory\n");
        exit(EXIT_FAILURE);
    }

    
    sem_t *sem_parent = sem_open(SEM_PARENT_NAME, 0);
    sem_t *sem_child = sem_open(SEM_CHILD_NAME, 0);
    if (sem_parent == SEM_FAILED || sem_child == SEM_FAILED) {
        error_print("Failed to open semaphores\n");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    while (1) {
        
        sem_wait(sem_child);

        
        strncpy(buffer, (char *)shm_ptr, BUFFER_SIZE - 1);
        buffer[BUFFER_SIZE - 1] = '\0';

        if (strcmp(buffer, "EOF") == 0) {
            break; 
        }

        int result = 0;
        int is_first_number = 1;

        char *token = strtok(buffer, " ");
        while (token != NULL) {
            int current_value;
            ERROR_CODES error = string_to_int(token, &current_value);

            if (error != SUCCESS) {
                error_print("ERROR: Invalid input or overflow\n");
                exit(error);
            }
            if (current_value == 0) {
                error_print("ERROR: Division by zero\n");
                exit(DIVISION_BY_ZERO);
            }

            if (is_first_number) {
                result = current_value;
                is_first_number = 0;
            } else {
                result /= current_value;
            }

            token = strtok(NULL, " ");
        }

        print_division_result(result);
        sem_post(sem_parent); 
    }

    munmap(shm_ptr, SHM_SIZE);
    sem_close(sem_parent);
    sem_close(sem_child);

    return 0;
}
