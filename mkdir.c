#include <sys/stat.h>

int main(int argc,char ** argv) {
    return mkdir(argv[1],S_IRWXU);
}