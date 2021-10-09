/**
* @file         history.c
* @brief		This is the implement of ui part of shell
* @details	    It read user input and output prompt line for user to show code status and number of command.
* @author		Haoyu Yan
* @date		    2020/4/7
* @version	    v1.0
*/

#include <stdio.h>
#include <readline/readline.h>
#include <locale.h>
#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>
#include <stdbool.h>

#include "history.h"
#include "logger.h"
#include "ui.h"

static const char *good_str = "😌";
static const char *bad_str  = "🤯";
extern int state;
extern int number;
extern bool scripting;
char *command = NULL;
char *prompt_str = NULL;
size_t line_sz;

static int readline_init(void);

void init_ui(void)
{
    LOGP("Initializing UI...\n");
    state = 0;
    command = NULL;
    prompt_str = malloc(100);

    char *locale = setlocale(LC_ALL, "en_US.UTF-8");
    LOG("Setting locale: %s\n",
            (locale != NULL) ? locale : "could not set locale!");

    rl_startup_hook = readline_init;
}

void destroy_ui(void)
{
    // TODO cleanup code, if necessary
    free(prompt_str);
    free(command);
}

char *prompt_line(void)
{
    const char *status = prompt_status() ? bad_str : good_str;

    char cmd_num[25];
    snprintf(cmd_num, 25, "%u", prompt_cmd_num());

    char *user = prompt_username();
    char *host = prompt_hostname();
    char *cwd = prompt_cwd();

    char *format_str = ">>-[%s]-[%s]-[%s@%s:%s]-> ";

    size_t prompt_sz
        = strlen(format_str)
        + strlen(status)
        + strlen(cmd_num)
        + strlen(user)
        + strlen(host)
        + strlen(cwd)
        + 1;

    //char *prompt_str =  malloc(sizeof(char) * prompt_sz);

    snprintf(prompt_str, prompt_sz, format_str,
            status,
            cmd_num,
            user,
            host,
            cwd);

    hist_iter_init();
    return prompt_str;
}

char *prompt_username(void)
{
    struct passwd *p = getpwuid(getuid());
    return p->pw_name;
}

char *prompt_hostname(void)
{
    static char hostname[128];
    gethostname(hostname, 128);
    return hostname;
}

char *prompt_cwd(void)
{
    static char pwd[128];
    bool has_home = true;
    getcwd(pwd, 128);
    char *home_path = getenv("HOME");
    int length = strlen(home_path);
    int i = 0;
    while(i < length)
    {
        if (pwd[i] != home_path[i])
        {
            has_home = false;
            break;
        }
        i++;
    }
    if (has_home)
    {
        int index = 1;
        pwd[0] = '~';
        while (pwd[i] != '\0')
        {
            pwd[index] = pwd[i];
            i++;
            index++;
        }
        pwd[index] = '\0';
    }
    return pwd;
}

int prompt_status(void)
{
    return state;
}

unsigned int prompt_cmd_num(void)
{
    return number;
}

char *read_command(void)
{
    
    if (!scripting) {
        char *prompt = prompt_line();
        command = readline(prompt);
        return command;
    } else {
        ssize_t read_sz = getline(&command, &line_sz, stdin);
        if (read_sz == -1) {
            return NULL;
        }
        command[read_sz - 1] = '\0';
        return command;
    }
}

int readline_init(void)
{
    rl_bind_keyseq("\\e[A", key_up);
    rl_bind_keyseq("\\e[B", key_down);
    rl_variable_bind("show-all-if-ambiguous", "on");
    rl_variable_bind("colored-completion-prefix", "on");
    return 0;
}

int key_up(int count, int key)
{
    /* Modify the command entry text: */
    char *p = hist_prev();
    rl_replace_line(p, 1);

    /* Move the cursor to the end of the line: */
    rl_point = rl_end;

    // TODO: step back through the history until no more history entries are
    // left. Once the end of the history is reached, stop updating the command
    // line.

    return 0;
}

int key_down(int count, int key)
{
    /* Modify the command entry text: */
    char *p = hist_next();
    rl_replace_line(p, 1);

    /* Move the cursor to the end of the line: */
    rl_point = rl_end;

    // TODO: step forward through the history (assuming we have stepped back
    // previously). Going past the most recent history command blanks out the
    // command line to allow the user to type a new command.

    return 0;
}
