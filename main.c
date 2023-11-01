#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include "ANSI-color-codes.h"
#define kBuffSize (1<<8)

void singal_handler(int SIG) {
    switch(SIG) {
        case SIGINT: {
            fprintf(stdout,"Process exit!\n");
            break;
        }        
    }
}

void exec_cmd(char *str) {

}

int main() {
    signal(SIGINT,singal_handler);
    struct passwd *pwd ;
    char hostname[kBuffSize];
    char dir[kBuffSize];
    char prompt[kBuffSize];
    using_history();

    while (1) {
        char *str;
        pwd=getpwuid(getuid());
        gethostname(hostname,kBuffSize);
        getcwd(dir,kBuffSize);
        sprintf(prompt,"(FAKE) " BHGRN "%s@%s" CRESET ":" BHBLU "%s" CRESET "%c ",pwd->pw_name,hostname,dir,(pwd->pw_uid ? '$' : '#'));
        str=readline(prompt);
        if (!str) break;
        add_history(str);
        printf("%s\n",str);
        free(str);
    }
    return 0;
}