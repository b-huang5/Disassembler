#include <fstream>
#include <iostream>
#include "dissem.h"
#include <math.h>
#include <strstream>
#include <sstream>
#include <iomanip>

std::ifstream objectFile;
std::ifstream symbolFile;
std::string objectLine;
std::string symbolLine;
std::ofstream outputFile;

std::string symbolName[100];
std::string symbolAddress[100];
std::string litName[100];
std::string litConst[100];
std::string litLength[100];
std::string litAddress[100];

std::string output;
std::string header;

int symbolArrayLength = 0;
int litArrayLength = 0;
int pcCounter = 0;
int base;
int p = 0;
std::string end;

void dissem::openObjectFile(std::string input) {
    objectFile.open(input);
    if(!(objectFile.is_open())) {
        std::cout << "File cannot be opened";
        exit(EXIT_FAILURE);
    }
}

void dissem::openSymbolFile(std::string input) {
    symbolFile.open(input);
    if(!(symbolFile.is_open())) {
        std::cout << "File cannot be opened";
        exit(EXIT_FAILURE);
    }
}

void dissem::readFile() {
    //opens out.lst
    outputFile.open("out.lst");
    if(!(outputFile.is_open())) {
        std::cout << "File cannot be opened";
        exit(EXIT_FAILURE);
    }
    getline(symbolFile, symbolLine);
    getline(symbolFile, symbolLine);
    getline(symbolFile, symbolLine);
    //reads in symbol and symbol address
    while(symbolLine.substr(0,4) != "Name") {
        for(int i = 0; i < symbolLine.length(); i++) {
            if(symbolLine[i] != ' ') {
                symbolName[symbolArrayLength] = symbolName[symbolArrayLength] + symbolLine[i];
            }
            else {
                for(int j = i; j < symbolLine.length(); j++) {
                    if(symbolLine[j] != ' ' && symbolLine[j] != 'R') {
                        symbolAddress[symbolArrayLength] = symbolAddress[symbolArrayLength] + symbolLine[j];
                    }
                }
                break;
            }
        }
        symbolArrayLength++;
        getline(symbolFile, symbolLine);
    }
    getline(symbolFile, symbolLine);
    getline(symbolFile, symbolLine);
    //reads in Name, Lit_Const, Length, and Address
    int sorter = 0;
    while(true) {
        if(symbolLine.empty()) {
            break;
        }
        else {
            for(int i = 0; i < symbolLine.length(); i++) {
                if(symbolLine[i] == ' ') {
                    sorter++;
                    while(true) {
                        if(symbolLine[i] == ' '){
                            i++;
                        }
                        else {
                            i--;
                            break;
                        }
                    }
                }
                else if(sorter == 0) {
                    litName[litArrayLength] = litName[litArrayLength] + symbolLine[i];
                }
                else if(sorter == 1) {
                    litConst[litArrayLength] = litConst[litArrayLength] + symbolLine[i];
                }
                else if(sorter == 2) {
                    litLength[litArrayLength] = litLength[litArrayLength] + symbolLine[i];
                }
                else if(sorter == 3) {
                    litAddress[litArrayLength] = litAddress[litArrayLength] + symbolLine[i];
                }
            }
        }
        litArrayLength++;
        sorter = 0;
        getline(symbolFile, symbolLine);
    }
    //reads objectFile to parse
    while (true) {
        getline(objectFile, objectLine);
        if(objectLine[0] == 'H') {
            writeHeader(objectLine);
        }
        else if(objectLine[0] == 'T') {
            writeText(objectLine);
        }
        else if(objectLine[0] == 'E') {
            writeEnd(objectLine);
            break;
        }
    }
    writeFile();
}

void dissem::writeHeader(std::string line) {
    //starting address
    for(int i = 13; i < line.length(); i++) {
        end = end + line[i];
    }
    for(int i = 12; i > 7; i--) {
        if(output.length() == 4) {
            break;
        }
        else {
            output = line[i] + output;
        }
    }
    output = output + "    ";
    //title in header
    for(int i = 1; i < 7; i++) {
        if(!(isdigit(line[i]))) {
            output = output + line[i];
            header = header + line[i];
        }
    }
    output = output + "    " + "START";
    output = output + "    " + "0" + "\n";
    outputFile << output;
}

void dissem::writeText(std::string line) {
    std::string temp;
    std::string lengthOfTextRecord = "";
    int loc = hexToDeci(line.substr(1,6));
    if(pcCounter != loc) {
        hole(loc);
    }
    lengthOfTextRecord = lengthOfTextRecord + line[7];
    lengthOfTextRecord = lengthOfTextRecord + line[8];
    int length = hexToDeci(lengthOfTextRecord);
    length = (length * 2) + 8;
    for(int i = 9; i < length; i++) {
        if(literalPicker(pcCounter) != -1) {
            if(pcCounter == 2133) {
                outputFile << std::setw(23) << std::setfill(' ') << std::right << "LTORG";
                outputFile << '\n';
                outputFile << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << std::right << pcCounter;
                outputFile << std::setw(15) << std::setfill(' ') << std::right << "*";
                outputFile << std::setw(18) << std::setfill(' ') << std::right << litConst[literalPicker(pcCounter)];
                outputFile << "    ";
                for(int j = 0; j < std::stoi(litLength[literalPicker(pcCounter)]); j++) {
                    outputFile << line[i];
                    i++;
                }
                i--;
                outputFile << '\n';
                pcCounter += std::stoi(litLength[literalPicker(pcCounter)])/2;
            }
            else if(literalPicker(pcCounter) == litArrayLength - 1) {
                if(pcCounter < hexToDeci(end)) {
                    outputFile << std::setw(23) << std::setfill(' ') << std::right << "LTORG";
                    outputFile << '\n';
                    outputFile << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << std::right << pcCounter;
                    outputFile << std::setw(15) << std::setfill(' ') << std::right << "*";
                    outputFile << std::setw(18) << std::setfill(' ') << std::right << litConst[literalPicker(pcCounter)];
                    outputFile << "    ";
                    for(int j = 0; j < std::stoi(litLength[literalPicker(pcCounter)]); j++) {
                        outputFile << line[i];
                        i++;
                    }
                    i--;
                    outputFile << '\n';
                    pcCounter += std::stoi(litLength[literalPicker(pcCounter)])/2;
                }
            }
            else {
                int positionInArray = literalPicker(pcCounter);
                outputFile << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << std::right << pcCounter;
                outputFile << "    ";
                outputFile << std::setw(10) << std::setfill(' ') << std::left << litName[positionInArray] + "    ";
                outputFile << std::setw(5) << std::setfill(' ') << std::left << "BYTE";
                outputFile << "    ";
                outputFile << std::setw(14) << std::setfill(' ') << std::left << litConst[positionInArray] + "    ";
                for(int j = 0; j < std::stoi(litLength[positionInArray]); j++) {
                    outputFile << line[i];
                    i++;
                }
                i--;
                outputFile << '\n';
                pcCounter += std::stoi(litLength[positionInArray])/2;
            }
        }
        else {
            temp = temp + line[i];
            i++;
            temp = temp + line[i];
            i++;
            if(temp == "B4") {
                for(int j = 0; j < 2; j++) {
                    temp = temp + line[i];
                    i++;
                }
                writeFormatTwo(temp);
                pcCounter += 2;
            }
            else if( (hexToBinary(line[i])[3]) == '1') {
                for(int j = 0; j < 6; j++) {
                    temp = temp + line[i];
                    i++;
                }
                writeFormatFour(temp);
                pcCounter = pcCounter + 4;
            }
            else {
                for(int j = 0; j < 4; j++) {
                    temp = temp + line[i];
                    i++;
                }
                writeFormatThree(temp);
                pcCounter = pcCounter + 3;
            }
            i--;
            temp = "";
        }
    }
}

void dissem::writeEnd(std::string line) {
    if(pcCounter != hexToDeci(end)) {
        hole(hexToDeci(end));
    }
    outputFile << "                  END      " + header;
}

void dissem::writeFile() {
    objectFile.close();
    symbolFile.close();
    outputFile.close();
}

void dissem::writeFormatTwo(std::string objectCode) {
    outputFile << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << std::right << pcCounter;
    if(symbolPicker(pcCounter) != "-1") {
        outputFile << symbolPicker(pcCounter);
        outputFile << "    ";
    }
    else {
        outputFile << "          ";
    }
    outputFile << std::setw(13) << std::setfill(' ') << std::right << opcodepicker(bintohex(hextobinary(objectCode).substr(0,6) + "00")) + "    ";
    if(opcodepicker(bintohex(hextobinary(objectCode).substr(0,6) + "00")) == "CLEAR") {
        if(objectCode[3] == '0') {
            outputFile << std::setw(14) << std::setfill(' ') << std::left << "A";
        }
    }
    outputFile << objectCode + '\n';
}

void dissem::writeFormatThree(std::string objectCode) {
    int format = 10;
    outputFile << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << std::right << pcCounter;
    if(symbolPicker(pcCounter) != "-1") {
        outputFile << "    ";
        outputFile << std::setw(10) << std::setfill(' ') << std::left << symbolPicker(pcCounter);
    }
    else {
        outputFile << "              ";
    }
    std::string opcode = opcodepicker(bintohex(hextobinary(objectCode).substr(0,6) + "00"));
    outputFile << std::setw(9) << std::setfill(' ') << std::left << opcode;
    //checks for pc addressing
    if(hextobinary(objectCode)[9] == '0' && hextobinary(objectCode)[10] == '1') {
        p = 1;
    }
    //n and i
    if(hextobinary(objectCode)[6] == '1' && hextobinary(objectCode)[7] == '0') {
        outputFile << "@";
        format = 9;
    }
    else if(hextobinary(objectCode)[6] == '0' && hextobinary(objectCode)[7] == '1') {
        outputFile << "#";
        format = 9;
    }
    //b and p
    if(hextobinary(objectCode)[9] == '0' && hextobinary(objectCode)[10] == '0') {
        if(opcode == "LDA" || opcode == "LDX") {
            outputFile << std::setw(format) << std::setfill(' ') << std::left << hexToDeci(objectCode.substr(3,5));
            outputFile << "    ";
        }
        else {
            outputFile << std::setw(format) << std::setfill(' ') << std::left << symbolPicker(operand(objectCode.substr(3,5), 0));
            outputFile << "    ";
        }
    }
    else if(symbolPicker(operand(objectCode.substr(3,5), 0)) != "-1") {
        outputFile << std::setw(format - 4) << std::setfill(' ') << std::left << symbolPicker(operand(objectCode.substr(3,5), 0));
        //x
        if(hextobinary(objectCode)[8] == '1') {
            outputFile << std::setw(8) << std::setfill(' ') << std::left << ",X";
        }
        else {
            outputFile << std::setw(format - 2) << std::setfill(' ') << std::left << " ";
        }
    }
    else if(literalPicker(hexToDeci(operand(objectCode.substr(3,5), 0))) != -1 && symbolPicker(operand(objectCode.substr(3,5), 0)) == "-1") {
        outputFile << std::setw(format) << std::setfill(' ') << std::left << litConst[literalPicker(hexToDeci(operand(objectCode.substr(3,5), 0)))];
        //x
        if(hextobinary(objectCode)[8] == '1') {
            outputFile << ",X    ";
        }
        else {
            outputFile << "    ";  
        }
    }
    outputFile << objectCode + '\n';
    if(opcode == "LDB") {
        outputFile << "BASE    ";
        base = hexToDeci(objectCode.substr(3,5));
        outputFile << symbolPicker(hexToDeci(objectCode.substr(3,6)));
        outputFile << '\n';
    }
    p = 0;
}

void dissem::writeFormatFour(std::string objectCode) {
    int format = 10;
    outputFile << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << std::right << pcCounter;
    outputFile << "    ";
    if(symbolPicker(pcCounter) != "-1") {
        outputFile << std::setw(6) << std::left << std::setfill(' ') << symbolPicker(pcCounter);
        outputFile << "    +";
    }
    else {
        outputFile << "          +";
    }
    std::string opcode = opcodepicker(bintohex(hextobinary(objectCode).substr(0,6) + "00"));
    outputFile << std::setw(8) << std::setfill(' ') << std::left << opcode;
    //checks for pc addressing
    if(hextobinary(objectCode)[9] == '0' && hextobinary(objectCode)[10] == '1') {
        p = 1;
    }
    //n and i
    if(hextobinary(objectCode)[6] == '1' && hextobinary(objectCode)[7] == '0') {
        outputFile << "@";
        format = 9;
    }
    else if(hextobinary(objectCode)[6] == '0' && hextobinary(objectCode)[7] == '1') {
        outputFile << "#";
        format = 9;
    }
    //b and p
    if(hextobinary(objectCode)[9] == '0' && hextobinary(objectCode)[10] == '0') {
        if(opcode == "LDA" || opcode == "LDX") {
            if(literalPicker(hexToDeci(objectCode.substr(3,6))) != -1) {
                outputFile << std::setw(10) << std::setfill(' ') << std::left << litConst[literalPicker(hexToDeci(objectCode.substr(3,6)))];
                outputFile << "    ";
            }
            else {
                outputFile << hexToDeci(objectCode.substr(3,6));
                outputFile << "    ";
            }
        }
        else {
            outputFile << std::setw(13) << std::setfill(' ') << std::left << symbolPicker(hexToDeci(objectCode.substr(3,6)));
        }
    }
    else if(symbolPicker(operand(objectCode.substr(3,6), 0)) != "-1") {
        outputFile << std::setw(format) << std::setfill(' ') << std::left << symbolPicker(operand(objectCode.substr(3,6), 1));
        //x
        if(hextobinary(objectCode)[8] == '1') {
            outputFile << ",X    ";
        }
        else {
            outputFile << "    ";
        }
    }
    else if(literalPicker(hexToDeci(operand(objectCode.substr(3,6), 1))) != -1 && symbolPicker(operand(objectCode.substr(3,6), 1)) == "-1") {
        outputFile << std::setw(format) << std::setfill(' ') << std::left << litName[literalPicker(hexToDeci(operand(objectCode.substr(3,6), 1)))];
        //x
        if(hextobinary(objectCode)[8] == '1') {
            outputFile << ",X    ";
        }
        else {
            outputFile << "    ";  
        }
    }
    outputFile << objectCode + '\n';
    if(opcode == "LDB") {
        outputFile << std::setw(27) << std::setfill(' ') << std::right << "BASE     ";
        base = hexToDeci(objectCode.substr(3,6));
        outputFile << symbolPicker(hexToDeci(objectCode.substr(3,6)));
        outputFile << '\n';
    }
    p = 0;
}

void dissem::hole(int location) {
    int difference = pcCounter;
    for(int i = 0; i < symbolArrayLength - 1; i++) {
        if(hexToDeci(symbolAddress[i]) >= pcCounter && hexToDeci(symbolAddress[i]) < location) {
            pcCounter = hexToDeci(symbolAddress[i]);
            outputFile << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << std::right << pcCounter;
            outputFile << "    ";
            outputFile << std::setw(10) << std::setfill(' ') << std::left << symbolPicker(pcCounter);
            outputFile << std::setw(9) << std::setfill(' ') << std::left << "RESB";
            if(hexToDeci(symbolAddress[i + 1]) < location && ((i + 2) != symbolArrayLength)) {
                outputFile << std::to_string(hexToDeci(symbolAddress[i + 1]) - difference);
                outputFile << '\n';
                difference = pcCounter;
            }
            else if (hexToDeci(symbolAddress[i + 1]) < location && ((i + 2) == symbolArrayLength)) {
                outputFile << std::to_string(location - difference);
                outputFile << '\n';
                pcCounter = location;
            }
            else {
                outputFile << std::to_string(location - hexToDeci(symbolAddress[i]));
                outputFile << '\n';
                pcCounter = location;
            }
        }
    }
}

std::string dissem::hexToBinary(char hex) {
    if(hex == 'A') {
        return "1010";
    }
    else if(hex == 'B') {
        return "1011";
    }
    else if(hex == 'C') {
        return "1100";
    }
    else if(hex == 'D') {
        return "1101";
    }
    else if(hex == 'E') {
        return "1110";
    }
    else if(hex == '0') {
        return "0000";
    }
    else if(hex == '1') {
        return "0001";
    }
    else if(hex == '2') {
        return "0010";
    }
    else if(hex == '3') {
        return "0011";
    }
    else if(hex == '4') {
        return "0100";
    }
    else if(hex == '5') {
        return "0101";
    }
    else if(hex == '6') {
        return "0110";
    }
    else if(hex == '7') {
        return "0111";
    }
    else if(hex == '8') {
        return "1000";
    }
    else { //9
        return "1001";
    }
}

std::string dissem::opcodepicker(std::string hex){
    int i =0;
    std::string command;
    std::string mnemonics[59] = {
            "ADD", "ADDF", "ADDR", "AND", "CLEAR", "COMP",
            "COMPF", "COMPR", "DIV", "DIVF", "DIVR", "FIX",
            "FLOAT", "HIO", "J", "JEQ", "JGT", "JLT",
            "JSUB", "LDA", "LDB", "LDCH", "LDF", "LDL",
            "LDS", "LDT", "LDX", "LPS", "MUL", "MULF",
            "MULR", "NORM", "OR", "RD", "RMO", "RSUB",
            "SHIFTL", "SHIFTR", "SIO", "SSK", "STA", "STB",
            "STCH", "STF", "STI", "STL","STS", "STSW",
            "STT", "STX", "SUB", "SUBF", "SUBR", "SVC",
            "TD", "TIO", "TIX", "TIXR", "WD"
    };
    std::string ops[59] = {
            "18", "58", "90", "40", "B4", "28",
            "88", "A0", "24", "64", "9C", "C4",
            "C0", "F4", "3C", "30", "34", "38",
            "48", "00", "68", "50", "70", "08",
            "6C", "74", "04", "D0", "20", "60",
            "98", "C8", "44", "D8", "AC", "4C",
            "A4", "A8", "F0", "EC", "0C", "78",
            "54", "80", "D4", "14", "7C", "E8",
            "84", "10", "1C", "5C", "94", "B0",
            "E0", "F8", "2C", "B8", "DC"
    };
    for (i=0;i< 59;i++){
        if (hex == ops[i]){
            command = mnemonics[i];
            break;
        }
    }
    return command;

}

std::string dissem::symbolPicker(int counter) {
    for(int i = 0; i < symbolArrayLength; i++) {
        if(counter == hexToDeci(symbolAddress[i])) {
            return symbolName[i];
        }
    }
    return "-1";
}

int dissem::literalPicker(int counter) {
    for(int i = 0; i < litArrayLength; i++) {
        if(counter == hexToDeci(litAddress[i])) {
            return i;
        }
    }
    return -1;
}

std::string dissem::symbolPicker(std::string disp) {
    for(int i = 0; i < symbolArrayLength; i++) {
        if(disp == symbolAddress[i]) {
            return symbolName[i];
        }
    }
    return "-1";
}

int dissem::hexToDeci(std::string hex) {
    int deci = 0;
    for(int i = hex.length() - 1; i > -1; i--) {
        if(hex[i] == 'A') {
            deci = deci + (10 * pow(16,hex.length() - (i + 1)));
        }
        else if(hex[i] == 'B') {
            deci = deci + (11 * pow(16,hex.length() - (i + 1)));
        }
        else if(hex[i] == 'C') {
            deci = deci + (12 * pow(16,hex.length() - (i + 1)));
        }
        else if(hex[i] == 'D') {
            deci = deci + (13 * pow(16,hex.length() - (i + 1)));
        }
        else if(hex[i] == 'E') {
            deci = deci + (14 * pow(16,hex.length() - (i + 1)));
        }
        else if(hex[i] == 'F') {
            deci = deci + (15 * pow(16,hex.length() - (i + 1)));
        }
        else {
            deci = deci + ((int) (hex[i] - '0') * pow(16,hex.length() - (i + 1)));
        }
    }
    return deci;

}
std::string dissem::bintohex(std::string binaryvalue){
    int endlength = 4;
    int i = 0;
    std::string endinghexval;
    std::string copystring;
    copystring.append(binaryvalue);

    for (endlength ;i<binaryvalue.length();endlength+3){
        std::string first = copystring.substr(i,endlength);
        if (first =="0000") {
            endinghexval.append("0");
        }
        else if (first =="0001"){
            endinghexval.append("1");
        }
        else if(first =="0010"){
            endinghexval.append("2");

        }
        else if(first == "0011"){
            endinghexval.append("3");

        }
        else if(first =="0100"){
            endinghexval.append("4");

        }
        else if(first == "0101"){
            endinghexval.append("5");
        }
        else if(first == "0110"){
            endinghexval.append("6");
        }
        else if(first =="0111"){
            endinghexval.append("7");
        }
        else if(first == "1000"){
            endinghexval.append("8");
        }
        else if(first =="1001"){
            endinghexval.append("9");
        }
        else if(first =="1010"){
            endinghexval.append("A");
        }
        else if(first == "1011"){
            endinghexval.append("B");
        }
        else if(first == "1100"){
            endinghexval.append("C");
        }
        else if(first == "1101"){
            endinghexval.append("D");

        }
        else if(first == "1110"){
            endinghexval.append("E");

        }
        else if(first == "1111"){
            endinghexval.append("F");
        }
        i = i+4;
    }
    return endinghexval;
}
std::string dissem::hextobinary(std::string hex){
    std::string bin;
    int i;
    for (i = 0;i<hex.length();i++)
        switch (hex[i]) {
            case '0':
                bin.append("0000");
                break;
            case '1':
                bin.append("0001");
                break;
            case'2':
                bin.append("0010");
                break;
            case '3':
                bin.append("0011");
                break;
            case '4':
                bin.append("0100");
                break;
            case'5':
                bin.append("0101");
                break;
            case '6':
                bin.append("0110");
                break;
            case '7':
                bin.append("0111");
                break;
            case'8':
                bin.append("1000");
                break;
            case '9':
                bin.append("1001");
                break;
            case 'A':
                bin.append("1010");
                break;
            case'B':
                bin.append("1011");
                break;
            case 'C':
                bin.append("1100");
                break;
            case 'D':
                bin.append("1101");
                break;
            case'E':
                bin.append("1110");
                break;
            case 'F':
                bin.append("1111");
                break;
        }

    return bin;
}
std::string dissem::operand(std::string disp,int format){
    //where we are going to be keeping our returned operand
    std::string operandsym;
    std::string computation;
    std::string mimicstring;
    //std::cout<<"inputted disp "<<disp<<"\n";
    int range = 0;
    int complement = 0;
    int calc = 0;
    //we will need something like a format 2 checker for
    //if format 2 check for register that was loaded
//    if (format = 2){
//        if (disp[0] == '0' ){
//            operandsym = "A";
//        }
//    }
    //Format 3
    if (format == 0) {
        if(disp == "000" && p == 0) {
            calc = base;
        }
        else if (disp[0] == 'F') {
            //do 2's complement
            computation = hextobinary(disp);
            for (range; range < computation.length(); range++) {
                if (computation[range] == '1') {
                    mimicstring.append("0");
                } else {
                    mimicstring.append("1");
                }

            }
            computation = bintohex(mimicstring);
            complement = stoi(computation, 0, 16);
            calc = complement + 1;
            //TA = disp - pc
            calc = pcCounter - calc + 3;

        } else if (disp[0] != 'F') {
            calc = stoi(disp, 0, 16);
            calc = calc + pcCounter + 3;
        }

    }


        //format 4
    else if (format == 1) {
        if(disp == "000" && p == 0) {
            calc = base;
        }
        else if (disp[0] == 'F') {
            //do 2's complement
            computation = hextobinary(disp);
            for (range; range < computation.length(); range++) {
                if (computation[range] == '1') {
                    mimicstring.append("0");
                }
                else {
                    mimicstring.append("1");
                }

            }
            computation = bintohex(mimicstring);
            complement = stoi(computation, 0, 16);
            calc = complement + 1;
            //TA = disp - pc
            calc = pcCounter - calc + 4;


        }
        else if (disp[0] != 'F') {
            calc = stoi(disp, 0, 16);
            calc = calc + pcCounter + 4;
        }
    }
    std::stringstream newhex;
    newhex << std::hex << calc;
    std::string result(newhex.str());
    //makes sure automated hex is turned to uppercase
    for (range = 0; range < result.length(); range++) {
        if (result[range] > 97) {
            result[range] = toupper(result[range]);
        }
    }
    //place the search here for symbol
    //std::cout<<result;
    for(int i = 0; i < 3; i++) {
        result = "0" + result;
    }
    //std::cout<<"this is the result "<<result<<"\n";

    return result;

}