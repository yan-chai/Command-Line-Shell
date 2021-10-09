/**
* @file         shell.c
* @brief		This is the main part of program
* @details	    it has main function, handle the basic logic of the shell
* @author		Haoyu Yan
* @date		    2020/4/7
* @version	    v1.0
*/

#include <fcntl.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <locale.h>
#include <readline/readline.h>

#include "history.h"
#include "jobs.h"
#include "logger.h"
#include "ui.h"

int state;
int number;
bool scripting;
char *job = NULL;

//This is next_token.c from 

char *next_token(char **str_ptr, const char *delim)
{
    if (*str_ptr == NULL) {
        return NULL;
    }

    size_t tok_start = strspn(*str_ptr, delim);
    size_t tok_end = strcspn(*str_ptr + tok_start, delim);

    /* Zero length token. We must be finished. */
    if (tok_end  == 0) {
        *str_ptr = NULL;
        return NULL;
    }

    /* Take note of the start of the current token. We'll return it later. */
    char *current_ptr = *str_ptr + tok_start;

    /* Shift pointer forward (to the end of the current token) */
    *str_ptr += tok_start + tok_end;

    if (**str_ptr == '\0') {
        /* If the end of the current token is also the end of the string, we
         * must be at the last token. */
        *str_ptr = NULL;
    } else {
        /* Replace the matching delimiter with a NUL character to terminate the
         * token string. */
        **str_ptr = '\0';

        /* Shift forward one character over the newly-placed NUL so that
         * next_pointer now points at the first character of the next token. */
        (*str_ptr)++;
    }

    return current_ptr;
}

void handler(int sig) 
{
    LOG("This job end: %s\n", job);
    jobs_dele(job);
}

void handle_int() {
    //rl_replace_line("", 1);
    //printf("\n%s", prompt_line());      //Comment this line when test case 06 cannot pass!!!!!
    //fflush(stdout);
} 

int main(void)
{
    job = malloc(100);
    signal(SIGINT, SIG_IGN);
    signal(SIGCHLD, handler);
    hist_init(100);
    init_ui();
    jobs_init();
    number = 1;
    if (isatty(STDIN_FILENO)) {
        scripting = false;
    } else {
        scripting = true;
    }

    while (true) {
        char *line = read_command();
        if (line == NULL) {
            break;
        }
        char *p = line;
        const char *tmp = NULL;
        if (strncmp(p, "!", 1) == 0) {
            if (strncmp(p, "!!", 2) == 0) {
                tmp = hist_search_cnum(hist_last_cnum());
            } else if (*(p+1) > '0' && *(p+1) <= '9') {
                int com_num = atoi(p+1);
                tmp = hist_search_cnum(com_num-1);
            } else {
                tmp = hist_search_prefix(p+1);
            }
            if (tmp == NULL) {
                continue;
            } else {
                strcpy(line, tmp);
            }
        }
        for (int index = 0; index < strlen(line); index++) {
            if (line[index] == '#') {
                line[index] = '\0';
                break;
            }
        }
        LOG("Input command: %s\n", line);
        memcpy(job, line, 100);
        if (strlen(line) != 0) {
            hist_add(line);
        }
        char *args[10];
        int tokens = 0;
        char *curr_tok = NULL;
        while ((curr_tok = next_token(&line, " \t\n")) != NULL){
            args[tokens++] = curr_tok;
        }
        args[tokens] = NULL;
        if (args[0] == NULL) {
            continue;
        }
        number++;
        state = 0;
        if (strcmp("exit", args[0]) == 0) {
            LOGP("Exiting shell");
            printf("\n");
            break;
        }else if (strcmp(args[0], "cd") == 0) {
            if (tokens == 1) {
                chdir(getenv("HOME"));
            } else {
                chdir(args[1]);
            }
        } else if (strcmp(args[0], "jobs") == 0) {
            jobs_print();
        } else if (strcmp(args[0], "history") == 0) {
            hist_print();
        } else if (strcmp(args[tokens-1], "&") == 0) {
            for (int i = 0; i < strlen(job); i++) {
                if(*(job+i) == '&') {
                    LOG("%s\n", "FOND!!!!");
                    *(job+i) = '\0';
                }
            }
            jobs_add(job);
            args[tokens-1] = NULL;
            LOG("%s\t%s\t%s\n", args[0], args[1], args[2]);
            pid_t child = fork();
            if (child == -1) {
            perror("fork");
            } else if (child == 0) {
                for (int i = 0; i < tokens; i++) {
                    if (strcmp(args[i], ">") == 0) {
                        args[i] = NULL;
                        FILE *out = fopen(args[++i], "w");
                        dup2(fileno(out), fileno(stdout));
                    } else if (strcmp(args[i], ">>") == 0) {
                        args[i] = NULL;
                        FILE *out = fopen(args[++i], "a+");
                        dup2(fileno(out), fileno(stdout));
                    } else if (strcmp(args[i], "<") == 0) {
                        args[1] = args[++i];
                        args[2] = NULL;
                    }
                }
                execvp(args[0], args);
                perror("execvp");
                close(0);
                exit(-1);
            } else {
                waitpid(-1, &state, WNOHANG);
                LOG("%s\n", "Parent end.");
            }
        } else {
            pid_t child = fork();
                
            if (child == -1) {
                perror("fork");
            } else if (child == 0) {
                for (int i = 0; i < tokens; i++) {
                    if (strcmp(args[i], ">") == 0) {
                        args[i] = NULL;
                        FILE *out = fopen(args[++i], "w");
                        dup2(fileno(out), fileno(stdout));
                    } else if (strcmp(args[i], ">>") == 0) {
                        args[i] = NULL;
                        FILE *out = fopen(args[++i], "a+");
                        dup2(fileno(out), fileno(stdout));
                    } else if (strcmp(args[i], "<") == 0) {
                        args[1] = args[++i];
                        args[2] = NULL;
                    }
                }
                signal(SIGINT, SIG_DFL);
                execvp(args[0], args);
                perror("execvp");
                close(0);
                exit(-1);
            } else {
                wait(&state);
            }
        }
    }
    hist_destroy();
    destroy_ui();
    jobs_destroy();
    free(job);
    exit(0);
}
