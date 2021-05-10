#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <functional>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>
#include "translator.hpp"
#include "stack_segment.hpp"

void Translator::translateAlu(const AluOperator& op) {
    const std::unordered_map<AluOperator, std::string> opMap{
        {AluOperator::PLUS, "+"},
        {AluOperator::OR, "|"},
        {AluOperator::AND, "&"} 
    };

    output("@SP");
    output("AM=M-1");
    output("D=M");
    output("@SP");
    output("A=M-1");

    auto c = opMap.at(op);
    output("M=D" + c + "M");
}

void Translator::translateAdd() {
    translateAlu(AluOperator::PLUS);
}

void Translator::translateAnd() {
    translateAlu(AluOperator::AND);
}

void Translator::translateOr() {
    translateAlu(AluOperator::OR);
}

void Translator::translateSub() {
    output("@SP");
    output("AM=M-1");
    output("D=M");
    output("@SP");
    output("A=M-1");
    output("M=M-D");
}

void Translator::translateBootstrap() {
    //256 + 5, 5
    output("@261");
    output("D=A");
    output("@SP");
    output("M=D");
    output("@Sys.init");
    output("0;JMP");
}

void Translator::translateCall(const std::string& fName, uint16_t nArgs) {
    /*
        push return-address
        push LCL
        push ARG
        push THIS
        push THAT
        ARG = SP - n - 5
        LCL = SP
        goto f_name
        (return_address)
    */

    static int nCalls = 1;
    std::string retAddr = fName + "$end" + std::to_string(nCalls++);

    //Save return address
    output("@" + retAddr );
    output("D=A");
    output("@SP");
    output("A=M");
    output("M=D");
    output("@SP");
    output("M=M+1");

    //Save caller's state
    for(auto& segment: STATE_SEGMENTS) {
        output("@" + segment );
        output("D=M");
        output("@SP");
        output("A=M");
        output("M=D");
        output("@SP");
        output("M=M+1");
    }

    //Setup ARG for the callee
    output("@" + std::to_string(ARG_START) );
    output("D=A");
    output("@" + std::to_string(static_cast<int>(nArgs)));
    output("D=D+A");
    output("@SP");
    output("D=M-D");
    output("@ARG");
    output("M=D");

    //Setup LCL for the callee
    output("@SP");
    output("D=M");
    output("@LCL");
    output("M=D");

    //Jump to the called function
    output("@" + fName );
    output("0;JMP");

    //Label end of function
    outputFile << "(" + retAddr + ")" << std::endl;
}

void Translator::translateClose() {
    outputFile << "(END)" << std::endl;
    output("@END");
    output("0; JMP");
}

void Translator::translateFunction(const std::string& fName, uint16_t nLocals){
    /*   
        create a label (f_name)
        push constant 0 n_locals times
    */
    std::string loop = fName + std::string("_SET_LCL");
    std::string end = fName + std::string("_SET_LCL_END");
    setCurrentFunctionName(fName);
    
    // Label the function's starting point
    outputFile << "(" + fName + ")" << std::endl;

    //Setup loop variables
    output("@LCL");
    output("D=M");
    output("@i");
    output("M=D");
    output("@" + std::to_string(static_cast<int>(nLocals)));
    output("D=D+A");
    output("@n");
    output("M=D");

    //Begin loop
    outputFile << "(" + loop +")" << std::endl;

    //Check for loop termination condition
    output("@n");
    output("D=M");
    output("@i");
    output("D=M-D");
    output("@" + end );
    output("D;JGE");

    //Push 0 onto the local segment
    output("@i");
    output("A=M");
    output("M=0");

    //Increment SP and loop variable and goto the beginning;
    output("@i");
    output("M=M+1");
    output("@SP");
    output("M=M+1");
    output("@" + loop );
    output("0;JMP");

    //End loop
    outputFile << "(" + end + ")" << std::endl;
}

void Translator::translateIfGoto(const std::string& label) {
    output("@SP");
    output("AM=M-1");
    output("D=M");
    output("@" + currentFunctionName + "$" + label );
    output("D;JNE");
}

void Translator::translateGoto(const std::string& label) {
    output("@" + currentFunctionName + "$" + label );
    output("0;JMP");
}

void Translator::translateLabel(const std::string& label) {
    outputFile << "(" + currentFunctionName + "$" + label + ")" << std::endl;
}

void Translator::translateRel(const RelOperator& op) {
    static uint64_t i = 1;
    std::string jmpInst;
    std::string endLabel("END_REL_OP_" + std::to_string(i++));

    switch(op) {
        case RelOperator::EQ:
            jmpInst = "D;JEQ"; break;

        case RelOperator::LT:
            jmpInst = "D;JLT"; break;

        case RelOperator::GT:
            jmpInst = "D;JGT"; break;
        
        default:
            break;
    }

    output("@SP");
    output("AM=M-1");
    output("D=M");
    output("@SP");
    output("A=M-1");
    output("D=M-D");
    output("M=-1");
    output("@" + endLabel );
    output(jmpInst);
    output("@SP");
    output("A=M-1");
    output("M=0");
    output("(" + endLabel + ")");
}

void Translator::translateEq() { translateRel(RelOperator::EQ); }

void Translator::translateGt() { translateRel(RelOperator::GT); }

void Translator::translateLt() { translateRel(RelOperator::LT); }

void Translator::translateReturn() {
    /*
        frame = LCL
        ret = *(frame - 5)
        *ARG = pop()
        SP = ARG + 1
        THAT = *(frame - 1)
        THIS = *(frame - 2)
        ARG = *(frame - 3)
        LCL = *(frame - 4)

        goto ret
    */

    //Save the current function's LCL in a variable
    output("@LCL");
    output("D=M");
    output("@frame");
    output("M=D");

    //Save the return address in a variable
    output("@" + std::to_string(ARG_START));
    output("D=A");
    output("@frame");
    output("A=M-D");
    output("D=M");
    output("@ret");
    output("M=D");

    //Return value to the caller
    output("@SP");
    output("A=M-1");
    output("D=M");
    output("@ARG");
    output("A=M");
    output("M=D");

    //Update SP
    output("@ARG");
    output("D=M+1");
    output("@SP");
    output("M=D");

    //Reset caller's state in reverse order
    for(auto it = STATE_SEGMENTS.rbegin(); it != STATE_SEGMENTS.rend(); ++it) {
        output("@frame");
        output("AM=M-1");
        output("D=M");
        output("@" + *it );
        output("M=D");
    }

    //Return control to the caller
    output("@ret");
    output("A=M");
    output("0;JMP");
}


void Translator::translate(const std::string& cmd, const std::string& arg1, uint16_t arg2) {
    //TODO: make static and solve memory leak issue
    const std::unordered_map<std::string, void(Translator::*)(const std::string&, uint16_t)> generatorMap {
        {"pop", &Translator::translateStackPop},
        {"push", &Translator::translateStackPush},
        {"call", &Translator::translateCall},
        {"function", &Translator::translateFunction}
    };
    
    try {
        auto generator = generatorMap.at(cmd);
        (this->*generator)(arg1, arg2);
    }
    catch (const std::out_of_range& exp) {
        throw std::out_of_range("Unknown command '" + cmd + "'");
    }
}

void Translator::translate(const std::string& cmd, const std::string& arg) {
    const std::unordered_map<std::string, void(Translator::*)(const std::string&)> generatorMap {
        {"label", &Translator::translateLabel},
        {"if-goto", &Translator::translateIfGoto},
        {"goto", &Translator::translateGoto},
    };
    
    try {
        auto generator = generatorMap.at(cmd);
        (this->*generator)(arg);
    }
    catch (const std::out_of_range& exp) {
        throw std::out_of_range("Unknown command '" + cmd + "'");
    }
}

void Translator::translate(const std::string& cmd) {
    const std::unordered_map<std::string, void(Translator::*)()> generatorMap {
        {"add", &Translator::translateAdd},
        {"sub", &Translator::translateSub},
        {"and", &Translator::translateAnd},
        {"eq", &Translator::translateEq},
        {"gt", &Translator::translateGt},
        {"lt", &Translator::translateLt},
        {"neg", &Translator::translateNeg},
        {"not", &Translator::translateNot},
        {"or", &Translator::translateOr},
        {"return", &Translator::translateReturn}
    };
    
    try {
        auto generator = generatorMap.at(cmd);
        (this->*generator)();
    }
    catch (const std::out_of_range& exp) {
        throw std::out_of_range("Unknown command '" + cmd + "'");
    }
}

void Translator::translateStackPush(const std::string& segment, uint16_t idx) {
    try {
        const auto stackStackSegment = STACK_MAP.at(segment); 
        for(const auto& inst : stackStackSegment->push(idx))
            output(inst);
    }
    catch (std::out_of_range& ex) {
        throw std::invalid_argument("Unknown stack segment: '" + segment + "'");
    }
}

void Translator::translateStackPop(const std::string& segment, uint16_t idx) {
    try {
        const auto stackStackSegment = STACK_MAP.at(segment); 
        for(const auto& inst : stackStackSegment->pop(idx))
            output(inst);
    }
    catch (std::out_of_range& ex) {
        throw std::invalid_argument("Unknown stack segment: '" + segment + "'");
    }
}

void Translator::translateNot() {
    output("@SP");
    output("A=M-1");
    output("M=!M");
}

void Translator::translateNeg() {
	output("@SP");
	output("A=M-1");
	output("M=-M");
}

void Translator::translate_file(const fs::path& vmPath) {
    std::ifstream infile(vmPath);
    if(!infile.is_open())
        throw std::runtime_error(std::strerror(errno) + std::string(": ") + vmPath.string());

    setFileName(vmPath.filename());

    std::string s;
    while(std::getline(infile, s)) {
        std::string line(s.substr(0, s.find_first_of('/')));
        line = line.substr(0, s.find_first_of('\r'));
        auto parameters = split(line);
        
        if(parameters.empty())
            continue;

        appendOutputLine("//" + line);
        switch(parameters.size()) {
            case 1:
                translate(parameters[0]);
                break;

            case 2:
                translate(parameters[0], parameters[1]);
                break;

            case 3:
                translate(parameters[0], parameters[1], static_cast<uint16_t>(std::stoul(parameters[2])));
                break;

            default:
                throw std::invalid_argument("Unknown vm instruction: '" + line + "'");
        }
    }
}

void Translator::translate(const fs::path& inputPath) {
    if(fs::is_directory(inputPath)) {
        setOutputFile(inputPath / inputPath.filename());
        translateBootstrap();

        std::for_each(fs::directory_iterator(inputPath),
                      fs::directory_iterator(),
                      [this](const fs::path& p) {
                            if(!fs::is_directory(p) && p.extension() == ".vm") {
                                translate_file(p);
                            }
                      });
    }
    else {
        setOutputFile(inputPath);
        translate_file(inputPath);
    }

    translateClose();
}

void Translator::setCurrentFunctionName(const std::string& funcName) {
    currentFunctionName = funcName;
}

void Translator::setOutputFile(const fs::path& path) {
    auto asmPath = path;
    asmPath.replace_extension(".asm");
    outputFile = std::ofstream(asmPath);
}

void Translator::appendOutputLine(const std::string& s) {
    outputFile << s << std::endl;
}

void Translator::output(const std::string& s) {
    outputFile << "\t" << s << std::endl;
}

void Translator::setFileName(const std::string& s) {
    auto* segment = std::dynamic_pointer_cast<StaticSegment>(STACK_MAP.at("static")).get();
    segment->setFileName(s);
}

std::vector<std::string> Translator::split(const std::string& s) {
    std::vector<std::string> words;
    std::string word;
    std::stringstream ss(s);
    while(getline(ss, word, ' '))
        if(word.length() > 0)
            words.push_back(word);
    return words;
}
