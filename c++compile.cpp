#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <sys/types.h>
#include <algorithm>
#include <windows.h>

using namespace std;

bool SearchFolder(string path, vector<string*>* sources,vector<string>* headers, string* argsFile) {
  vector<string> folders;
  bool is_in_own_folder = false;
  bool headerFolder = false;
  struct dirent *entry;
  DIR *dir = opendir(path.c_str());
  if(dir == NULL) return false;

  string name;
  string* folder_file_ext;

  while((entry = readdir(dir))!=NULL) {
    folder_file_ext = new string[3];
    name = entry->d_name;
    folder_file_ext[0] = path;
    folder_file_ext[1] = name.substr(0,name.find("."));
    folder_file_ext[2] = name.substr(name.find(".")+1,name.size());
    if (folder_file_ext[1] == "" || folder_file_ext[1] == ".") {continue;}

    if (name == "c++compile.cpp") {is_in_own_folder = true; break;}

    if (folder_file_ext[2] == "cpp") {
      sources->push_back(folder_file_ext);
    } else if (folder_file_ext[2] == "h" && !headerFolder) {
      headers->push_back(path);
      headerFolder = true;
    } else if (folder_file_ext[2] == "args") {
      string library = path+name;
      *argsFile = library;
    } else if(folder_file_ext[1] == folder_file_ext[2]) {
      
      folders.push_back(path+folder_file_ext[1]+"\\");
    }
  }
  closedir(dir);
  for (string folder : folders) {
    SearchFolder(folder,sources,headers, argsFile);
  }
  return is_in_own_folder;
}

int  main () {
  vector<string*> sources; //List of sources split into folder file extension format
  vector<string> headers; //List of header folder.
  string arg_file = ""; //File with list of arguments in it
  string path = ".\\";
  string* main_file = new string[3];
  //Read Through All folders + sub folders
  if (SearchFolder(path,&sources,&headers,&arg_file)) {
    return 0;
  }

  if (sources.size() == 0) {
    exit(0);
  }

  ifstream read_file;
  string line;
  for (auto source : sources) {
    read_file = ifstream((source[0]+source[1]+"."+source[2]));
    while(getline(read_file,line)) {
      line.erase(std::remove(line.begin(),line.end(),' '), line.end());
      transform(line.begin(),line.end(),line.begin(), ::tolower);
      if (line.find("intmain(") != string::npos || line.find("intwmain(") != string::npos || line.find("winmain(") != string::npos) {
        if( line.find("//") == string::npos) {
          main_file = source;
        }
      }
    }
    read_file.close();
  }

  // Read argument list
  vector<string> args_list;
  read_file = ifstream(arg_file);
  while(getline(read_file,line)) {
    line.erase(std::remove(line.begin(),line.end(),' '), line.end());
    if (line.size() > 0) {
      args_list.push_back(line);
    }
  }

  // Create Compile String
  string run_string = "g++ ";
  for (string header : headers)
  {
    run_string += "-I" + header + " ";
  }
  run_string += "-I " + std::string("C:\\MinGW-w64\\mingw64\\lib\\gcc\\x86_64-w64-mingw32\\8.1.0\\include\\c++ ");
  run_string += "-I " + std::string("C:\\MinGW-w64\\mingw64\\include ");
  
  run_string += "-g ";
  for (string* source : sources)
  { 
    source[0].erase(0,2);
    run_string += "\"" + source[0] + source[1]+ "." + source[2]+"\" ";
  }
  run_string += " -o " + main_file[1];
  for (int i = 0; i < args_list.size(); i++)
  {
    run_string += " -" +  string(args_list[i]);
  }
  std::cout << run_string.c_str() << std::endl;
  if (system(run_string.c_str()) == 0) {
    system((".\\" + main_file[1] + ".exe").c_str());
  }
  delete[] main_file;
  sources.erase();
}