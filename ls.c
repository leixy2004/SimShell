#include <unistd.h>
#include <dirent.h>

int main(int agrc,char ** agrv) {
    char path[1<<8];
    getcwd(path,1<<8);
    DIR *dir = opendir(path);
    struct dirent *t= readdir(dir);
    
} 