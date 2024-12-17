#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SHM_NAME "/my_shared_memory"
#define SEM_PARENT_NAME "/sem_parent"
#define SEM_CHILD_NAME "/sem_child"
#define SHM_SIZE 1024

void error_print(const char *str) {
    if (str == NULL) {
        write(STDERR_FILENO, "ERROR\n", 6);
    } else {
        write(STDERR_FILENO, str, strlen(str));
    }
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        error_print("Wrong input, try one file\n");
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        error_print("File didn't open\n");
    }

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        error_print("Failed to create shared memory\n");
    }
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        error_print("Failed to set size of shared memory\n");
    }

    void *shm_ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        error_print("Failed to map shared memory\n");
    }

    sem_t *sem_parent = sem_open(SEM_PARENT_NAME, O_CREAT, 0666, 1);
    sem_t *sem_child = sem_open(SEM_CHILD_NAME, O_CREAT, 0666, 0);
    if (sem_parent == SEM_FAILED || sem_child == SEM_FAILED) {
        error_print("Failed to create semaphores\n");
    }

    pid_t pid = fork();

    if (pid == 0) {
      
        execl("./child", "./child", NULL);
        error_print("execl failed\n");
    } else if (pid < 0) {
        error_print("fork failed\n");
    } else {

        char file_buffer[BUFSIZ];
        while (fgets(file_buffer, sizeof(file_buffer), file) != NULL) {

            sem_wait(sem_parent);

    
            strncpy((char *)shm_ptr, file_buffer, SHM_SIZE - 1);
            ((char *)shm_ptr)[SHM_SIZE - 1] = '\0'; 


            sem_post(sem_child);
        }

        sem_wait(sem_parent);
        strncpy((char *)shm_ptr, "EOF", SHM_SIZE);
        sem_post(sem_child);
        wait(NULL);

        // Очистка
        fclose(file);
        munmap(shm_ptr, SHM_SIZE);
        shm_unlink(SHM_NAME);
        sem_close(sem_parent);
        sem_close(sem_child);
        sem_unlink(SEM_PARENT_NAME);
        sem_unlink(SEM_CHILD_NAME);
    }

    return 0;
}
