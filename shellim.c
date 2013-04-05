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
static int length_of_arguments;


void   argv_free();
int    run();
int    parse(char *);


int
main(void)
{
        char *komut;
        
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
        }
        atexit(argv_free);
        free(komut);
        return 0;
}


void 
CD()
{
        if (!strcmp(arguments[1], "~") || !strcmp(arguments[1], " ") || !strcmp(arguments[1], "")) {
                chdir(getenv("HOME"));
        }else if (!strcmp(arguments[1], ".")) {
        
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
argv_free()
{
        int j;
        
        for (j = 0; j < length_of_arguments ; j++) {
                free(arguments[j]);
        }
}


int
run()
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
        length_of_arguments = 0;
        add_history(string);
        
        while ( ( token = strsep(str_p, " \t") )) {
                arguments[length_of_arguments] = strdup(token);
                length_of_arguments++;
        }
        arguments[length_of_arguments] = NULL;
        if (!strcmp(arguments[0], "exit") || !strcmp(arguments[0], "quit")) {
                exit(EXIT_SUCCESS);
        }
        if (!strcmp(arguments[0], "cd")) {
                CD();
        }
        return 0;
}
