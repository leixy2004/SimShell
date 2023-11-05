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
#include <sys/stat.h>
#include <stdbool.h>
#include <fcntl.h>
#define __POSIX_C_SOURCE
#include "ANSI-color-codes.h"
#define kBuffSize (1<<12)

void swap(int *a,int *b) {
    int *t=a;
    a=b;
    b=t;
}

void show_argv(char **argv) {
    // fprintf(stderr,"\n");
    for (int i=0;argv[i]!=NULL;i++) {
        fprintf(stderr,"[%d]:%s",i,argv[i]);
    }
    fprintf(stderr,"\n");
}

struct passwd *pwd ;
char hostname[kBuffSize];
char dir[kBuffSize];

void parent_singal_handler(int SIG) {
    switch(SIG) {
        case SIGINT: {
            break;
        }
    }
}

void child_singal_handler(int SIG) {
    switch(SIG) {
        case SIGINT: {
            fprintf(stdout,"Process exit!\n");
            exit(EXIT_SUCCESS);
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

void real_exec_cmd(char **argv,int fd_in,int fd_out) {
    fprintf(stdout,"cmd in:%d out:%d\n",fd_in,fd_out);
    show_argv(argv);
    fprintf(stdout,"\n");


    pid_t pid = fork();
    if (pid==0) {
        signal(SIGINT,child_singal_handler);
        if (in_cmd(argv)) return;
        if (fd_in!=STDIN_FILENO) {
            dup2(fd_in,STDIN_FILENO);
        }
        if (fd_out!=STDOUT_FILENO) {
            dup2(fd_out,STDOUT_FILENO);
        }
        execvp(argv[0],argv);
        printf("Cmd Failed\n");
    } else {
        wait(NULL);
        printf("Cmd success.\n");
    }
}


void divide_space(char *str,int fd_in,int fd_out) {
    char *argv[kBuffSize];
    int argc=0;
    char *token=strtok(str," ");
    while (token!=NULL) {
        argv[argc++]=token;
        token=strtok(NULL," "); // can not deal with \"
    }
    argv[argc]=NULL;
    for (int i=0;i<argc;i++) {
        if (argv[i]==NULL) continue;
        if (strcmp(">",argv[i])==0) {
            if (fd_out!=STDOUT_FILENO || i+1>=argc || argv[i+1]==NULL) {
                perror("pipe failed.\n");
                return;
            }
            mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

            fd_out=creat(argv[i+1],mode);
            argv[i]=argv[i+1]=NULL;
        } else if (strcmp("<",argv[i])==0) {
            if (fd_in!=STDIN_FILENO || i+1>=argc || argv[i+1]==NULL) {
                perror("pipe failed.\n");
                return;
            }

            fd_in=open(argv[i+1],O_RDONLY);
            argv[i]=argv[i+1]=NULL;
        }
    }
    for (int i=0;i<argc;i++) {
        if (argv[i]!=NULL) continue;
        for (int j=i+1;j<argc;j++) {
            if (argv[j]!=NULL) {
                argv[i]=argv[j];
                argv[j]=NULL;
                break;
            }
        }
    }
    real_exec_cmd(argv,fd_in,fd_out);
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
    int stream[(cmdc)*2];
    stream[0]=STDIN_FILENO;
    stream[(cmdc)*2-1]=STDOUT_FILENO;
    for (int i=0;i<cmdc-1;i++) {
        int p[2];
        if (pipe(p)==-1) {
            perror("FAILED PIPE.\n");
            return;
        }
        stream[i*2+1]=p[1];  // write
        stream[i*2+2]=p[0];  // read
    }
    for (int i=0;i<(cmdc)*2;i++) {
        printf("s[%d]:%d ",i,stream[i]);
    }
    cmdv[cmdc]=NULL;
    for (int i=0;i<cmdc;i++) {
        divide_space(cmdv[i],stream[i*2],stream[i*2+1]);
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
    signal(SIGINT,parent_singal_handler);

    char prompt[kBuffSize];
    using_history();
    pwd=getpwuid(getuid());
    while (true) {
        char *str;
        gethostname(hostname,kBuffSize);
        getcwd(dir,kBuffSize);
        sprintf(prompt,"(FAKE) " BHGRN "%s@%s" CRESET ":" BHBLU "%s" CRESET "%c ",pwd->pw_name,hostname,dir,(pwd->pw_uid ? '$' : '#'));
        fflush(stdout);
        str=readline(prompt);
        if (!str) break;
        add_history(str);
        // printf("%s\n",str);
        divide_semicolon(str);
        free(str);
    }
    return 0;
}