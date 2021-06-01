# C-Compile
Compiles C++ code and runs the exe produced.
## Prequisits
* Mingw with added mingw/bin folder

## Installation
Simply compile the `ccompile.cpp` into the required executable. Then add it to the Path.
To add exrta arguments the one can create a `.cm` file and pass it as an argument to the ccompile executable.
To add a folder to look through for header or source files add `-f path/to/folder` and it will search through automatically. Make sure to add the space after the `-f`.
You can add the usuall Mingw compiler arguments, one on each line. Using `-o filename` it will automatically run the exe if it can find the file and it compiles correctly.