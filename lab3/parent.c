#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define SHM_NAME "/shared_memory"
#define SEM_NAME "/sync_semaphore"
#define BUFFER_SIZE 1024
#define NUM_LINES 100

void error_handler(const char *msg) {
    write(STDERR_FILENO, msg, strlen(msg));
    write(STDERR_FILENO, "\n", 1);
    exit(EXIT_FAILURE);
}

int main() {
    int shm_fd;
    char *shared_mem;
    sem_t *semaphore;
    ssize_t bytesRead;

    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        error_handler("ERROR: broken creation of shared memory");
    }

    if (ftruncate(shm_fd, BUFFER_SIZE * NUM_LINES) == -1) {
        error_handler("ERROR: broken size changing for shared memory");
    }

    shared_mem = mmap(NULL, BUFFER_SIZE * NUM_LINES, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) {
        error_handler("ERROR: broken displaying for shared memory");
    }

    semaphore = sem_open(SEM_NAME, O_CREAT, 0666, 0);
    if (semaphore == SEM_FAILED) {
        error_handler("ERROR: broken semaphore creation");
    }

    char filename[BUFFER_SIZE];
    const char *prompt = "Put file name : ";
    write(STDOUT_FILENO, prompt, strlen(prompt));
    bytesRead = read(STDIN_FILENO, filename, sizeof(filename));
    if (bytesRead <= 0) {
        error_handler("ERROR: error of reading filename");
    }

    size_t len = strlen(filename);
    if (len > 0 && filename[len - 1] == '\n') {
        filename[len - 1] = '\0';
    }

    strncpy(shared_mem, filename, BUFFER_SIZE);

    pid_t child_pid = fork();
    if (child_pid == -1) {
        error_handler("ERROR: error of creating child process");
    }

    if (child_pid == 0) {
        execl("./child", "./child", NULL);
        error_handler("ERROR: error in child process");
    } else {
        sem_post(semaphore);

        wait(NULL);

        for (int i = 0; i < NUM_LINES; i++) {
            if (strlen(shared_mem + i * BUFFER_SIZE) > 0) {
                write(
                        STDOUT_FILENO, shared_mem + i * BUFFER_SIZE, strlen(
                                shared_mem + i * BUFFER_SIZE
                                )
                        );
            }
        }

        munmap(shared_mem, BUFFER_SIZE * NUM_LINES);
        shm_unlink(SHM_NAME);
        sem_close(semaphore);
        sem_unlink(SEM_NAME);

        exit(EXIT_SUCCESS);
    }
}
