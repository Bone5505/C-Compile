#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>

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

string remove_repeated_words(string input) {
    string temp = remove_extra_spaces(input);
    string word;
    map<string,int> word_count;
    string::size_type current_space = 0;
    string::size_type next_space;
    while ((next_space = temp.find(' ',current_space+1)) != string::npos) {
        word = temp.substr(current_space, next_space-current_space);
        word_count[word] = 1;
        current_space = next_space;
    }
    word = temp.substr(current_space, temp.length()-current_space);
    word_count[word] = 1;
    map<string,int>::reverse_iterator iter;
    string result = "";
    for(iter = word_count.rbegin(); iter != word_count.rend(); ++iter) {
        result += iter->first;
    }
    return result;
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
        if (extension == "cpp" || extension == "c") {
            if (folder == "./") {
                args->push_back(folder+name);
            } else {
                args->push_back(folder+"/"+name);
            }
        } else if (extension == "h" && !is_header_folder) {
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
            index = c_file_line.find("-f ",0);
            if (index != string::npos) {
                process_folder(c_file_line.substr(3,c_file_line.length()-3), &c_args);
                continue;
            }
            index = c_file_line.find("-o ",0);
            if (index != string::npos) {
                c_main = "./"+c_file_line.substr(3,c_file_line.length()-3) + ".exe";
                c_args.push_back(c_file_line);
                continue;
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
                    c_args.push_back(remove_repeated_words(string(path)));
                }
                continue;
            }

            c_args.push_back(c_file_line);
        }
        c_file_stream.close();
    } else {
        cerr << "Cannot find file " << c_file << endl;
    }
    for (int i = 0; i < c_args.size(); i++) {
        run_string += " " + c_args[i] + " ";
    }

    // Remove any eroneous new line characers
    string::size_type i = 0;
    while (i < run_string.length()) {
        i = run_string.find('\n', i);
        if (i == string::npos) {
            break;
        }
        run_string.erase(i,1);
    }
    run_string = remove_extra_spaces(run_string);
    cout << run_string << endl << endl;
    if (system(run_string.c_str()) == 0) {
        c_main.erase(0,2);
        system(c_main.c_str());
    }
}