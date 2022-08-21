/* Jerin John 
 * 18 Feb 2022
 * C Program to serve as a shell interface that can accept user commands and then execute each command in a separate process
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 80

// error_message : Default Shell Error State
void error_message(char *a){
    printf("Error in osh: %s \n", a);
}

// check_input : Used to check the users input for length errors.
int check_input(char *a){
    char p;
    int n = 0;
    while (((p = getchar()) != '\n') && (n < MAX_LINE)) {
        a[n++] = p;
    }

    if(n == MAX_LINE && p != '\n') {
        error_message("Command exceeds maximum length");
    }
    else {
        a[n] = 0;
    }

    while(p != '\n'){
        p = getchar();
    }

    return n;
}

int check_input_len(char *a){
    int n = 0; 
    char c;

    while (((c = getchar()) != '\n') && (n < MAX_LINE)) {
        a[n++] = c;
    } 

    if (n == MAX_LINE && c != '\n') {
        error_message("Command exceeds maximum length");
    } else {
        a[n] = 0;
    }
    while(c != '\n') {
        c = getchar();
    }
    return n;
}

// history_output : Returns the history of up to the five most recent commands. 
void history_output(char history[][MAX_LINE + 1], int h_count){
    if (h_count == 0){
        error_message("No History Yet");
        return;
    }
    int i, j = 5;
    for (i = h_count; i > 0 && j > 0; i--, j--){
        printf("%4d\t %s \n", i, history[i]);
    }
    return;
}

// parse_intput : Reads and parses the users input
int read_input (char *buffer, int length, char* args[]){
    int args_n = 0, last = -1 , i;
    args[0] = NULL;
    for (i = 0; i <= length; ++i){
        if (buffer[i] != ' ' && buffer[i] != '\t' && buffer[i]){
            continue;
        }
        else {
            if (last != i-1) {
                args[args_n] = (char*)malloc(i-last);
                if (args[args_n] == NULL){
                    error_message("Unable to allocate memory");
                    return 1;
                }

                memcpy(args[args_n], &buffer[last+1], i-last-1);
                args[args_n][i-last] = 0;
                args[++ args_n] = NULL;
            }

            last = i;
        }
    }
    return args_n;
}

// to_num : Converts the string (array of Char) to an integer value
int to_num(char *a){
    int length = strlen(a), i, answer = 0;
    for (i = 0; i < length; i++) {
        if (a[i] > '9' || a[i] < '0'){
            return -1;
        }
        answer = answer * 10 + a[i] - '0';
    }
    return answer;
}

// main : Reponsible of running program

int main(void){
    char *args[MAX_LINE/2 + 1]; /* command line arguments */
    int should_run = 1; /* flag to determine when to exit program */

    char history[10][MAX_LINE + 1];
    int h_count = 0;

    char buffer[MAX_LINE + 1];
    memset(buffer,0, sizeof(buffer));
    int length, args_n;
    
    while (should_run) {
        printf("osh>");
        fflush(stdout);
        
        length = check_input_len(buffer);
        if(strcmp(buffer, "!!") == 0) {
            if (h_count > 0){
                memcpy(buffer, history[h_count], MAX_LINE + 1);
                length = strlen(buffer);
            } else {
                error_message("No commands in history");
                continue;
            }
        }

        args_n = read_input(buffer, length, args);

        if (args_n == 0) {
            continue;
        }

        if (strcmp(args[0], "!") == 0) {
            int temp = to_num(args[1]);
            if (temp <= 0 || temp < h_count - 9 || temp > h_count) {
                error_message("No such command in history");
                continue;
            } else {
                memcpy(buffer, history[temp], MAX_LINE + 1);
                length = strlen(buffer);
                args_n = read_input(buffer, length, args);
            }
        }

        if (strcmp(args[0], "exit") == 0) {
            should_run = 0;
            continue;
        }
        

        if (strcmp(args[0], "history") == 0) {
            history_output(history, h_count);
            continue;
        }
        h_count++;
        memcpy(history[h_count], buffer, MAX_LINE + 1);

        int bg = 0;
        if (strcmp(args[args_n-1], "&") == 0) {
            bg = 1;
            args[--args_n] = NULL;
        }

        pid_t pid = fork();
        if (pid < 0) {
            error_message("Error while creating fork");
            return 0;
        }

        int status;

        if (pid == 0){
            status = execvp(args[0], args);
            if (status == -1){
                error_message("Invalid Command");
            }
            return 0;
        } else {
            if (bg == 1){
                printf("%d \n", pid);
            }
            else{
                waitpid(pid, &status, 0);
            }
        }
     }
     return 0;
}

