#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

string remove_extra_spaces(string input) {
    string output = "";
    string append_char = "";
    for (int i = 0; i < input.length(); i++) {
        if ((input[i] == ' ' && output == "") ||  (input[i] == ' ' && output.back() == ' ')){
            continue;
        } else { output += input[i]; }
    }
    output.erase(output.find_last_not_of(" \n\r\t")+1);
    return output;
}

void process_folder(string folder, vector<string>* args) {
    DIR *dir = opendir(folder.c_str());
    if (dir == NULL) return;
    struct dirent* entry;
    string name;
    string extension;
    bool is_header_folder = false;
    while((entry = readdir(dir)) !=NULL) {
        name = entry->d_name;
        extension = name.substr(name.find(".")+1,name.size());
        if (extension == "cpp") {
            if (folder == "./") {
                cout << folder << endl;
                args->push_back(folder+name);
            } else {
                args->push_back(folder+"/"+name);
            }
        }
        if (extension == "h" && !is_header_folder) {
            is_header_folder = true;
            args->push_back("-I" + folder);
        }
    }
}

int main(int argc, char* argv[]) {
    string c_file = "";
    if (argc >= 2 ) {
    c_file = string(argv[1]);
    }
    if (c_file.find(".cm") == string::npos) {
        cerr << "Error: Invalid Argument, a '.cm' file needs to be passes as an argument.\n";
        return -1;
    }
    ifstream c_file_stream(c_file);
    string   c_file_line;
    string   c_main;
    vector<string> c_args;
    string   run_string = "g++ ";
    size_t   index = 0;
    process_folder("./",&c_args);
    if (c_file_stream.is_open()) {
        while(getline(c_file_stream,c_file_line)) {
            c_file_line = remove_extra_spaces(c_file_line);
            index = c_file_line.find("-F ",0);
            if (index != string::npos) {
                process_folder(c_file_line.substr(3,c_file_line.length()-3), &c_args);
            }
            index = c_file_line.find("-o ",0);
            if (index != string::npos) {
                c_main = "./"+c_file_line.substr(3,c_file_line.length()-3) + ".exe";
                c_args.push_back(c_file_line);
            }
            index = c_file_line.find("$(");
            if (index != string::npos) {
                FILE* fp;
                fp = popen((c_file_line.substr(index+2,c_file_line.find(")",0)-2)).c_str(), "r");
                if (fp == NULL) {
                    cerr << "Failed to run command: " << c_file_line << endl;
                }
                char path[2048];
                if (fgets(path, sizeof(path), fp)!=NULL) {
                    c_args.push_back(string(path));
                }
            }
        }
        c_file_stream.close();
    } else {
        cout << "Cannot find file " << c_file << endl;
    }
    for (int i = 0; i < c_args.size(); i++) {
        run_string += " " + c_args[i] + " ";
    }
    cout << run_string << endl;
    if (system(run_string.c_str()) == 0) {
        c_main.erase(0,2);
        system(c_main.c_str());
    }
}