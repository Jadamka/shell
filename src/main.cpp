#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <functional>

enum INPUT_TYPE 
{
    EXECUTABLE,
    BUILT_IN,
};

INPUT_TYPE getType(std::string &input)
{
    std::vector<std::string> builtInCommands = {"cd", "pwd", "exit", "export", "unset", "echo", "alias", "unalias"};

    for(int i = 0; i < builtInCommands.size(); i++){
        if(strcmp(input.c_str(), builtInCommands[i].c_str()) == 0){
            return BUILT_IN;
        }
    }

    return EXECUTABLE;
}

std::vector<std::string> tokenize(std::string &input, std::string delimiter)
{
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while((pos = input.find(delimiter)) != std::string::npos){
        token = input.substr(0, pos);
        tokens.push_back(token);
        input.erase(0, pos + delimiter.length());
    }
    tokens.push_back(input);
    
    return tokens;
}

void executeCommand(std::vector<std::string> &tokens)
{
    pid_t pid = fork();

    if(pid == 0){
        //char *args[] = {(char*) tokens[0].c_str(), nullptr};
        std::vector<char*> args;
        for(int i = 0; i < tokens.size(); i++){
            args.push_back(strdup(tokens[i].c_str()));
        }
        args.push_back(nullptr);

        execvp(args[0], args.data());
        std::cerr << "Command not found: " << args[0] << "\n";
        exit(1);
    }
    else if(pid > 0){
        wait(nullptr);
    }
    else{
        std::cerr << "Fork failed!\n";
    }
}

void executeBuiltinCommand(std::vector<std::string> &tokens)
{
    static std::unordered_map<std::string, std::function<void()>> builtins = {
        {"cd", []() {}},
        {"pwd", []() {}},
        {"exit", []() {}},
    };

    auto it = builtins.find(tokens[0]);
    if (it != builtins.end()){
        it->second();
    }
}

int main(int argc, char *argv[])
{
    std::string input;

    while(true){
        std::cout << "shell> ";
        getline(std::cin, input);

        if(input.empty()){
            continue;
        }

        std::vector<std::string> tokens = tokenize(input, " ");
        INPUT_TYPE type = getType(tokens[0]);

        if(type == EXECUTABLE){
            executeCommand(tokens);
        }
        else{
            executeBuiltinCommand(tokens);
        }

    }
    
    return 0;
}
