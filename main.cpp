#include <iostream>
#include <string>
#include <regex>
#include <algorithm>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <climits>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/sysinfo.h>

using namespace std;

string promptString = "cwushell";
string promptChar = ">";

void manual(){
    cout << "CWU SHELL MANUAL"<< endl;
    cout << "implemented commands are:" << endl;
    cout << "exit [n], prompt, sysinf, meminf, and all other existing shell commands" << endl;
    cout << "For these implemented commands please use '[command] -help' for more information" << endl;
    cout << "prompt -switch -- changes the prompt" << endl;
    cout << "exit [n] -- terminates the shell" << endl;
    cout << "sysinf -switch -- will print different system information depending on switch" << endl;
    cout << "meminf -switch -- will print different memory related information depending on switch" << endl;
}

void exitCommand(string opArray[]) {
    string code = opArray[1];

    if(std::any_of(code.begin(), code.end(), ::isdigit)){
        std::string output = std::regex_replace(
                code,
                std::regex("[^0-9]*([0-9]+).*"),
                std::string("$1")
        );

        int numCode = stoi(output);
        std::cout << "Exiting the terminal with exit code " << numCode;
        exit(numCode);
    }else if(opArray[1] == "-h" || opArray[1] == "--help") {
        cout << "exit [n] terminates the shell" << endl;
        cout << "If an argument (n) is given, it will be the exit value of the shell's execution" << endl;
        cout << "If no argument is given, the exit value will be the value returned by the last executed command." << endl;
        cout << "The exit value will be 0 is no commands were executed" << endl;
    }else{
        cout<< "Exiting the terminal with exit code 0";
        exit(0);
    }
}

void promptChange(string opArray[]) {

    bool flag = false;

    if(opArray[1].empty()){
        promptString = "cwushell";
        promptChar = ">";
    }else{
        for(int i = 0; i < opArray->size(); i++){

            if(opArray[i] == "-c"){
                // character change with the following index
                promptChar = opArray[i + 1];
                flag = true;
            }
            else if(opArray[i] == "-s"){
                // change symbol w/ following index value
                promptString = opArray[i + 1];
                flag = true;
            }
            else if(opArray[i] == "-d"){
                // change to date with default character
                stringstream buffer;
                time_t t = time(nullptr);
                tm* tPtr = localtime(&t);
                buffer << (tPtr->tm_mon)+1<<"/"<< (tPtr->tm_mday) <<"/"<< (tPtr->tm_year)+1900;
                promptString = buffer.str();
                flag = true;
            }
        }
        if(!flag){
            // help message for prompt change
            cout << "prompt –switch will change the prompt. " << endl;
            cout << "-s will change the prompt to the string provided" << endl;
            cout << "the original prompt name is cwushell" << endl;
            cout << "-c will change the prompt ending character to the string provided" << endl;
            cout << "the original ending character is '>'" << endl;
        }
    }
}

void sysinf(string opArray[]){
    bool flag = false;

    for(int i = 0; i < opArray->size(); i++){
        if(opArray[i] == "-h"){
            // print hostname
            char hostname[HOST_NAME_MAX + 1];
            gethostname(hostname, HOST_NAME_MAX + 1);
            cout <<  hostname << endl;
            flag = true;
        } else if(opArray[i] == "-i"){
            // print out ip address of current machine
            char host[256];
            char *IP;
            struct hostent *host_entry;
            int hostname;
            //find the host name
            hostname = gethostname(host, sizeof(host));
            //find host information
            host_entry = gethostbyname(host);
            //check host entry
            if (host_entry == nullptr){
                perror("gethostbyname");
                exit(1);
            }
            IP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));
            cout << IP << endl;
            flag = true;
        }else if(opArray[i] == "-p"){
            // print number of processors and the types
            cout << "There are "<< get_nprocs_conf() << " configured processors and " << get_nprocs() << " processors are available" << endl;
            flag = true;
        }
    }
    if(!flag){
        cout << "sysinf -switch will print on the screen different system related information based on the switch. \nThe different switches are:" << endl;
        cout << "-h will print the hostname,"<< endl;
        cout << "-i will print the IP address of the current host machine" <<endl;
        cout << "-p will print the number of processors and the types" <<endl;
    }
}

int meminf(string opArray[]){
    struct sysinfo memInfo;
    sysinfo (&memInfo);
    //long long int physMemUsed = memInfo.totalram - memInfo.freeram;
    bool flag = false;

    for(int i = 0; i < opArray->size(); i++){
        if(opArray[i] == "-s"){
            // print available free swap space in bytes
            printf("free swap space in bytes: %lld\n", (long long int)memInfo.freeswap * memInfo.mem_unit);
            flag = true;
        }else if(opArray[i] == "-u"){
            // print available free memory in bytes
            printf("free memory in bytes: %lld\n", (long long int)memInfo.freeram * memInfo.mem_unit);
            flag = true;
        }else if(opArray[i] == "-S"){
            // print the total shared memory in bytes in the system
            printf("shared memory in bytes: %lld\n", (long long int)memInfo.sharedram * memInfo.mem_unit);
            flag = true;
        }
    }
    if(!flag){
        cout << "meminf -switch will print to the screen different memory related information based on the switch" << endl;
        cout << "-s -- will print available free swap space in bytes" << endl;
        cout << "-u -- will print available free memory in bytes" << endl;
        cout << "-S -- will print total shared memory in bytes" << endl;
    }
}

int mapGuide(string opArray[], string op){
    if(opArray[0] == "exit"){
        exitCommand(opArray);
    }else if(opArray[0] == "prompt"){
        promptChange(opArray);
    }else if(opArray[0] == "sysinf") {
        sysinf(opArray);
    }else if(opArray[0] == "meminf") {
        meminf(opArray);
    }else if(opArray[0] == "manual" || opArray[0] == "man"){
        manual();
    }else{
        system(op.c_str());
        }
    }

void openterm() {
    //op is original operation
    string op;

    while(true) {
        cout << promptString + promptChar;
        getline(cin, op);

        string opArray[op.size()];
        int i = 0;
        stringstream ssin(op);
        while (ssin.good() && i < op.size()) {
            ssin >> opArray[i];
            ++i;
        }
        mapGuide(opArray, op);
    }
}

int main() {
        std::cout << "====Starting terminal====" << std::endl;
        openterm();

}