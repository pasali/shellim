#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<readline/readline.h>
#include<readline/history.h>
#include<sys/wait.h>


/*  pasali */

static char * arguments[10];
static char * komut;


void   argv_free(void);
int    run(void);
int    parse(char *);


int
main(void)
{
        while(1) {
                komut = readline("#> ");
                if (komut == NULL) {
                        exit(EXIT_SUCCESS);
                }else if (!strcmp(komut, "")) {
                        continue;
                }
                else {
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
CD(void)
{
        if (!strcmp(arguments[1], "~") || !strcmp(arguments[1], " ") || !strcmp(arguments[1], "")) {
                chdir(getenv("HOME"));
        }else if (!strcmp(arguments[1], ".")) {
        /* none */
        }else if (!strcmp(arguments[1], "..")) {
                char * parent_directory = strrchr(getenv("PWD"), '/');
                char * pwd = getenv("PWD");
                int index = parent_directory - pwd;
                pwd[index] = '\0';
                chdir(pwd);
        }else
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

        add_history(string);
        for (len = 0; (token = strsep(str_p, " \t")); len++) {
                arguments[len] = strdup(token);
        }
        arguments[len] = NULL;
        if (!strcmp(arguments[0], "exit") || !strcmp(arguments[0], "quit")) {
                exit(EXIT_SUCCESS);
        }
        if (!strcmp(arguments[0], "cd")) {
                CD();
        }
        if (komut[0] != '/' && strchr(komut, '/')) {
                printf("full path of program or just name !!!\n");
                main();
        }

        return 0;
}
