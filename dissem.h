#ifndef dissem_h
#define dissem_h

class dissem {
public:
    std::string bintohex(std::string binaryvalue);
    std::string hextobinary(std::string hex);
    void openObjectFile(std::string input);
    void openSymbolFile(std::string input);
    void readFile();
    void writeHeader(std::string line);
    void writeText(std::string line);
    void writeEnd(std::string line);
    void writeFile();
    void writeFormatTwo(std::string objectCode);
    void writeFormatThree(std::string objectCode);
    void writeFormatFour(std::string objectCode);
    void hole(int location);
    std::string symbolPicker(int counter);
    std::string symbolPicker(std::string disp);
    int literalPicker(int counter);
    std::string hexToBinary(char hex);
    int hexToDeci(std::string hex);
    std::string operandmethod(std::string TA);
    std::string opcodepicker(std::string hex);
    std::string operand(std::string disp,int format);
};

#endif