#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>
#include <string>
#include <pwd.h>
#include <unistd.h>

int main() {
    auto pwd = getpwuid(getuid());
    std::string username = pwd->pw_name;
    std::string address;
    std::string hostname;
    char buff[1<<8];
    gethostname(buff,(1<<8));
    hostname=buff;
    std::string prompt = pwd->pw_name;
    address=pwd->pw_dir;
    while (true) {
        auto str = readline(("\e[1;92m"+username+"@"+hostname+"\e[0m"+":"+"\e[1;94m"+address+"\e[0m"+"$ ").c_str());
    }
}