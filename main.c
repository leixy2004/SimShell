#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "ANSI-color-codes.h"
#define kBuffSize (1<<12)

struct passwd *pwd ;
char hostname[kBuffSize];
char dir[kBuffSize];

void singal_handler(int SIG) {
    switch(SIG) {
        case SIGINT: {
            fprintf(stdout,"Process exit!\n");
            break;
        }
    }
}

bool in_cmd(char **argv) {
    if (strcmp(argv[0],"cd")==0) {
        if (argv[1]==NULL) {
            if (chdir(pwd->pw_dir)!=0) {
                printf("No such dir.\n");
            }
        } else if (argv[1]!=NULL && argv[2]==NULL) {
            if (chdir(argv[1])!=0) {
                printf("No such dir.\n");
            }
        } else {
            printf("cd cmd failed.\n");
        }
        return true;
    } else if (strcmp(argv[0],"exit")==0) {
        int exit_status=0;
        if (argv[1]!=NULL) {
            char *ptr_end;
            exit_status=(argv[1],&ptr_end,10);
        }
        exit(exit_status);
    } else if (strcmp(argv[0],"echo")==0) {
        int id=1;
        while (argv[id]!=NULL) {
            printf("%s ",argv[id]);
            id++;
        }
        printf("\n");
        return true;
    } else if (strcmp(argv[0],"pwd")==0) {
        printf("%s\n",dir);
        return true;
    }
        return false;
}

void real_exec_cmd(char **argv) {
    pid_t pid = fork();
    if (pid==0) {
        if (in_cmd(argv)) return;
        execvp(argv[0],argv);
        printf("Cmd Failed\n");
    } else {
        wait(NULL);
        printf("Cmd success.\n");
    }
}


void divide_space(char *str) {
    char *argv[kBuffSize];
    int argc=0;
    char *token=strtok(str," ");
    while (token!=NULL) {
        argv[argc++]=token;
        token=strtok(NULL," "); // can not deal with \"
    }
    argv[argc]=NULL;
    real_exec_cmd(argv);
}


void divide_pipe(char *str) {
    char *cmdv[kBuffSize];
    int cmdc=0;
    char *token=strtok(str,"|");
    while (token!=NULL) {
        // printf("token:%s\n",token);
        cmdv[cmdc++]=token;
        token=strtok(NULL,"|"); // can not deal with \"
    }
    cmdv[cmdc]=NULL;
    for (int i=0;i<cmdc;i++) {
        divide_space(cmdv[i]);
    }
}


void divide_semicolon(char *str) {
    char *cmdv[kBuffSize];
    int cmdc=0;
    char *token=strtok(str,";");
    while (token!=NULL) {
        // printf("token:%s\n",token);
        cmdv[cmdc++]=token;
        token=strtok(NULL,";"); // can not deal with \"
    }
    cmdv[cmdc]=NULL;
    for (int i=0;i<cmdc;i++) {
        divide_pipe(cmdv[i]);
    }
}

int main() {
    signal(SIGINT,singal_handler);
    char prompt[kBuffSize];
    using_history();
    pwd=getpwuid(getuid());
    while (true) {
        char *str;
        gethostname(hostname,kBuffSize);
        getcwd(dir,kBuffSize);
        sprintf(prompt,"(FAKE) " BHGRN "%s@%s" CRESET ":" BHBLU "%s" CRESET "%c ",pwd->pw_name,hostname,dir,(pwd->pw_uid ? '$' : '#'));
        str=readline(prompt);
        if (!str) break;
        add_history(str);
        // printf("%s\n",str);
        divide_semicolon(str);
        free(str);
    }
    return 0;
}