#include <sys/stat.h>
#include <unistd.h>
int main(int argc,char ** argv) {
    return rmdir(argv[1]);
}