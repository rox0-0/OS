#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void error_print(const char * str)
{

    if (str == NULL)
    {
        write(STDERR_FILENO, "ERROR", 6);
    }
    write(STDERR_FILENO, str, strlen(str));
	exit(EXIT_FAILURE);
}

int main(int args, char *argv[])
{

    if (args != 2)
    {
        error_print("wrong input, try one file\n");
    }

    FILE *file = fopen(argv[1], "r");
    if (!file)
    {
        error_print("file didnt opened\n");
    }

    int fd[2];
    
    if (pipe(fd) == -1)
    {
        error_print("pipe failed\n");
    }

    pid_t pid = fork();
    

    if (pid == 0)
    {
        
        close(fd[1]);

        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);

        execl("./child", "", NULL);

        error_print("execl failed \n");
        
    }
	else if (pid < 0)
    {
        error_print("fork failed\n");
        
    }
    else
    {
        close(fd[0]);

        char file_buffer[BUFSIZ];
        while (fgets(file_buffer, sizeof(file_buffer), file) != NULL)
        {
            write(fd[1], file_buffer, strlen(file_buffer));
        }
        close(fd[1]);

        fclose(file);
        wait(NULL);
    }

    return 0;
}
