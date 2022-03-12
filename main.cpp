#include <iostream>
#include "dissem.h"
using namespace std;
int main(int argc, char *argv[]) {
    if(argc == 3)
    {
        dissem Disassembler;
        Disassembler.openObjectFile(argv[1]);
        Disassembler.openSymbolFile(argv[2]);
        Disassembler.readFile();
    }
    else 
    {
        cout << "ERROR" << endl;
    }
}