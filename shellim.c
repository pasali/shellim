#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<readline/readline.h>
#include<readline/history.h>
#include<sys/wait.h>


static char *arguments[10];
static char *komut;


void   argv_free(void);
int    run(void);
int    parse(char *);
void   command_cd(void);
void   command_exit(void);
struct command_t find_command(char *);

struct command_t {
        char *command;
        void (*handler) (void);
} commands[] = {
        {"cd", &command_cd},
        {"quit", &command_exit},
        {"exit", &command_exit},
        {NULL, NULL}
};


int
main(void)
{
        for ( ; ; ) {
                komut = readline("#> ");
                if (komut == NULL) {
                        exit(EXIT_SUCCESS);
                } else if (!strcmp(komut, "")) {
                        continue;
                } else {
                        parse(komut);
                        run();
                        argv_free();
                }
                free(komut);
        }
        atexit(argv_free);

        return 0;
}


void
command_exit(void)
{
        exit(EXIT_SUCCESS);
}


void
command_cd(void)
{
        if (!strcmp(arguments[1], "~") || !strcmp(arguments[1], " ") || !strcmp(arguments[1], "")) {
                chdir(getenv("HOME"));
        } else if (!strcmp(arguments[1], ".")) {
                /* none */
        } else if (!strcmp(arguments[1], "..")) {
                char * parent_directory = strrchr(getenv("PWD"), '/');
                char * pwd = getenv("PWD");
                int index = parent_directory - pwd;
                pwd[index] = '\0';
                chdir(pwd);
        } else
                chdir(arguments[1]);
        main();
}


void
argv_free(void)
{
        int i;

        for (i = 0;  arguments[i]; i++) {
                free(arguments[i]);
        }
}


int
run(void)
{
        int status;
        pid_t pid;
        pid = fork();

        if (!pid) {
                int ret;
                if ( **arguments == '/')
                        ret = execv(arguments[0], arguments);
                else
                        ret = execvp(arguments[0], arguments);
                if (ret == -1) {
                        perror("execv");
                        exit(EXIT_FAILURE);
                }
        } else if (pid == -1)
                perror("fork");

        if (waitpid(-1, &status, 0) == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
        }

        return 0;
}


int
parse(char *string)
{
        char ** str_p = &string;
        char * token;
        int len;
        struct command_t ret;

        add_history(string);
        for (len = 0; (token = strsep(str_p, " \t")); len++) {
                arguments[len] = strdup(token);
        }
        arguments[len] = NULL;
        ret = find_command(arguments[0]);
        if (ret.command != NULL) {
                ret.handler();
        }
        if (komut[0] != '/' && strchr(komut, '/')) {
                printf("full path of program or just name !!!\n");
                main();
        }

        return 0;
}

struct command_t
find_command(char * cmd)
{
        int i;

        for (i = 0; i < 3 ; i++) {
                if (!strcmp(cmd, commands[i].command)) {
                        return commands[i];
                }
        }
        return commands[i];
}
