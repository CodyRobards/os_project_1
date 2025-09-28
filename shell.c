#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT_LENGTH 1024
#define MAX_TOKENS 16
#define MAX_TOKEN_LENGTH 64
#define PID_HISTORY 5

static pid_t pid_history[PID_HISTORY];
static size_t pid_count = 0;
static size_t pid_start = 0;

static void record_pid(pid_t pid) {
    if (pid_count < PID_HISTORY) {
        pid_history[(pid_start + pid_count) % PID_HISTORY] = pid;
        pid_count++;
    } else {
        pid_history[pid_start] = pid;
        pid_start = (pid_start + 1) % PID_HISTORY;
    }
}

static void show_pid_history(void) {
    for (size_t i = 0; i < pid_count; ++i) {
        size_t index = (pid_start + i) % PID_HISTORY;
        printf("%d\n", pid_history[index]);
    }
}

static void print_prompt(void) {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        strcpy(cwd, "?");
    }
    printf("\033[1;34m%s\033[0m$ ", cwd);
    fflush(stdout);
}

static int change_directory(char *path) {
    const char *target = path;
    if (target == NULL) {
        target = getenv("HOME");
        if (target == NULL) {
            target = ".";
        }
    }

    if (chdir(target) == -1) {
        perror("cd");
        return -1;
    }

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        if (setenv("PWD", cwd, 1) == -1) {
            perror("setenv");
        }
    } else {
        perror("getcwd");
    }

    return 0;
}

int main(void) {
    char input[MAX_INPUT_LENGTH];

    while (1) {
        print_prompt();

        if (fgets(input, sizeof(input), stdin) == NULL) {
            if (feof(stdin)) {
                putchar('\n');
                break;
            }
            clearerr(stdin);
            continue;
        }

        if (input[0] == '\n' || input[0] == '\0') {
            continue;
        }

        input[strcspn(input, "\n")] = '\0';

        char *tokens[MAX_TOKENS + 1];
        size_t token_count = 0;
        int invalid = 0;

        char *saveptr = NULL;
        char *token = strtok_r(input, " \t", &saveptr);
        while (token != NULL) {
            size_t length = strlen(token);
            if (length > MAX_TOKEN_LENGTH) {
                fprintf(stderr, "Error: token exceeds maximum length (%d).\n", MAX_TOKEN_LENGTH);
                invalid = 1;
                break;
            }

            if (token_count >= MAX_TOKENS) {
                fprintf(stderr, "Error: too many tokens (maximum %d).\n", MAX_TOKENS);
                invalid = 1;
                break;
            }

            tokens[token_count++] = token;
            token = strtok_r(NULL, " \t", &saveptr);
        }

        if (invalid || token_count == 0) {
            continue;
        }

        tokens[token_count] = NULL;

        if (strcmp(tokens[0], "exit") == 0) {
            break;
        }

        if (strcmp(tokens[0], "cd") == 0) {
            change_directory(token_count > 1 ? tokens[1] : NULL);
            continue;
        }

        if (strcmp(tokens[0], "showpid") == 0) {
            show_pid_history();
            continue;
        }

        pid_t child_pid = fork();
        if (child_pid == -1) {
            perror("fork");
            continue;
        }

        if (child_pid == 0) {
            execvp(tokens[0], tokens);
            perror("execvp");
            _exit(EXIT_FAILURE);
        }

        int status = 0;
        pid_t waited_pid = waitpid(child_pid, &status, 0);
        if (waited_pid == -1) {
            perror("waitpid");
            continue;
        }

        record_pid(waited_pid);
    }

    return 0;
}
