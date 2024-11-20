#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <stdio.h>

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
    char buffer[BUFFER_SIZE];

    
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        int result = 0;
        int is_first_number = 1;  
        buffer[strcspn(buffer, "\n")] = '\0';  

        char *token = strtok(buffer, " ");  

        while (token != NULL) {
            int current_value;
            ERROR_CODES error = string_to_int(token, &current_value);

            if (error == INT_OVERFLOW) {
                error_print("ERROR: Integer overflow\n");
                return INT_OVERFLOW;
            } else if (error == INVALID_INPUT) {
                error_print("ERROR: Invalid input\n");
                return INVALID_INPUT;
            } else if (error == SUCCESS) {
                
                if (current_value == 0) {
                    error_print("ERROR: Division by zero\n");
                    return DIVISION_BY_ZERO;
                }

                if (is_first_number) {
                    result = current_value;
                    is_first_number = 0;
                } else {
                    
                    if (result == INT_MIN && current_value == -1) {
                        error_print("ERROR: Integer overflow on division\n");
                        return INT_OVERFLOW;
                    }
                    result /= current_value;
                }
            }

            token = strtok(NULL, " ");
        }

        print_division_result(result);
    }

    return SUCCESS;
}
