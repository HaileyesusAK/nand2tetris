#ifndef __VM_TRANSLATOR__
#define __VM_TRANSLATOR__

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "stack_segment.hpp"

using AsmInst = std::vector<std::string>;
using StackCodeMap = std::unordered_map<std::string, std::shared_ptr<StackSegment>>; 
namespace fs = std::filesystem;

enum class AluOperator { PLUS, OR, AND };
enum class RelOperator { EQ, GT, LT };
enum class StackOpCode { PUSH, POP };

class Translator {
    private:
        static const int ARG_START = 5;
        std::ofstream outputFile;
        std::string currentFunctionName;
        static inline StackCodeMap STACK_MAP {
            {"argument", std::make_shared<ArgumentSegment>()},
            {"constant", std::make_shared<ConstantSegment>()},
            {"local", std::make_shared<LocalSegment>()},
            {"pointer", std::make_shared<PointerSegment>()},
            {"static", std::make_shared<StaticSegment>()},
            {"temp", std::make_shared<TempSegment>()},
            {"that", std::make_shared<ThatSegment>()},
            {"this", std::make_shared<ThisSegment>()},
        };
        static const inline std::vector<std::string> STATE_SEGMENTS {"LCL", "ARG", "THIS", "THAT"};
        std::vector<std::string> split(const std::string& s);
        void appendOutputLine(const std::string& s); 
        void inline output(const std::string& s); 

    public:
        void setFileName(const std::string& s);
        void setOutputFile(const fs::path& path);
        void setCurrentFunctionName(const std::string& funcName);
        void translateAlu(const AluOperator& op);
        void translateRel(const RelOperator& op);
        void translate(const std::string& cmd);
        void translate(const std::string& cmd, const std::string& arg1);
        void translate(const std::string& cmd, const std::string& arg1, uint16_t arg2);
        void translate_file(const fs::path& vmPath);
        void translateAdd();
        void translateSub();
        void translateAnd();
        void translateOr();
        void translateEq();
        void translateGt();
        void translateLt();
        void translateNeg();
        void translateNot();
        void translateBootstrap();
        void translateCall(const std::string& fName, uint16_t nArgs);
        void translateClose();
        void translateFunction(const std::string& name, uint16_t nLocals);
        void translateIfGoto(const std::string& label);
        void translateGoto(const std::string& label);
        void translateLabel(const std::string& label);
        void translateReturn();
        void translateStackPush(const std::string& segment, uint16_t idx);
        void translateStackPop(const std::string& segment, uint16_t idx);
        void translate(const fs::path& inputPath);
};

#endif
